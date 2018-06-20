/* MPA3 driver
 * Based on ADCSimDetector (Mark Rivers)
 * Joe Sullivan
 * Feb. 10 2017
 */

#include <epicsEvent.h>
#include <epicsTime.h>
#include "asynNDArrayDriver.h"
#include <windows.h>
#include "dmpa3.h"

#define mpaAcquireString        "MPA_ACQUIRE"
#define mpaAcquireTimeString    "MPA_ACQUIRE_TIME"
#define mpaAutoIncrementString  "MPA_AUTO_INCREMENT"
#define mpaAutoSaveString        "MPA_AUTO_SAVE"
#define mpaRunTimeEnableString   "MPA_RUNTIME_ENABLE"
#define mpaFilenameString        "MPA_FILENAME"


#define mpaElapsedTimeString    "MPA_ELAPSED_TIME"
#define mpaTimeStepString       "MPA_TIME_STEP"
#define mpaNumTimePointsString  "MPA_NUM_TIME_POINTS"
#define mpaAmplitudeString      "MPA_AMPLITUDE"
#define mpaOffsetString         "MPA_OFFSET"
#define mpaPeriodString         "MPA_PERIOD"
#define mpaFrequencyString      "MPA_FREQUENCY"
#define mpaPhaseString          "MPA_PHASE"
#define mpaNoiseString          "MPA_NOISE"
#define mpaCommandString        "MPA_COMMAND"

#define MAX_SIGNALS 8

/** ADC simulation driver; does 1-D waveforms on 8 channels. 
  * Inherits from asynNDArrayDriver */
class epicsShareClass MPA3Detector : public asynNDArrayDriver {
public:
    MPA3Detector(const char *portName, int numTimePoints, NDDataType_t dataType,
                   int maxBuffers, size_t maxMemory,
                   int priority, int stackSize);
    virtual ~MPA3Detector();

    /* These are the methods that we override from asynNDArrayDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, 
                                    size_t nChars, size_t *nActual);

    virtual void report(FILE *fp, int details);
    void mpaTask(); /**< Should be private, but gets called from C, so must be public */

protected:
    int P_Acquire;
    #define FIRST_MPA_DETECTOR_PARAM P_Acquire
    int P_AcquireTime;
    int P_ElapsedTime;
    int P_AutoIncrement;
    int P_AutoSave;
    int P_RunTimeEnable;
    int P_MPAFilename;
    int P_TimeStep;
    int P_NumTimePoints;
    int P_Period;
    int P_Amplitude;
    int P_Offset;
    int P_Frequency;
    int P_Phase;
    int P_Noise;

    int P_Command;
    #define LAST_MPA_DETECTOR_PARAM P_Command


private:
    /* These are the methods that are new to this class */
    template <typename epicsType> void computeArraysT();
    void computeArrays();
    void setAcquire(int value);
    int getStatus();
    int getSettings();
    void sendCommand( char * sndCmnd);

    void PrintMpaStatus(ACQSTATUS *Stat);   // ON/OFF, TIME 
    void PrintStatus(ACQSTATUS *Stat);
    void PrintDatSetting(DATSETTING *Set);
    void PrintMP3Setting(ACQMP3 *Set);
    void PrintSetting(ACQSETTING *Set);

    char command[256];
    //    char status[100];

    /* Our data */
    epicsEventId startEventId_;
    epicsEventId stopEventId_;
    int uniqueId_;
    int acquiring_;
    double elapsedTime_;

    // HANDLE hDLL;
    HMODULE hDLL;
    IMPAGETSETTING lpSet;
    IMPANEWSTATUS lpNewStat;
    IMPAGETSTATUS lpStat;
    IMPARUNCMD lpRun;
    IMPAGETCNT lpCnt;
    IMPAGETROI lpRoi;
    IMPAGETDAT lpDat;
    IMPAGETSTR lpStr;
    IMPASERVEXEC lpServ;
    IMPAGETDATSET lpGetDatSet;
    IMPAGETMP3SET lpGetMp3Set;
    IMPADIGINOUT lpDigInOut;
    IMPADACOUT lpDacOut;
    IMPASTART lpStart;
    IMPAHALT lpHalt;
    IMPACONTINUE lpContinue;
    IMPAERASE lpErase;
    IMPANEWSET lpNewSetting;

    ACQSETTING Setting;
    ACQDATA Data;
    ACQDEF Def;
    ACQSTATUS Status;
    DATSETTING DatSetting;
    ACQMP3 MP3Setting;
    short nDev;
};


#define NUM_MPA_DETECTOR_PARAMS ((int)(&LAST_MPA_DETECTOR_PARAM - &FIRST_MPA_DETECTOR_PARAM + 1))
