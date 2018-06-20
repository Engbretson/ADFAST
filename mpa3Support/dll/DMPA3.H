#ifdef __cplusplus
extern "C"	
{
#endif

#include "struct.h"

#define MAXCNT              448
#define MAXDEV 17

#ifdef WINDOWS31
#define MAXDSP 32
#else
#define MAXDSP 64
#endif

#define ID_SAVE             103
#define ID_CONTINUE         106
#define ID_START            109
#define ID_BREAK            137
#define ID_NEWSETTING       139
#define ID_GETSTATUS        141
#define ID_SAVEFILE         151
#define ID_ERASE            154
#define ID_LOADFILE         155
#define ID_NEWDATA          160
#define ID_HARDWDLG         161
#define ID_SAVEFILE2                194
#define ID_LOADFILE2                203
#define ID_SAVEFILE3                217
#define ID_LOADFILE3                219
#define ID_SAVEFILE4                223
#define ID_LOADFILE4                225
#define ID_LOADFILE5                226
#define ID_LOADFILE6                227
#define ID_LOADFILE7                228
#define ID_LOADFILE8                229
#define ID_SAVEFILE5                230
#define ID_SAVEFILE6                231
#define ID_SAVEFILE7                232
#define ID_SAVEFILE8                233
#define ID_SUMFILE                      234
#define ID_SUMFILE2                     235
#define ID_SUMFILE3                     236
#define ID_SUMFILE4                     237
#define ID_SUMFILE5                     238
#define ID_SUMFILE6                     239
#define ID_SUMFILE7                     240
#define ID_SUMFILE8                     241
#define ID_SUBTRACT                     289
#define ID_SMOOTH                       290
#define ID_SUBTRACT2                    296
#define ID_SMOOTH2                      297
#define ID_SUBTRACT3                    298
#define ID_SMOOTH3                      299
#define ID_SUBTRACT4                    300
#define ID_SMOOTH4                      301
#define ID_SUBTRACT5                    302
#define ID_SMOOTH5                      303
#define ID_SUBTRACT6                    304
#define ID_SMOOTH6                      305
#define ID_SUBTRACT7                    306
#define ID_SMOOTH7                      307
#define ID_SUBTRACT8                    308
#define ID_SMOOTH8                      309
#define ID_COMBDLG          401
#define ID_DATADLG          402
#define ID_MAPLSTDLG        403
#define ID_REPLDLG          404
#define ID_ERASE2          1108
#define ID_ERASE3          1109
#define ID_ERASE4          1110
#define ID_ERASEFILE2      1111
#define ID_ERASEFILE3      1112
#define ID_ERASEFILE4      1113
#define ID_START2          1114
#define ID_BREAK2          1115
#define ID_CONTINUE2       1116
#define ID_START3          1117
#define ID_BREAK3          1118
#define ID_CONTINUE3       1119
#define ID_START4          1120
#define ID_BREAK4          1121
#define ID_CONTINUE4       1122
#define ID_RUNCMD                       1123
#define ID_RUNCMD2                      1124
#define ID_RUNCMD3                      1125
#define ID_RUNCMD4                      1126
#define ID_RUNCMD5                      1127
#define ID_RUNCMD6                      1128
#define ID_RUNCMD7                      1129
#define ID_RUNCMD8                      1130
#define ID_ERASEFILE5                   1131
#define ID_ERASEFILE6                   1132
#define ID_ERASEFILE7                   1133
#define ID_ERASEFILE8                   1134
#define ID_DIGINOUT			1137
#define ID_DACOUT			1138
#define ID_GETROIINDEX		1139
#define ID_GETROISUM		1141
#define ID_DELETEROI		1301
#define ID_SELECTROI		1302


/*** FUNCTION PROTOTYPES (do not change) ***/
BOOL APIENTRY DllMain(HANDLE hInst, DWORD ul_reason_being_called, LPVOID lpReserved);

VOID APIENTRY StoreSettingData(ACQSETTING *Setting, int nDisplay);
                                           // Stores Settings into the DLL
int APIENTRY GetSettingData(ACQSETTING *Setting, int nDisplay);
                                           // Get Settings stored in the DLL
VOID APIENTRY StoreStatusData(ACQSTATUS *Status, int nDev);
                                           // Store the Status into the DLL
int APIENTRY GetStatusData(ACQSTATUS *Status, int nDev);
                                           // Get the Status
VOID APIENTRY Start(int nSystem);        // Start
VOID APIENTRY Halt(int nSystem);         // Halt
VOID APIENTRY Continue(int nSystem);     // Continue
VOID APIENTRY NewSetting(int nDisplay);   // Indicate new Settings to Server
UINT APIENTRY ServExec(HWND ClientWnd);  // Execute the Server MPA3.EXE
VOID APIENTRY StoreData(ACQDATA *Data, int nDisplay);
                                           // Stores Data pointers into the DLL
int APIENTRY GetData(ACQDATA *Data, int nDisplay);
                                           // Get Data pointers
long APIENTRY GetSpec(long i, int nDisplay);
                                           // Get a spectrum value
VOID APIENTRY SaveSetting(void);         // Save Settings
int APIENTRY GetStatus(int nDev);        // Request actual Status from Server
VOID APIENTRY Erase(int nSystem);        // Erase spectrum
VOID APIENTRY SaveData(int nDisplay, int all);     // Saves data
VOID APIENTRY GetBlock(long *hist, int start, int end, int step,
  int nDisplay);                           // Get a block of spectrum data
VOID APIENTRY StoreDefData(ACQDEF *Def);
                                           // Store System Definition into DLL
int APIENTRY GetDefData(ACQDEF *Def);	   // Get System Definition
VOID APIENTRY LoadData(int nDisplay, int all);     // Loads data
VOID APIENTRY NewData(void);             // Indicate new ROI or string Data
VOID APIENTRY HardwareDlg(int item);     // item=0: Calls the Settings dialog
                                           // 1: data dialog, 2: system dialog
VOID APIENTRY UnregisterClient(void);    // Clears remote mode from MPANT
VOID APIENTRY DestroyClient(void);       // Close MPANT
UINT APIENTRY ClientExec(HWND ServerWnd);
                                           // Execute the Client MPANT.EXE
int APIENTRY LVGetDat(unsigned long HUGE *datp, int nDisplay);
                                           // Copies the spectrum to an array
VOID APIENTRY RunCmd(int nDisplay, LPSTR Cmd);
                                           // Executes command
VOID APIENTRY AddData(int nDisplay, int all);      // Adds data
VOID APIENTRY SubData(int nDisplay, int all);      // Subtracts data
VOID APIENTRY Smooth(int nDisplay);       // Smooth data
int APIENTRY LVGetRoi(unsigned long FAR *roip, int nDisplay);
                                     // Copies the ROI boundaries to an array
int APIENTRY LVGetOneRoi(int nDisplay, int roinum, long *x1, long *x2);
									 // Get one ROI boundary
int APIENTRY LVGetCnt(double *cntp, int nDisplay);
                                           // Copies Cnt numbers to an array
int APIENTRY LVGetOneCnt(double *cntp, int nDisplay, int cntnum);
                             // Get one Cnt number
int APIENTRY LVGetStr(char *strp, int nDisplay);
                                           // Copies strings to an array
VOID APIENTRY StoreMP3Setting(ACQMP3 *Defmp3);
                                    // Store MP3 System Definition into DLL
int APIENTRY GetMP3Setting(ACQMP3 *Defmp3);
                                    // Get MP3 System Definition from DLL
VOID APIENTRY StoreDatSetting(DATSETTING *Defdat);
                                    // Store Data Format Definition into DLL
int APIENTRY GetDatSetting(DATSETTING *Defdat);
                                    // Get Data Format Definition from DLL
VOID APIENTRY StoreReplaySetting(REPLAYSETTING *Repldat);
                                    // Store Replay Settings into DLL
int APIENTRY GetReplaySetting(REPLAYSETTING *Repldat);
                                    // Get Replay Settings from DLL
int APIENTRY GetDatPtr(int nDisplay, long *xmax, long *ymax, LPSTR *pt);
                                    // Get a temporary pointer to spectra data
int APIENTRY ReleaseDatPtr(void);
									// Release temporary data pointer
long APIENTRY GetSVal(int DspID, long xval);
					  // Get special display data like projections from MPANT 

int APIENTRY DigInOut(int value, int enable);  // controls Dig I/0 ,
								  // returns digin
int APIENTRY DacOut(int value);   // output Dac value as analogue voltage
long APIENTRY GetRoiIndex(LPSTR roiname);
      // get a unique index to address ROIs from named ROI's.
      // rectangular or 1D ROIs: 
      //    LOWORD is the spectra number, 
      //    HIWORD is the ROI number (1,2,..)
      // polygonal ROIs:    
      //    LOWORD is an entry number
      //    HIWORD is the roiid = 100 * spectra_number + ROI_number
      // returns 0 if not found.
int APIENTRY DeleteRoi(DWORD roiindex);
      // deletes ROI with given index
int APIENTRY SelectRoi(DWORD roiindex);
      // selects ROI with given index
int APIENTRY GetRoiSum(DWORD roiindex, double *sum);
	// get sum of counts in ROI, 
	// returns roiindex, or 0 if not found
int APIENTRY BytearrayToShortarray(short *Shortarray, char *Bytearray, int length);
    // auxiliary function for VB.NET to convert strings

int APIENTRY LVGetRoinam(char *strp, int nDisplay);
	// get Roi names
int APIENTRY LVGetSpecLength(int nDisplay);

int APIENTRY LVGetDefData(LVACQDEF *Def);

int APIENTRY LVGetDatSetting(LVDATSETTING *Defdat, LPSTR filename, LPSTR specfile, LPSTR command);

int APIENTRY LVGetReplaySetting(LVREPLAYSETTING *Repldat, LPSTR filename);

int APIENTRY LVGetProiDat(int roiid, int x0, int y0, int xdim, int ydim, double *roisum, int *datp);

int APIENTRY LVGetRoiRect(int nDisplay, int roinum, int *x0, int *y0, int *xdim, int *ydim, int *xmax);

int APIENTRY LVGetRroiDat(int nDisplay, int roinum, int x0, int y0, int xdim, int ydim, int xmax,
				 double *RoiSum, long *datp, double *area);
#ifdef __cplusplus
}
#endif
