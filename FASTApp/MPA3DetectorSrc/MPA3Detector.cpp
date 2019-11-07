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
#include <epicsString.h>
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
static void mpaTaskCBackground(void *drvPvt)
{
    MPA3Detector *pPvt = (MPA3Detector *)drvPvt; 
    pPvt->mpaTaskBackground();

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
  * \param[in] channelNum, this specifies which of the 4 different data arrays that we will use by default.
  */
MPA3Detector::MPA3Detector(const char *portName, int numTimePoints, NDDataType_t dataType,
                               int maxBuffers, size_t maxMemory, int priority, int stackSize, int channelNum)

//    : asynNDArrayDriver(portName, MAX_SIGNALS, NUM_MPA_DETECTOR_PARAMS, maxBuffers, maxMemory,
//               0, 0, /* No interfaces beyond those set in ADDriver.cpp */
//               ASYN_CANBLOCK | ASYN_MULTIDEVICE, /* asyn flags*/
//               1,                                /* autoConnect=1 */
//               priority, stackSize),
//    uniqueId_(0), acquiring_(0)
// AD > 3.0 change
    : asynNDArrayDriver(portName, MAX_SIGNALS, maxBuffers, maxMemory,
               0, 0, /* No interfaces beyond those set in ADDriver.cpp */
               ASYN_CANBLOCK | ASYN_MULTIDEVICE, /* asyn flags*/
               1,                                /* autoConnect=1 */
               priority, stackSize),
    uniqueId_(0), acquiring_(0)			   
{
    int status = asynSuccess;
    int ErrSet;
    const char *functionName = "MPA3Detector";

    this->channelNum = channelNum;

    hDLL = LoadLibrary("DMPA3.DLL");
	
    if(hDLL){

//
// Original entry points
//
 
// VOID APIENTRY RunCmd(int nDisplay, LPSTR Cmd)
        lpRun=(IMPARUNCMD)GetProcAddress(hDLL,"RunCmd");

// VOID APIENTRY Erase(int nSystem)
        lpErase=(IMPAERASE)GetProcAddress(hDLL,"Erase");

// VOID APIENTRY Halt(int nSystem)
        lpHalt=(IMPAHALT)GetProcAddress(hDLL,"Halt");

//VOID APIENTRY Start(int nSystem)
         lpStart=(IMPASTART)GetProcAddress(hDLL,"Start");

// VOID APIENTRY NewSetting(int nDev) 
	lpNewSetting=(IMPANEWSET) GetProcAddress(hDLL,"NewSetting");

// int APIENTRY GetStatusData(ACQSTATUS *Status, int nDev) 
        lpStat=(IMPAGETSTATUS)GetProcAddress(hDLL,"GetStatusData");

// int APIENTRY GetStatus(int nDev)
	lpNewStat=(IMPANEWSTATUS)GetProcAddress(hDLL,"GetStatus");

// int APIENTRY GetSettingData(ACQSETTING *Setting, int nDisplay)
	lpSet=(IMPAGETSETTING)GetProcAddress(hDLL,"GetSettingData");

//int APIENTRY GetDatSetting(DATSETTING *Defdat) 
	lpGetDatSet=(IMPAGETDATSET)GetProcAddress(hDLL,"GetDatSetting");

// int APIENTRY GetMP3Setting(ACQMP3 *Defmp3)
	lpGetMp3Set=(IMPAGETMP3SET)GetProcAddress(hDLL,"GetMP3Setting");

//
// additional entry points
//

// int APIENTRY LVGetCnt(double *cntp, int nDisplay);
lpCnt=(IMPAGETCNT)GetProcAddress(hDLL,"LVGetCnt");

// int APIENTRY LVGetRoi(unsigned long FAR *roip, int nDisplay);
lpRoi=(IMPAGETROI)GetProcAddress(hDLL,"LVGetRoi");

// int APIENTRY LVGetDat(unsigned long HUGE *datp, int nDisplay);
lpDat=(IMPAGETDAT)GetProcAddress(hDLL,"LVGetDat");

// int APIENTRY LVGetStr(char *strp, int nDisplay);
lpStr=(IMPAGETSTR)GetProcAddress(hDLL,"LVGetStr");

// UINT APIENTRY ServExec(HWND ClientWnd);// Execute the Server MPA3.EXE
lpServ=(IMPASERVEXEC)GetProcAddress(hDLL,"ServExec");

// int APIENTRY DigInOut(int value, int enable); 
lpDigInOut=(IMPADIGINOUT)GetProcAddress(hDLL,"DigInOut");

// int APIENTRY DacOut(int value);  
lpDacOut=(IMPADACOUT)GetProcAddress(hDLL,"DacOut");

// VOID APIENTRY Continue(int nSystem); 
lpContinue=(IMPACONTINUE)GetProcAddress(hDLL,"Continue");

lpLVGetDat = (IMPALVGetDat)GetProcAddress(hDLL, "LVGetDat");

//
// Potential future entry points
//

// VOID APIENTRY StoreSettingData(ACQSETTING *Setting, int nDisplay);
// VOID APIENTRY StoreStatusData(ACQSTATUS *Status, int nDev);
// VOID APIENTRY StoreData(ACQDATA *Data, int nDisplay);
// int APIENTRY GetData(ACQDATA *Data, int nDisplay);
// long APIENTRY GetSpec(long i, int nDisplay);
// VOID APIENTRY SaveSetting(void);         
// VOID APIENTRY SaveData(int nDisplay, int all
// VOID APIENTRY GetBlock(long *hist, int start, int end, int step, int nDisplay);                           
// VOID APIENTRY StoreDefData(ACQDEF *Def);
// int APIENTRY GetDefData(ACQDEF *Def);	   
// VOID APIENTRY LoadData(int nDisplay, int all);     
// VOID APIENTRY NewData(void);             
// VOID APIENTRY HardwareDlg(int item);     
// VOID APIENTRY UnregisterClient(void);    
// VOID APIENTRY DestroyClient(void);       
// UINT APIENTRY ClientExec(HWND ServerWnd);// Execute the Client MPANT.EXE
// VOID APIENTRY AddData(int nDisplay, int all);      
// VOID APIENTRY SubData(int nDisplay, int all);      
// VOID APIENTRY Smooth(int nDisplay);       
// int APIENTRY LVGetOneRoi(int nDisplay, int roinum, long *x1, long *x2);	
// int APIENTRY LVGetOneCnt(double *cntp, int nDisplay, int cntnum);

// VOID APIENTRY StoreMP3Setting(ACQMP3 *Defmp3);
// VOID APIENTRY StoreDatSetting(DATSETTING *Defdat);
// VOID APIENTRY StoreReplaySetting(REPLAYSETTING *Repldat);
// int APIENTRY GetReplaySetting(REPLAYSETTING *Repldat);
// int APIENTRY GetDatPtr(int nDisplay, long *xmax, long *ymax, LPSTR *pt);
// int APIENTRY ReleaseDatPtr(void);
// long APIENTRY GetSVal(int DspID, long xval);
 
 
// long APIENTRY GetRoiIndex(LPSTR roiname);
// int APIENTRY DeleteRoi(DWORD roiindex);
// int APIENTRY SelectRoi(DWORD roiindex);
// int APIENTRY GetRoiSum(DWORD roiindex, double *sum);
// int APIENTRY BytearrayToShortarray(short *Shortarray, char *Bytearray, int length);
// int APIENTRY LVGetRoinam(char *strp, int nDisplay);
// int APIENTRY LVGetSpecLength(int nDisplay);
// int APIENTRY LVGetDefData(LVACQDEF *Def);
// int APIENTRY LVGetDatSetting(LVDATSETTING *Defdat, LPSTR filename, LPSTR specfile, LPSTR command);
// int APIENTRY LVGetReplaySetting(LVREPLAYSETTING *Repldat, LPSTR filename);
// int APIENTRY LVGetProiDat(int roiid, int x0, int y0, int xdim, int ydim, double *roisum, int *datp);
// int APIENTRY LVGetRoiRect(int nDisplay, int roinum, int *x0, int *y0, int *xdim, int *ydim, int *xmax);
// int APIENTRY LVGetRroiDat(int nDisplay, int roinum, int x0, int y0, int xdim, // int ydim, int xmax, double *RoiSum, long *datp, double *area);


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
    createParam(mpaTotalRateString,     asynParamFloat64, &P_TotalRate);

    status |= setIntegerParam(P_NumTimePoints, numTimePoints);
    status |= setIntegerParam(NDDataType, dataType);
    status |= setDoubleParam(P_TimeStep, 0.001);
    status |= setDoubleParam(P_Amplitude, 1.0);
    status |= setDoubleParam(P_Offset,    0.0);
    status |= setDoubleParam(P_Period,    1.0);
    status |= setDoubleParam(P_Phase,     0.0);
    status |= setDoubleParam(P_Noise,     0.0);
    status |= setDoubleParam(P_TotalRate, 0.0);

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
    /* Create the thread that updates the images */
    status = (epicsThreadCreate("mpaDetTaskBackground",
                                epicsThreadPriorityMedium,
                                epicsThreadGetStackSize(epicsThreadStackMedium),
                                (EPICSTHREADFUNC)mpaTaskCBackground,
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




void MPA3Detector::computeArrays(int verbose = 0)
{
    long xx = 0, yy= 0 , maxvalue = 0;
	
	size_t dims[2];
	int numTimePoints;
//	int i; 
//	int j;
	NDDataType_t dataType;
	epicsType *pData;
	uint32_t size = 0;
	double acquireTime;
	double timeStep;
//	double rndm;
//	double amplitude[MAX_SIGNALS];
//	double period[MAX_SIGNALS];
//	double frequency[MAX_SIGNALS];
//	double phase[MAX_SIGNALS];
//	double noise[MAX_SIGNALS];
//	double offset[MAX_SIGNALS];
//    NDArray *pImage;

ACQSTATUS      Status = { 0 }; 

	getIntegerParam(NDDataType, (int *)&dataType);
	getIntegerParam(P_NumTimePoints, &numTimePoints);
	getDoubleParam(P_TimeStep, &timeStep);
	getDoubleParam(P_AcquireTime, &acquireTime);

//	dims[0] = MAX_SIGNALS;
//	dims[1] = numTimePoints;

	dims[0] = 1024;
	dims[1] = 1024;
	dataType = NDUInt32;
//	epicsType = epicsUInt16;

	size = dims[0] * dims[1] * sizeof(uint32_t);
	
	setIntegerParam(NDDataType, NDUInt32);

//	printf(" dims0 %d dims1 %d \n", dims[0], dims[1]);

	if (this->pArrays[0]) this->pArrays[0]->release();
	this->pArrays[0] = pNDArrayPool->alloc(2, dims, dataType, 0, 0);
	pData = (epicsType *)this->pArrays[0]->pData;
	
	memset(pData, 0, 1024 * 1024 * sizeof(epicsType));
		
// The actual code that gets the data

	unsigned long *data;
//	int ii = 2;
int two = 2;
double temptotalrate;
      int err;
	  
	if (lpStat) err = (*lpStat)(&Status, two); 
//    printf("totalrate= %.2lf\n", Status->cnt[ST_TOTALRATE]);
    temptotalrate = Status.cnt[ST_TOTALRATE];
    setDoubleParam(P_TotalRate, temptotalrate);
    callParamCallbacks();
   
	data = (unsigned long *)calloc(1024 * 1024, sizeof(long));
	if (lpLVGetDat) err = (*lpLVGetDat) (data, abs(this->channelNum));

//    memcpy(pImage->pData, data, size);
	
    for (int jj=0; jj < 1024; jj++) {
	for (int kk=0; kk < 1024; kk++) {
	
//	printf(" jj %d kk %d \n",jj,kk);
		
//	data[(jj*1024)+kk] = jj;
	
	pData[(jj*1024)+kk] = (epicsType) data[(jj*1024)+kk];	
if (data[(jj*1024)+kk] > maxvalue){	
	maxvalue = data[(jj*1024)+kk];
	xx=jj;
	yy=kk;
}
}
	}
free(data);	
	if (verbose != 0) {
printf("Max Value seen at (%d,%d) %d \n",xx,yy,maxvalue);
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
 //       printf("before Computer arrays \n");
        computeArrays(1);
 //      printf("after Compute arrays \n");


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

/**
This loop is designed to run about periodically ( 1 sec ?) so that a caqtdm cacamera widget can be displaying what the vendor software
is doing. A standard collect and acquire is expensive many, many second, so just displaying what it already has save a lot of time.
So do this when idle and hope for the best.
*/

void MPA3Detector::mpaTaskBackground()
{
    int status = asynSuccess;
    NDArray *pImage;
    epicsTimeStamp startTime;
    //    int numTimePoints;
//    int arrayCounter;
    //    double timeStep;

    int i;
    int value;
    const char *functionName = "mpaTaskBackground";

//printf("In mpaTaskBackground %d\n",channelNum);

    /* Loop forever */

    while (1) {
       this->unlock();
	   epicsThreadSleep(1.0);

 //       printf("In the mpaTaskBackground loop %d\n",channelNum);	
	   this->lock();
	   if (this->channelNum > 0) {
        status = getIntegerParam(P_RunTimeEnable, &value);
        if (value == 0) {
        computeArrays(0);
		
//	status |= setIntegerParam(ADMinX, 0);
//	status |= setIntegerParam(ADMinY, 0);
	status |= setIntegerParam(NDArraySizeX, 1024);
//	status |= setIntegerParam(ADSizeX, 1024);
	status |= setIntegerParam(NDArraySizeY, 1024);
//	status |= setIntegerParam(ADSizeY, 1024);

        pImage = this->pArrays[0];

        /* Put the frame number and time stamp into the buffer */
        pImage->uniqueId = uniqueId_;
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
    }
	   }
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
                                 int maxBuffers, int maxMemory, int priority, int stackSize, 
								 int channelNum)
{
    new MPA3Detector(portName, numTimePoints, (NDDataType_t)dataType,
                    (maxBuffers < 0) ? 0 : maxBuffers,
                    (maxMemory < 0) ? 0 : maxMemory, 
                    priority, stackSize, 
					channelNum);
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
static const iocshArg MPA3DetectorConfigArg7 = {"channelNum",    iocshArgInt};

static const iocshArg * const MPA3DetectorConfigArgs[] = {&MPA3DetectorConfigArg0,
                                                            &MPA3DetectorConfigArg1,
                                                            &MPA3DetectorConfigArg2,
                                                            &MPA3DetectorConfigArg3,
                                                            &MPA3DetectorConfigArg4,
                                                            &MPA3DetectorConfigArg5,
                                                            &MPA3DetectorConfigArg6,
											                &MPA3DetectorConfigArg7,
															};
static const iocshFuncDef configMPA3Detector = {"MPA3DetectorConfig", 8, MPA3DetectorConfigArgs};
static void configMPA3DetectorCallFunc(const iocshArgBuf *args)
{
    MPA3DetectorConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival,
                         args[4].ival, args[5].ival, args[6].ival, args[7].ival);
}


static void MPA3DetectorRegister(void)
{

    iocshRegister(&configMPA3Detector, configMPA3DetectorCallFunc);
}

extern "C" {
epicsExportRegistrar(MPA3DetectorRegister);
}
