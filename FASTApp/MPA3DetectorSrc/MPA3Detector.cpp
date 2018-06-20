/* MPA3Detector.cpp
 *
 * This is a driver for the FAST ComTec MPA3 Analyzer (Quantar MCP Detector)
 * Based on the ADCSimDetector by Mark Rivers (UofC)
 * Author: Joe Sullivan
 *         Argonne National Laboratory
 *
 * Created:  February 10, 2017
 *
 */

#include <stdlib.h>
#include <math.h>

#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include "asynNDArrayDriver.h"
#include <epicsExport.h>
#include "MPA3Detector.h"

static const char *driverName = "MPA3Detector";

// Some systems don't define M_PI in math.h
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

static void mpaTaskC(void *drvPvt)
{
    MPA3Detector *pPvt = (MPA3Detector *)drvPvt;

    pPvt->mpaTask();
}

/** Constructor for MPA3Detector; most parameters are simply passed to ADDriver::ADDriver.
  * After calling the base class constructor this method creates a thread to compute the simulated detector data,
  * and sets reasonable default values for parameters defined in this class, asynNDArrayDriver and ADDriver.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] numTimePoints The initial number of time points.
  * \param[in] dataType The initial data type (NDDataType_t) of the arrays that this driver will create.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
MPA3Detector::MPA3Detector(const char *portName, int numTimePoints, NDDataType_t dataType,
                               int maxBuffers, size_t maxMemory, int priority, int stackSize)

    : asynNDArrayDriver(portName, MAX_SIGNALS, NUM_MPA_DETECTOR_PARAMS, maxBuffers, maxMemory,
               0, 0, /* No interfaces beyond those set in ADDriver.cpp */
               ASYN_CANBLOCK | ASYN_MULTIDEVICE, /* asyn flags*/
               1,                                /* autoConnect=1 */
               priority, stackSize),
    uniqueId_(0), acquiring_(0)

{
    int status = asynSuccess;
    int ErrSet;
    const char *functionName = "MPA3Detector";

    hDLL = LoadLibrary("DMPA3.DLL");
    if(hDLL){
        lpRun=(IMPARUNCMD)GetProcAddress(hDLL,"RunCmd");
        lpErase=(IMPAERASE)GetProcAddress(hDLL,"Erase");
        lpHalt=(IMPAHALT)GetProcAddress(hDLL,"Halt");
        lpStart=(IMPASTART)GetProcAddress(hDLL,"Start");
	lpNewSetting=(IMPANEWSET)GetProcAddress(hDLL,"NewSetting");
        lpStat=(IMPAGETSTATUS)GetProcAddress(hDLL,"GetStatusData");
	lpNewStat=(IMPANEWSTATUS)GetProcAddress(hDLL,"GetStatus");
	lpSet=(IMPAGETSETTING)GetProcAddress(hDLL,"GetSettingData");
	lpGetDatSet=(IMPAGETDATSET)GetProcAddress(hDLL,"GetDatSetting");
	lpGetMp3Set=(IMPAGETMP3SET)GetProcAddress(hDLL,"GetMP3Setting");
    }
    else { 
        printf("%s:%s Windows LoadLibray Failure\n",
	       driverName, functionName);
        return;
    }

    // Initialize parameters
    ErrSet = (*lpNewStat)(0);
    ErrSet = (*lpStat)(&Status, 0);
    //    if (ErrSet) {
    if (NULL) {
        printf("%s:%s DLL getStatus Failure\n",
	       driverName, functionName);
        return;
    }
    PrintMpaStatus(&Status);

    getSettings();
    if (NULL) {
        printf("%s:%s DLL getSetting Failure\n",
	       driverName, functionName);
        return;
    }
    

    /* Create the epicsEvents for signaling to the simulate task when acquisition starts and stops */
    this->startEventId_ = epicsEventCreate(epicsEventEmpty);
    if (!this->startEventId_) {
        printf("%s:%s epicsEventCreate failure for start event\n",
            driverName, functionName);
        return;
    }
    this->stopEventId_ = epicsEventCreate(epicsEventEmpty);
    if (!this->stopEventId_) {
        printf("%s:%s epicsEventCreate failure for stop event\n",
            driverName, functionName);
        return;
    }

    createParam(mpaAcquireString,       asynParamInt32, &P_Acquire);
    createParam(mpaAutoIncrementString, asynParamInt32, &P_AutoIncrement);
    createParam(mpaAutoSaveString,      asynParamInt32, &P_AutoSave);
    createParam(mpaRunTimeEnableString, asynParamInt32, &P_RunTimeEnable);
    createParam(mpaFilenameString,      asynParamOctet,  &P_MPAFilename);

    createParam(mpaAcquireTimeString,   asynParamFloat64, &P_AcquireTime);
    createParam(mpaElapsedTimeString,   asynParamFloat64, &P_ElapsedTime);
    createParam(mpaTimeStepString,      asynParamFloat64, &P_TimeStep);
    createParam(mpaNumTimePointsString, asynParamInt32, &P_NumTimePoints);
    createParam(mpaPeriodString,        asynParamFloat64, &P_Period);
    createParam(mpaAmplitudeString,     asynParamFloat64, &P_Amplitude);
    createParam(mpaOffsetString,        asynParamFloat64, &P_Offset);
    createParam(mpaFrequencyString,     asynParamFloat64, &P_Frequency);
    createParam(mpaPhaseString,         asynParamFloat64, &P_Phase);
    createParam(mpaNoiseString,         asynParamFloat64, &P_Noise);
    createParam(mpaCommandString,       asynParamOctet,   &P_Command);

    status |= setIntegerParam(P_NumTimePoints, numTimePoints);
    status |= setIntegerParam(NDDataType, dataType);
    status |= setDoubleParam(P_TimeStep, 0.001);
    status |= setDoubleParam(P_Amplitude, 1.0);
    status |= setDoubleParam(P_Offset,    0.0);
    status |= setDoubleParam(P_Period,    1.0);
    status |= setDoubleParam(P_Phase,     0.0);
    status |= setDoubleParam(P_Noise,     0.0);

    if (status) {
        printf("%s: unable to set parameters\n", functionName);
        return;
    }

    /* Create the thread that updates the images */
    status = (epicsThreadCreate("mpaDetTask",
                                epicsThreadPriorityMedium,
                                epicsThreadGetStackSize(epicsThreadStackMedium),
                                (EPICSTHREADFUNC)mpaTaskC,
                                this) == NULL);
    if (status) {
        printf("%s:%s epicsThreadCreate failure for simulation task\n",
            driverName, functionName);
        return;
    }
}

/**
 * Destructor. Free Library 
 */
MPA3Detector::~MPA3Detector()
{
  FreeLibrary(hDLL);
}


int MPA3Detector::getStatus()
{
  int ErrSet;        
 
  ErrSet = (*lpStat)(&Status, 0);
  PrintMpaStatus(&Status);
  return(Status.val);
}


void MPA3Detector::sendCommand(char * sndCmnd)
{

  printf("%s\n",sndCmnd);
  (*lpRun)(0,sndCmnd);
  (*lpNewSetting)(0);

}



int MPA3Detector::getSettings()
{
  int ErrSet;        
 
  ErrSet = (*lpSet)(&Setting, 0);
  //  if (ErrSet) return(ErrSet);
  PrintSetting(&Setting);

  ErrSet = (*lpGetMp3Set)(&MP3Setting);
  //  if (ErrSet) return(ErrSet);
  PrintMP3Setting(&MP3Setting);

  ErrSet = (*lpGetDatSet)(&DatSetting);
  //  if (ErrSet) return(ErrSet);
  PrintDatSetting(&DatSetting);

  return(0);
}



void MPA3Detector::PrintMpaStatus(ACQSTATUS *Stat)
{
if(Stat->val) printf("ON\n"); else printf("OFF\n");
printf("realtime= %.2lf\n", Stat->cnt[ST_REALTIME]);
printf("runtime= %.2lf\n", Stat->cnt[ST_RUNTIME]);
printf("single_ev= %lf\n", Stat->cnt[ST_SINGLESUM]);
printf("coinc_ev= %lf\n", Stat->cnt[ST_COINCSUM]);
printf("sglrate= %.2lf\n", Stat->cnt[ST_SGLRATE]);
printf("coirate= %.2lf\n", Stat->cnt[ST_COIRATE]);
}

void MPA3Detector::PrintStatus(ACQSTATUS *Stat)
{
printf("livetime= %.2lf\n", Stat->cnt[ST_LIVETIME]);
printf("deadtime%%= %.2lf\n", Stat->cnt[ST_DEADTIME]);
printf("totalsum= %lf\n", Stat->cnt[ST_TOTALSUM]);
printf("roisum= %lf\n", Stat->cnt[ST_ROISUM]);
printf("netsum= %lf\n", Stat->cnt[ST_NETSUM]);
printf("totalrate= %.2lf\n", Stat->cnt[ST_TOTALRATE]);
}

void MPA3Detector::PrintDatSetting(DATSETTING *Set)
{
printf("savedata= %d\n", Set->savedata);
printf("autoinc= %d\n", Set->autoinc);
printf("fmt= %d\n", Set->fmt);
printf("sepfmt= %d\n", Set->sepfmt);
printf("sephead= %d\n", Set->sephead);
printf("filename= %s\n", Set->filename);
}

void MPA3Detector::PrintMP3Setting(ACQMP3 *Set)
{
printf("rtcuse= %x\n", Set->rtcuse);
printf("dac= %d\n", Set->dac);
printf("diguse= %x\n", Set->diguse);
printf("digval= %d\n", Set->digval);
printf("rtprena= %x\n", Set->rtprena);
printf("rtpreset= %lg\n", Set->rtpreset);
}

void MPA3Detector::PrintSetting(ACQSETTING *Set)
{
printf("range= %ld\n", Set->range);
printf("prena= %x\n", Set->prena);
printf("roimin= %ld\n", Set->roimin);
printf("roimax= %ld\n", Set->roimax);
printf("nregions= %d\n", Set->nregions);
printf("caluse= %d\n", Set->caluse);
printf("calpoints= %d\n", Set->calpoints);
printf("param= %lx\n", Set->param);
printf("offset= %lx\n", Set->offset);
printf("xdim= %d\n", Set->xdim);
printf("timesh= %d\n", Set->timesh);
printf("active= %x\n", Set->active);
printf("roipreset= %lg\n", Set->roipreset);
printf("ltpreset= %lg\n", Set->ltpreset);
printf("timeoffs= %lg\n", Set->timeoffs);
printf("dwelltime= %lg\n", Set->dwelltime);
}



/** Template function to compute the simulated detector data for any data type */
template <typename epicsType> void MPA3Detector::computeArraysT()
{
    size_t dims[2];
    int numTimePoints;
    int i, j;
    NDDataType_t dataType;
    epicsType *pData;
    double acquireTime;
    double timeStep;
    double rndm;
    double amplitude[MAX_SIGNALS];
    double period[MAX_SIGNALS];
    double frequency[MAX_SIGNALS];
    double phase[MAX_SIGNALS];
    double noise[MAX_SIGNALS];
    double offset[MAX_SIGNALS];
    
    getIntegerParam(NDDataType, (int *)&dataType);
    getIntegerParam(P_NumTimePoints, &numTimePoints);
    getDoubleParam(P_TimeStep, &timeStep);
    getDoubleParam(P_AcquireTime, &acquireTime);

    dims[0] = MAX_SIGNALS;
    dims[1] = numTimePoints;

    if (this->pArrays[0]) this->pArrays[0]->release();
    this->pArrays[0] = pNDArrayPool->alloc(2, dims, dataType, 0, 0);
    pData = (epicsType *)this->pArrays[0]->pData;
    memset(pData, 0, MAX_SIGNALS * numTimePoints * sizeof(epicsType));

    for (j=0; j<MAX_SIGNALS; j++) {
        getDoubleParam(j, P_Amplitude, amplitude+j);
        getDoubleParam(j, P_Offset,    offset+j);
        getDoubleParam(j, P_Period,    period+j);
        frequency[j] = 1. / period[j];
        setDoubleParam(j, P_Frequency, frequency[j]);
        getDoubleParam(j, P_Phase,     phase+j);
        getDoubleParam(j, P_Noise,     noise+j);
        phase[j] = phase[j]/360.0;
    }
    for (i=0; i<numTimePoints; i++) {
        rndm = 2.*(rand()/(double)RAND_MAX - 0.5);
        // Signal 0 is a sin wave
        j = 0;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] * 
                                               sin((elapsedTime_ * frequency[j] + phase[j]) * 2. * M_PI));
        // Signal 1 is a cos wave
        j = 1;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] * 
                                               cos((elapsedTime_ * frequency[j] + phase[j]) * 2. * M_PI));
        // Signal 2 is a square wave
        j = 2;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] * 
                                              (sin((elapsedTime_ * frequency[j] + phase[j]) * 2. * M_PI) > 0 ? 1.0 : -1.0));
        // Signal 3 is a sawtooth
        j = 3;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] * 
                                               -2.0/M_PI * atan(1./tan((elapsedTime_ * frequency[j] + phase[j]) * M_PI)));
        // Signal 4 is white noise
        j = 4;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] * rndm);

        // Signal 5 is signal 0 + signal 1
        j = 5;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] * 
                                               pData[MAX_SIGNALS*i + 0] + pData[MAX_SIGNALS*i + 1]) ;

        // Signal 6 is signal 0 * signal 1
        j = 6;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] * 
                                               pData[MAX_SIGNALS*i + 0] * pData[MAX_SIGNALS*i + 1]) ;

        // Signal 7 is 4 sin waves
        j = 7;
        pData[MAX_SIGNALS*i + j] = (epicsType)(offset[j] + noise[j] * rndm + amplitude[j] *
                                              (sin((elapsedTime_ * 1.*frequency[j] + phase[j]) * 2. * M_PI) +
                                               sin((elapsedTime_ * 2.*frequency[j] + phase[j]) * 2. * M_PI) +
                                               sin((elapsedTime_ * 3.*frequency[j] + phase[j]) * 2. * M_PI) +
                                               sin((elapsedTime_ * 4.*frequency[j] + phase[j]) * 2. * M_PI)));

        elapsedTime_ += timeStep;
	//        if ((acquireTime > 0) && (elapsedTime_ > acquireTime)) {
	//            setAcquire(0);
	//            setIntegerParam(P_Acquire, 0);
	//            break;
	//        }
    }
    setDoubleParam(P_ElapsedTime, elapsedTime_);
}

/** Computes the new image data */
void MPA3Detector::computeArrays()
{
    int dataType;
    getIntegerParam(NDDataType, &dataType); 

    switch (dataType) {
        case NDInt8:
            computeArraysT<epicsInt8>();
            break;
        case NDUInt8:
            computeArraysT<epicsUInt8>();
            break;
        case NDInt16:
            computeArraysT<epicsInt16>();
            break;
        case NDUInt16:
            computeArraysT<epicsUInt16>();
            break;
        case NDInt32:
            computeArraysT<epicsInt32>();
            break;
        case NDUInt32:
            computeArraysT<epicsUInt32>();
            break;
        case NDFloat32:
            computeArraysT<epicsFloat32>();
            break;
        case NDFloat64:
            computeArraysT<epicsFloat64>();
            break;
    }
}

void MPA3Detector::setAcquire(int value)
{
  long Errset=0;

  if (value && !acquiring_) {
    /* Send Start Command */
    (*lpStart)(0);        

    /* Send an event to wake up the acquitision task */
    epicsEventSignal(this->startEventId_); 
  }
  if (!value && acquiring_) {
    /* This was a command to stop acquisition */
    /* Send Stop Command */
    (*lpHalt)(0);        
    /* Send the stop event */
    epicsEventSignal(this->stopEventId_); 
  }
}

/** This thread calls computeImage to compute new image data and does the callbacks to send it to higher layers.
  * It implements the logic for single, multiple or continuous acquisition. */
void MPA3Detector::mpaTask()
{
    int status = asynSuccess;
    NDArray *pImage;
    epicsTimeStamp startTime;
    //    int numTimePoints;
    int arrayCounter;
    //    double timeStep;
    int acquire;
    int i, devStatus, timeout;
    const char *functionName = "mpaTask";

    this->lock();
    /* Loop forever */
    while (1) {
        /* Has acquisition been stopped? */
        status = epicsEventTryWait(this->stopEventId_);
        if (status == epicsEventWaitOK) {
            acquiring_ = 0;
        }
       
        /* If we are not acquiring then wait for a semaphore that is given when acquisition is started 
	 *   and wait for the detector to actually start acquiring --- it takes about 4 sec. */
        if (!acquiring_) {
          /* Release the lock while we wait for an event that says acquire has started, then lock again */
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                "%s:%s: waiting for acquire to start\n", driverName, functionName);
            this->unlock();
            status = epicsEventWait(this->startEventId_);
            timeout = 12;
            getIntegerParam(P_Acquire, &acquire);
    	    while (acquire && !(devStatus = getStatus()) && timeout--) {
	      epicsThreadSleep(0.5);
              getIntegerParam(P_Acquire, &acquire);
	    }
            this->lock();
            acquiring_ = 1;
            elapsedTime_ = 0.0;
        }

	this->unlock();           
        while ((devStatus = getStatus()) && 
	       ((status = epicsEventTryWait(this->stopEventId_)) != epicsEventWaitOK)) {
	  epicsThreadSleep(1);
	}
	this->lock();

	setAcquire(0);
	setIntegerParam(P_Acquire, 0);

        /* Update the data */
        computeArrays();

        pImage = this->pArrays[0];

        /* Put the frame number and time stamp into the buffer */
        pImage->uniqueId = uniqueId_++;
        getIntegerParam(NDArrayCounter, &arrayCounter);
        arrayCounter++;
        setIntegerParam(NDArrayCounter, arrayCounter);
        epicsTimeGetCurrent(&startTime);
        pImage->timeStamp = startTime.secPastEpoch + startTime.nsec / 1.e9;
        updateTimeStamp(&pImage->epicsTS);

        /* Get any attributes that have been defined for this driver */
        this->getAttributes(pImage->pAttributeList);

        /* Call the NDArray callback */
        /* Must release the lock here, or we can get into a deadlock, because we can
         * block on the plugin lock, and the plugin can be calling us */
        this->unlock();
        doCallbacksGenericPointer(pImage, NDArrayData, 0);
        this->lock();

        /* Call the callbacks to update any changes */
        for (i=0; i<MAX_SIGNALS; i++) {
            callParamCallbacks(i);
        }

        /* Sleep for numTimePoint * timeStep seconds */
	//        getIntegerParam(P_NumTimePoints, &numTimePoints);
	//        getDoubleParam(P_TimeStep, &timeStep);
	//        unlock();
	//        epicsThreadSleep(numTimePoints * timeStep);
	//        lock();
    }
}


/** Called when asyn clients call pasynInt32->write().
  * This function performs actions for some parameters, including ADAcquire, ADColorMode, etc.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus MPA3Detector::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    int enable;

    /* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
     * status at the end, but that's OK */
    status = setIntegerParam(function, value);

    if (function == P_Acquire) {
      /* Do callbacks before delay for MCP Acquire Active so higher layers see any changes */
        setAcquire(value);
    } else if (function == P_RunTimeEnable) {
      enable = (value)?1:0;
      sprintf(command,"rtprena=%d",enable);
      sendCommand(command);
    } else if (function == P_AutoIncrement) {
      enable = (value)?1:0;
      sprintf(command,"autoinc=%d",enable);
      sendCommand(command);
    } else if (function == P_AutoSave) {
      enable = (value)?1:0;
      sprintf(command,"savedata=%d",enable);
      sendCommand(command);
    } else {
        /* If this parameter belongs to a base class call its method */
        if (function < FIRST_MPA_DETECTOR_PARAM) status = asynNDArrayDriver::writeInt32(pasynUser, value);
    }

    /* Do callbacks so higher layers see any changes */
    callParamCallbacks();

    if (status)
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
              "%s:writeInt32 error, status=%d function=%d, value=%d\n",
              driverName, status, function, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:writeInt32: function=%d, value=%d\n",
              driverName, function, value);
    return status;
}

/** Called when asyn clients call pasynFloat64->write().
  * This function performs actions for some parameters, including ADAcquireTime, ADGain, etc.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus MPA3Detector::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;

    /* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
     * status at the end, but that's OK */
    status = setDoubleParam(function, value);

    /* Changing any of the following parameters requires recomputing the base image */
    if (function == P_AcquireTime) {
      sprintf(command,"rtpreset=%.3f",value);
      sendCommand(command);
    } else {
        /* If this parameter belongs to a base class call its method */
        if (function < FIRST_MPA_DETECTOR_PARAM) status = asynNDArrayDriver::writeFloat64(pasynUser, value);
    }

    /* Do callbacks so higher layers see any changes */
    callParamCallbacks();
    if (status)
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
              "%s:writeFloat64 error, status=%d function=%d, value=%f\n",
              driverName, status, function, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:writeFloat64: function=%d, value=%f\n",
              driverName, function, value);
    return status;
}

/** Called when asyn clients call pasynOctet->write().
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. 
  * \param[in] nChars Number of characters to write 
  * \param[out] nActual Number of characters actually written */
asynStatus MPA3Detector::writeOctet(asynUser *pasynUser, const char *value,
                                            size_t nChars, size_t *nActual)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;

    /* Set the parameter in the parameter library. */
    status = setStringParam(function, (char *)value);

    if (function == P_Command) {
      if (strlen(value)+1 < sizeof(command))
	sendCommand((char *)value);
    }
    else if (function == P_MPAFilename) {
      if (strlen(value)+strlen("mapname=") < sizeof(command))
	{
	sprintf(command,"mpaname=%s",value);
	sendCommand((char *)command);
	}
    }
    /* If this is not a parameter we have handled call the base class */
    if (function < FIRST_MPA_DETECTOR_PARAM) 
      status = asynNDArrayDriver::writeOctet(pasynUser, value,nChars, nActual);
 
    /* Update any changed parameters */
    callParamCallbacks();

    if (status) 
        asynPrint(pasynUser, ASYN_TRACE_ERROR, 
              "%s:writeOctet: error, status=%d function=%d, value=%s\n", 
              driverName, status, function, value);
    else        
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
              "%s:writeOctet: function=%d, value=%s\n", 
              driverName, function, value);

    *nActual = nChars;
    return(status); 
}


/** Report status of the driver.
  * Prints details about the driver if details>0.
  * It then calls the ADDriver::report() method.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details If >0 then driver details are printed.
  */
void MPA3Detector::report(FILE *fp, int details)
{

    fprintf(fp, "FAST ComTec MPA3 detector %s\n", this->portName);
    if (details > 0) {
        int numTimePoints, dataType;
        getIntegerParam(P_NumTimePoints, &numTimePoints);
        getIntegerParam(NDDataType, &dataType);
        fprintf(fp, "  # time points:   %d\n", numTimePoints);
        fprintf(fp, "      Data type:   %d\n", dataType);
    }
    /* Invoke the base class method */
    asynNDArrayDriver::report(fp, details);
}


/** Configuration command, called directly or from iocsh */
extern "C" int MPA3DetectorConfig(const char *portName, int numTimePoints, int dataType,
                                 int maxBuffers, int maxMemory, int priority, int stackSize)
{
    new MPA3Detector(portName, numTimePoints, (NDDataType_t)dataType,
                    (maxBuffers < 0) ? 0 : maxBuffers,
                    (maxMemory < 0) ? 0 : maxMemory, 
                    priority, stackSize);
    return(asynSuccess);
}

/** Code for iocsh registration */
static const iocshArg MPA3DetectorConfigArg0 = {"Port name",     iocshArgString};
static const iocshArg MPA3DetectorConfigArg1 = {"# time points", iocshArgInt};
static const iocshArg MPA3DetectorConfigArg2 = {"Data type",     iocshArgInt};
static const iocshArg MPA3DetectorConfigArg3 = {"maxBuffers",    iocshArgInt};
static const iocshArg MPA3DetectorConfigArg4 = {"maxMemory",     iocshArgInt};
static const iocshArg MPA3DetectorConfigArg5 = {"priority",      iocshArgInt};
static const iocshArg MPA3DetectorConfigArg6 = {"stackSize",     iocshArgInt};
static const iocshArg * const MPA3DetectorConfigArgs[] = {&MPA3DetectorConfigArg0,
                                                            &MPA3DetectorConfigArg1,
                                                            &MPA3DetectorConfigArg2,
                                                            &MPA3DetectorConfigArg3,
                                                            &MPA3DetectorConfigArg4,
                                                            &MPA3DetectorConfigArg5,
                                                            &MPA3DetectorConfigArg6};
static const iocshFuncDef configMPA3Detector = {"MPA3DetectorConfig", 7, MPA3DetectorConfigArgs};
static void configMPA3DetectorCallFunc(const iocshArgBuf *args)
{
    MPA3DetectorConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival,
                         args[4].ival, args[5].ival, args[6].ival);
}


static void MPA3DetectorRegister(void)
{

    iocshRegister(&configMPA3Detector, configMPA3DetectorCallFunc);
}

extern "C" {
epicsExportRegistrar(MPA3DetectorRegister);
}
