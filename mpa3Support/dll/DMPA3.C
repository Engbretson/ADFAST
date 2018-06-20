/***************************************************************************
  MODUL:    DMPA3.C
  PURPOSE:  DLL to communicate with MPA3 Server
****************************************************************************/

#include "windows.h"
#include <string.h>
#include <stdio.h>
#define DLL
#include "dmpa3.h"

#pragma data_seg("dmpa3sh")
BOOL    bRemote=0;
BOOL    bDef=FALSE;
BOOL    bMp3=FALSE;
BOOL    bDat=FALSE;
BOOL    bRepl=FALSE;
HWND    hwndServer=0;
HWND    hwndClient=0;
HWND    hwndMPANT=0;
int     MM_NEARCONTROL=0;
int     MM_GETVAL=0;
BOOL    bStatus[MAXDEV]={0};
BOOL    bSetting[MAXDSP]={0};

ACQSTATUS DLLStatus[MAXDEV] = {0};
EXTACQSETTING DLLSetting[MAXDSP] = {0};
#ifdef WINDOWS31
ACQDATA DLLData[MAXDSP] = {0};
HANDLE  hInst=0;
#endif
ACQDEF DLLDef = {0};
ACQMP3 DLLMp3 = {0};
DATSETTING DLLdat = {0};
REPLAYSETTING DLLRepl = {0};

#pragma data_seg()

#ifdef WINDOWS31
/****************************************************************************
    FUNCTION:  WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded (no
              remaining tasks still have the DLL loaded).  It is strongly
              recommended that a DLL have a WEP() function, even if it does
              nothing but returns success (1), as in this example.

*******************************************************************************/
int FAR PASCAL WEP (int bSystemExit)
{
    return(1);
}

/****************************************************************************
   FUNCTION: LibMain(HANDLE, WORD, WORD, LPSTR)

   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
             the DLL is loaded.  The LibEntry routine is provided in
             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
             source LIBENTRY.ASM is also provided.)

             LibEntry initializes the DLL's heap, if a HEAPSIZE value is
             specified in the DLL's DEF file.  Then LibEntry calls
	     LibMain.

	     LibMain should return a value of 1 if the initialization is
	     successful.
*******************************************************************************/
int FAR PASCAL LibMain(hModule, wDataSeg, cbHeapSize, lpszCmdLine)
HANDLE	hModule;
WORD    wDataSeg;
WORD    cbHeapSize;
LPSTR   lpszCmdLine;
{
   DLLDef.bRemote = 0;
   hInst=hModule;
   MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
   MM_GETVAL = RegisterWindowMessage((LPSTR)"MPANTGetval");
   if(cbHeapSize)
     UnlockData(0);

    return 1;
}

#else

BOOL APIENTRY DllMain(HANDLE hInst, DWORD ul_reason_being_called, LPVOID lpReserved)
{
    return 1;
        UNREFERENCED_PARAMETER(hInst);
        UNREFERENCED_PARAMETER(ul_reason_being_called);
        UNREFERENCED_PARAMETER(lpReserved);
}

#endif

VOID APIENTRY StoreDefData(ACQDEF *Def)
{
  int i;
/* {
	  char txt[100];
	  int nDisplay = 0;
	  sprintf(txt,"StoreDefData %d %d %ld", nDisplay, bSetting[nDisplay], DLLSetting[nDisplay].range);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
} */
  if(Def == NULL) {
    bDef = FALSE;
    for (i=0; i<MAXDSP; i++) {
      bSetting[i] = FALSE;
    } 
  }
  else{
    _fmemcpy((LPSTR FAR *)&DLLDef,(LPSTR FAR *)Def,sizeof(ACQDEF));
    bDef = TRUE;
  }
/* {
	  char txt[100];
	  int nDisplay = 0;
	  sprintf(txt,"StoreDefData %d %d %ld", nDisplay, bSetting[nDisplay], DLLSetting[nDisplay].range);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
} */
}

int APIENTRY GetDefData(ACQDEF *Def)
{
  if (bDef) {
    DLLDef.bRemote = bRemote;
    _fmemcpy((LPSTR FAR *)Def,(LPSTR FAR *)&DLLDef,sizeof(ACQDEF));
  }
  return bDef;
}

int APIENTRY LVGetDefData(LVACQDEF *Def)
{
  if (bDef) {
    DLLDef.bRemote = bRemote;
    _fmemcpy((LPSTR FAR *)Def,(LPSTR FAR *)&DLLDef,sizeof(LVACQDEF));
	return 0;
  }
  return -1;
}

VOID APIENTRY StoreMP3Setting(ACQMP3 *Defmp3)
{
  if(Defmp3 == NULL) {
    bMp3 = FALSE;
  }
  else{
    _fmemcpy((LPSTR FAR *)&DLLMp3,(LPSTR FAR *)Defmp3,sizeof(ACQMP3));
    bMp3 = TRUE;
  }
}

int APIENTRY GetMP3Setting(ACQMP3 *Defmp3)
{
  if (bMp3) {
    _fmemcpy((LPSTR FAR *)Defmp3,(LPSTR FAR *)&DLLMp3,sizeof(ACQMP3));
  }
  return bMp3;
}

VOID APIENTRY StoreDatSetting(DATSETTING *Defdat)
{
  if(Defdat == NULL) {
    bDat = FALSE;
  }
  else{
    _fmemcpy((LPSTR FAR *)&DLLdat,(LPSTR FAR *)Defdat,sizeof(DATSETTING));
    bDat = TRUE;
  }
}

int APIENTRY GetDatSetting(DATSETTING *Defdat)
{
  if (bDat) {
    _fmemcpy((LPSTR FAR *)Defdat,(LPSTR FAR *)&DLLdat,sizeof(DATSETTING));
  }
  return bDat;
}

VOID APIENTRY StoreReplaySetting(REPLAYSETTING *Repldat)
{
  if(Repldat == NULL) {
    bRepl = FALSE;
  }
  else{
    _fmemcpy((LPSTR FAR *)&DLLRepl,(LPSTR FAR *)Repldat,sizeof(REPLAYSETTING));
    bRepl = TRUE;
  }
}

int APIENTRY GetReplaySetting(REPLAYSETTING *Repldat)
{
  if (bRepl) {
    _fmemcpy((LPSTR FAR *)Repldat,(LPSTR FAR *)&DLLRepl,sizeof(REPLAYSETTING));
  }
  return bRepl;
}

VOID APIENTRY StoreSettingData(ACQSETTING * Setting, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if(Setting == NULL) {
    bSetting[nDisplay] = FALSE;
  }
  else{
    _fmemcpy((LPSTR FAR *)&DLLSetting[nDisplay],
           (LPSTR FAR *)Setting,sizeof(ACQSETTING));
    bSetting[nDisplay] = TRUE;
    if(Setting->range == 0L) {
      bSetting[nDisplay] = FALSE;
    }
/* {
	  char txt[100];
	  sprintf(txt,"StoreSettingData %d %d %ld", nDisplay, bSetting[nDisplay], Setting->range);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
} */
  }
}

int APIENTRY GetSettingData(ACQSETTING *Setting, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bSetting[nDisplay]) {
    _fmemcpy((LPSTR FAR *)Setting,
           (LPSTR FAR *)&DLLSetting[nDisplay],sizeof(ACQSETTING));
  }
  return bSetting[nDisplay];
}

VOID APIENTRY StoreExtSettingData(EXTACQSETTING *Setting, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if(Setting == NULL) {
    bSetting[nDisplay] = FALSE;
  }
  else{
    _fmemcpy((LPSTR FAR *)&DLLSetting[nDisplay],
           (LPSTR FAR *)Setting,sizeof(EXTACQSETTING));
    bSetting[nDisplay] = TRUE;
    if(Setting->range == 0L) {
      bSetting[nDisplay] = FALSE;
    }
  }
/* {
	  char txt[100];
	  sprintf(txt,"StoreExtSettingData %d %d %ld", nDisplay, bSetting[nDisplay], DLLSetting[nDisplay].range);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
} */
}

int APIENTRY GetExtSettingData(EXTACQSETTING *Setting, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bSetting[nDisplay]) {
    _fmemcpy((LPSTR FAR *)Setting,
           (LPSTR FAR *)&DLLSetting[nDisplay],sizeof(EXTACQSETTING));
  }
/* {
	  char txt[100];
	  sprintf(txt,"GetExtSettingData %d %d %ld", nDisplay, bSetting[nDisplay], DLLSetting[nDisplay].range);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
} */
  return bSetting[nDisplay];
}

VOID APIENTRY StoreData(ACQDATA *Data, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if(Data == NULL) {
    bSetting[nDisplay] = FALSE;
  }
#ifdef WINDOWS31
  else
    _fmemcpy((LPSTR FAR *)&DLLData[nDisplay],(LPSTR FAR *)Data,sizeof(ACQDATA));
#endif
}

int APIENTRY GetData(ACQDATA *Data, int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    _fmemcpy((LPSTR FAR *)Data,(LPSTR FAR *)&DLLData[nDisplay],sizeof(ACQDATA));
  }
#endif
  return bSetting[nDisplay];
}

long APIENTRY GetSpec(long i, int nDisplay)
{
#ifdef WINDOWS31
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (bSetting[nDisplay] && i < DLLSetting[nDisplay].range)
    return (DLLData[nDisplay].s0[i]);
  else
    return 0L;
#else
  char sz[40];
  HANDLE hs0;
  unsigned long *s0;
  unsigned long val;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 0;
  if (!bSetting[nDisplay]) return 0;
  if (i > DLLSetting[nDisplay].range) return 0;
  sprintf(sz,"MPA3_S0_%d",nDisplay);
  if (!(hs0 = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 0;
  if (!(s0 = (unsigned long *)MapViewOfFile(hs0,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hs0);
    return 0;
  }
  val = s0[i];
  UnmapViewOfFile(s0);
  CloseHandle(hs0);
  return val;
#endif
}

VOID APIENTRY GetBlock(long *hist, int start, int end, int step,
  int nDisplay)
{
#ifdef WINDOWS31
  int i,j=0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (end > DLLSetting[nDisplay].range) end = DLLSetting[nDisplay].range;
  for (i=start; i<end; i+=step, j++)
    *(hist + j) = DLLData[nDisplay].s0[i];
#else
  int i,j=0;
  char sz[40];
  HANDLE hs0;
  unsigned long *s0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!bSetting[nDisplay]) return;
  if (end > DLLSetting[nDisplay].range) end = DLLSetting[nDisplay].range;
  sprintf(sz,"MPA3_S0_%d",nDisplay);
  if (!(hs0 = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return;
  if (!(s0 = (unsigned long *)MapViewOfFile(hs0,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hs0);
    return;
  }
  for (i=start; i<end; i+=step, j++)
    *(hist + j) = s0[i];
  UnmapViewOfFile(s0);
  CloseHandle(hs0);
  return;
#endif
}

int APIENTRY LVGetDat(unsigned long HUGE *datp, int nDisplay)
{
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    long i;
    for (i=0; i<DLLSetting[nDisplay].range; i++)
      datp[i] = DLLData[nDisplay].s0[i];
    return 0;
  }
  else return 4;
#else
  long i;
  char sz[40];
  HANDLE hs0;
  unsigned long *s0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MPA3_S0_%d",nDisplay);
  if (!(hs0 = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(s0 = (unsigned long *)MapViewOfFile(hs0,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hs0);
    return 4;
  }
  for (i=0; i<DLLSetting[nDisplay].range; i++)
      datp[i] = s0[i];
  UnmapViewOfFile(s0);
  CloseHandle(hs0);
  return 0;
#endif
}

HANDLE hEXMDisplay=0;
LPSTR EXMDisplay=NULL;

int APIENTRY GetDatPtr(int nDisplay, long *xmax, long *ymax, LPSTR *pt)
{
  char sz[40];
  if (nDisplay < 0 || nDisplay >= MAXDSP) return -1;
  if (!bSetting[nDisplay]) return -1;
  *xmax = DLLSetting[nDisplay].xdim;
  *ymax = DLLSetting[nDisplay].range;
  if (*xmax) *ymax /= *xmax;
  else {*xmax = *ymax; *ymax = 1;}
  sprintf(sz,"MPA3_S0_%d",nDisplay);
  ReleaseDatPtr();
  if (!(hEXMDisplay = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 0;
  if (!(EXMDisplay = MapViewOfFile(hEXMDisplay,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hEXMDisplay);
    return 0;
  }
  *pt = EXMDisplay;
  return (int)hEXMDisplay;
}

int APIENTRY ReleaseDatPtr()
{
  if(EXMDisplay)
	 UnmapViewOfFile(EXMDisplay);
  EXMDisplay = NULL;
  if(hEXMDisplay)
	 CloseHandle(hEXMDisplay);
  hEXMDisplay = 0;
  return 0;
}

int APIENTRY LVGetProiDat(int roiid, int x0, int y0, int xdim, int ydim, double *roisum, int *datp)
{
  int idisp, x,y, x1, y1, xmax, ymax, pos, ret;
  unsigned int *pt;
  idisp = roiid / 100;
  ret = GetDatPtr(idisp, &xmax, &ymax, (LPSTR *)&pt);
  if (ret <= 0) return -1;
  *roisum = 0.;
  x1 = x0 + xdim;
  y1 = y0 + ydim;
  for (x = x0; x < x1; x++) {
    for (y = y0; y < y1; y++) {
	  pos = y * xmax + x;
      *datp *= *(pt + pos);
	  *roisum += *datp;
      datp++;
    }
  }
  ReleaseDatPtr();
  return 0;
}

int APIENTRY LVGetRoi(unsigned long FAR *roip, int nDisplay)
{
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    int i,n;
    n = 2 * DLLSetting[nDisplay].nregions;
    for (i=0; i<n; i++)
      roip[i] = DLLData[nDisplay].region[i];
    return 0;
  }
  else return 4;
#else
  int i,n;
  char sz[40];
  HANDLE hrg;
  unsigned long *region;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MPA3_RG_%d",nDisplay);
  if (!(hrg = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(region = (unsigned long *)MapViewOfFile(hrg,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hrg);
    return 4;
  }
  n = 2 * DLLSetting[nDisplay].nregions;
  for (i=0; i<n; i++)
    roip[i] = region[i];
  UnmapViewOfFile(region);
  CloseHandle(hrg);
  return 0;
#endif
}

int APIENTRY LVGetOneRoi(int nDisplay, int roinum, long *x1, long *x2)
{
#ifdef WINDOWS31
  if (bSetting[nDisplay] && (roinum > 0 && (roinum <= 128)) {
    *x1 = DLLData[nDisplay].region[2*(roinum-1)];
    *x2 = DLLData[nDisplay].region[2*(roinum-1)+1];
    return 0;
  }
  else return 4;
#else
  char sz[40];
  HANDLE hrg;
  unsigned long *region;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay] || (roinum < 1) || (roinum > 128)) return 4;
  sprintf(sz,"MPA3_RG_%d",nDisplay);
  if (!(hrg = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(region = (unsigned long *)MapViewOfFile(hrg,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hrg);
    return 4;
  }
  *x1 = region[2*(roinum-1)];
  *x2 = region[2*(roinum-1)+1];
  UnmapViewOfFile(region);
  CloseHandle(hrg);
  return 0;
#endif
}

int APIENTRY LVGetRoiRect(int nDisplay, int roinum, int *x0, int *y0, int *xdim, int *ydim, int *xmax)
{
  int ret, xrange, xch1, xch2, ymax, offset, x1, y1;
  unsigned long *pt;
  if (!GetDatPtr(nDisplay, &xrange, &ymax, (LPSTR *)&pt)) {
    return -6;
  } 			// Display not found

  xrange = DLLSetting[nDisplay].xdim;
  ymax = DLLSetting[nDisplay].range;
  if (xrange) ymax /= xrange;
  else {xrange = ymax; ymax = 1;}

  ret = LVGetOneRoi(nDisplay, roinum, &xch1, &xch2);
  if (ret || !xrange) {
	  ReleaseDatPtr();
	  return -1;
  }
  *y0 = xch1 / xrange;
  offset = (*y0) * xrange;
  *x0 = xch1 - offset;
  y1 = xch2 / xrange;
  offset = y1 * xrange;
  x1 = xch2 - offset;
  *xdim = x1 - *x0;
  *ydim = y1 - *y0 + 1;
  *xmax = xrange;
  return 0;
}

int APIENTRY LVGetRroiDat(int nDisplay, int roinum, int x0, int y0, int xdim, int ydim, int xmax,
				 double *RoiSum, long *datp, double *area)
{
  int x, x1, y, y1, pos, ymax;
  unsigned long *pt;
  unsigned long val;
  if (!GetDatPtr(nDisplay, &xmax, &ymax, (LPSTR *)&pt)) {
    return -6;
  } 			// Display not found

  x1 = x0 + xdim;
  y1 = y0 + ydim -1;
  *RoiSum = 0.;
  *area = 0.;
  for (x = x0; x < x1; x++) {
    for (y = y0; y <= y1; y++) {
      pos = y * xmax + x;
      val = *(pt + pos);
      *RoiSum += val;
      *datp = val;
      datp++;
      *area += 1.;
    }
  }
  ReleaseDatPtr();
  return 0;
}

int APIENTRY LVGetCnt(double *cntp, int nDisplay)
{
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    int i;
    for (i=0; i<MAXCNT; i++)
      cntp[i] = DLLData[nDisplay].cnt[i];
    return 0;
  }
  else return 4;
#else
  int i;
  char sz[40];
  HANDLE hct;
  double *cnt;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MPA3_CT_%d",nDisplay);
  if (!(hct = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(cnt = (double *)MapViewOfFile(hct,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hct);
    return 4;
  }
  for (i=0; i<MAXCNT; i++)
    cntp[i] = cnt[i];
  UnmapViewOfFile(cnt);
  CloseHandle(hct);
  return 0;
#endif
}

int APIENTRY LVGetOneCnt(double *cntp, int nDisplay, int cntnum)
                             // Get one Cnt number
{
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    *cntp = DLLData[nDisplay].cnt[cntnum];
    return 0;
  }
  else return 4;
#else
  char sz[40];
  HANDLE hct;
  double *cnt;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MPA3_CT_%d",nDisplay);
  if (!(hct = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(cnt = (double *)MapViewOfFile(hct,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hct);
    return 4;
  }
  *cntp = cnt[cntnum];
  UnmapViewOfFile(cnt);
  CloseHandle(hct);
  return 0;
#endif
}

int APIENTRY LVGetStr(char *strp, int nDisplay)
{
#ifdef WINDOWS31
  if (bSetting[nDisplay]) {
    int i;
    for (i=0; i<1024; i++)
      strp[i] = DLLData[nDisplay].comment0[i];
    return 0;
  }
  else return 4;
#else
  int i;
  char sz[40];
  HANDLE hcm;
  char *comment0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MPA3_CM_%d",nDisplay);
  if (!(hcm = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(comment0 = (char *)MapViewOfFile(hcm,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hcm);
    return 4;
  }
  for (i=0; i<1024; i++)
    strp[i] = comment0[i];
  UnmapViewOfFile(comment0);
  CloseHandle(hcm);
  return 0;
#endif
}

VOID APIENTRY StoreStatusData(ACQSTATUS *Status, int nDev)
{
  if (nDev < 0 || nDev >= MAXDEV) return;
  if(Status == NULL)
    bStatus[nDev] = FALSE;
  else{
    _fmemcpy((LPSTR FAR *)&DLLStatus[nDev],
           (LPSTR FAR *)Status,sizeof(ACQSTATUS));
    bStatus[nDev] = TRUE;
  }
/*  {
	  char txt[100];
	  sprintf(txt,"StoreStatusData %d %d", nDisplay, bStatus[nDisplay]);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
  } */
}

int APIENTRY GetStatusData(ACQSTATUS *Status, int nDev)
{
  //DebugBreak();
  /* {
	  char txt[100];
	  sprintf(txt,"GetStatusData %d %d", nDev, bStatus[nDev]);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
  } */
  if (nDev < 0 || nDev > DLLDef.nDevices) return 0;
  if (bStatus[nDev]) {
    _fmemcpy((LPSTR FAR *)Status,
           (LPSTR FAR *)&DLLStatus[nDev],sizeof(ACQSTATUS));
  }
  return bStatus[nDev];
}

VOID APIENTRY Start(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_START, 0L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_START2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_START3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_START4, 0L);
     break;
  }
}

VOID APIENTRY Halt(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK, 0L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_BREAK4, 0L);
     break;
  }
}

VOID APIENTRY Continue(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE, 0L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_CONTINUE4, 0L);
     break;
  }
}

VOID APIENTRY SaveSetting()
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  PostMessage(hwndServer, WM_COMMAND, ID_SAVE, 0L);
}

VOID APIENTRY NewSetting(int nDev)
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  //if (nDev>=0 && nDev<8) bStatus[nDev] = FALSE;
  SendMessage(hwndServer, WM_COMMAND, ID_NEWSETTING, 0L);
}

VOID APIENTRY NewData()
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  PostMessage(hwndServer, WM_COMMAND, ID_NEWDATA, 0L);
}

int APIENTRY GetStatus(int nDev)
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (bStatus[nDev]) {
      SendMessage(hwndServer, WM_COMMAND, ID_GETSTATUS, 0L);
  }
  /* {
	  char txt[100];
	  sprintf(txt,"GetStatus %d %d", nDev, bStatus[nDev]);
	  MessageBox(NULL, txt, "DMCD2.DLL", MB_OK);
  } */
  return bStatus[nDev];
}

UINT APIENTRY ServExec(HWND ClientWnd)
{
  bRemote = 1;
  hwndClient = ClientWnd;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (hwndServer) {
    ShowWindow(hwndServer, SW_MINIMIZE);
    return 32;
  }
  else
    return WinExec("MPA3.EXE", SW_SHOW);
}

UINT APIENTRY ClientExec(HWND ServerWnd)
{
  if (ServerWnd) hwndServer = ServerWnd;
  return WinExec((LPSTR)"MPANT /device=MPA3", SW_SHOW);
}

VOID APIENTRY UnregisterClient()
{
  hwndClient = 0;
  bRemote = 0;
}

VOID APIENTRY DestroyClient()
{
  bRemote = 0;
  if (hwndClient) SendMessage(hwndClient, WM_CLOSE, 0, 0L);
  hwndClient = 0;
}

VOID APIENTRY Erase(int nSystem)
{
  if (nSystem < 0 || nSystem > 3) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  switch (nSystem) {
    case 0:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE, 0x10000L);
     break;
    case 1:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE2, 0L);
     break;
    case 2:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE3, 0L);
     break;
    case 3:
     PostMessage(hwndServer, WM_COMMAND, ID_ERASE4, 0L);
     break;
  }
}

VOID APIENTRY SaveData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  PostMessage(hwndServer, WM_COMMAND, ID_SAVEFILE, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY LoadData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
//  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_LOADFILE, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY AddData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
//  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_SUMFILE, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY SubData(int nDisplay, int all)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
//  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_SUBTRACT, 
		  MAKELPARAM((WORD)nDisplay, (WORD)all));
}

VOID APIENTRY Smooth(int nDisplay)
{
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
//  bStatus[nDisplay] = FALSE;
  PostMessage(hwndServer, WM_COMMAND, ID_SMOOTH, 
		  MAKELPARAM((WORD)nDisplay, (WORD)0));
}

VOID APIENTRY HardwareDlg(int item)
{
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  switch (item) {
    case 0:
      PostMessage(hwndServer, WM_COMMAND, ID_HARDWDLG, 0L);
      break;
    case 1:
      PostMessage(hwndServer, WM_COMMAND, ID_DATADLG, 0L);
      break;
    case 2:
      PostMessage(hwndServer, WM_COMMAND, ID_COMBDLG, 0L);
      break;
    case 3:
      PostMessage(hwndServer, WM_COMMAND, ID_MAPLSTDLG, 0L);
      break;
    case 4:
      PostMessage(hwndServer, WM_COMMAND, ID_REPLDLG, 0L);
      break;
  }
}

VOID APIENTRY RunCmd(int nDisplay, LPSTR Cmd)
{
#ifdef WINDOWS31
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  if (Cmd != NULL) {
    _fstrcpy(&DLLData[0].comment0[800], Cmd);
  }
#else
  char sz[40];
  HANDLE hcm;
  char *comment0;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return;
  if (!bSetting[nDisplay]) return;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  sprintf(sz,"MPA3_CM_%d",nDisplay);
  if (!(hcm = OpenFileMapping(FILE_MAP_WRITE, FALSE, sz)))
	return;
  if (!(comment0 = (char *)MapViewOfFile(hcm,
          FILE_MAP_WRITE, 0, 0, 0))) {
    CloseHandle(hcm);
    return;
  }
  strcpy(&comment0[800], Cmd);
#endif
//  SendMessage(hwndServer, MM_NEARCONTROL, (WPARAM)nDisplay, (LONG)(LPSTR)Cmd);
  SendMessage(hwndServer, MM_NEARCONTROL, (WPARAM)ID_RUNCMD, (LONG)(LPSTR)Cmd);
#ifndef WINDOWS31
  strcpy(Cmd, &comment0[1024]);
  UnmapViewOfFile(comment0);
  CloseHandle(hcm);
#endif
}

long APIENTRY GetSVal(int DspID, long xval)
{
  long val=0;
  if (xval == -2) {
	  hwndMPANT = FIND_WINDOW("mpwframe",NULL);
	  return (long)hwndMPANT;  
	  // should be called first to be sure that MPANT is started
  }
  if (!hwndMPANT) hwndMPANT = FIND_WINDOW("mpwframe",NULL);
  if (!MM_GETVAL) MM_GETVAL = RegisterWindowMessage((LPSTR)"MPANTGetval");
  val = (long)SendMessage(hwndMPANT, MM_GETVAL, (WPARAM)DspID, (LPARAM)xval);
    // for xval == -1 returns Display size
  return val;
}

int APIENTRY DigInOut(int value, int enable)  // controls Dig I/0 ,
								  // returns digin
{
  int val=0;
  long lval;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  lval = ((long)value & 0xFF) | ((enable & 0xFF)<<8);
  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_DIGINOUT, (LONG)lval);
  return val;
}

int APIENTRY DacOut(int value)    // output Dac value as analogue voltage
{
  int val=0;
  long lval;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  value &= 0xFF;
  lval = (long)value;
  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_DACOUT, (LONG)lval);
  return val;
}

long APIENTRY GetRoiIndex(LPSTR roiname)
	// for named ROI's returns in LOWORD the spectra number, in HIWORD the roiid,
    // returns 0 if not found.
{
  int val=0;
  char sz[40];
  HANDLE hcm;
  char *comment0;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  strcpy(sz,"MPA3_CM_0");
  if (!(hcm = OpenFileMapping(FILE_MAP_WRITE, FALSE, sz)))
	return 0;
  if (!(comment0 = (char *)MapViewOfFile(hcm,
          FILE_MAP_WRITE, 0, 0, 0))) {
    CloseHandle(hcm);
    return 0;
  }
  strncpy(&comment0[800], roiname, 20);
  comment0[820] = '\0';

  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_GETROIINDEX, 0);
  UnmapViewOfFile(comment0);
  CloseHandle(hcm);

  return val;
}

int APIENTRY DeleteRoi(DWORD roiindex)
    // deletes ROI
{
  int val=0;
  HWND hwndClient = FIND_WINDOW("mpwframe",NULL);
  if (hwndClient) 
    return (int)SendMessage(hwndClient, WM_COMMAND, ID_DELETEROI, roiindex);
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_DELETEROI, (LONG)roiindex);
  return val;
}

int APIENTRY SelectRoi(DWORD roiindex)
    // selects ROI
{
  int val=0;
  HWND hwndClient = FIND_WINDOW("mpwframe",NULL);
  if (hwndClient) 
    return (int)SendMessage(hwndClient, WM_COMMAND, ID_SELECTROI, roiindex);
  return val;
}

int APIENTRY GetRoiSum(DWORD roiindex, double *sum)
	// get sum of counts in ROI, 
	// returns roiindex= in LOWORD the spectra number, in HIWORD the roiid, or 0 if not found
{
  int val=0, nDisplay, roiid;
  if (!hwndServer) hwndServer = FIND_WINDOW("MPA-3 Server",NULL);
  if (!MM_NEARCONTROL) MM_NEARCONTROL = RegisterWindowMessage((LPSTR)"MPANEARCONTROL");
  val = (int)SendMessage(hwndServer, MM_NEARCONTROL, ID_GETROISUM, (LONG)roiindex);
  if (val) {
	  roiid = HIWORD(roiindex);
	  if (roiid < 100) nDisplay = LOWORD(roiindex);
	  else nDisplay = roiid/100;
	  if(LVGetOneCnt(sum, nDisplay, 7)) return 0;
    // cnt[7] contains the sum, cnt[8] contains the area   
  }
  return val;
}

int APIENTRY BytearrayToShortarray(short *Shortarray, char *Bytearray, int length)
{
  int i;
  char c;
  for (i=0; i<length; i++) {
	  c = Bytearray[i];
	  Shortarray[i] = c;
	  if (!c) {
		  i++; break;
	  }
  }
  return i;
}

/*
#define ROINAMELEN 21		// max. 20 characters + '\0'
#define MAXREGIONS 128		// max. 1D regions
#define MAXROI     64		// max. polygonal ROIs
#define MAXROINAM  (MAXREGIONS + MAXROI)
*/

int APIENTRY LVGetRoinam(char *strp, int nDisplay)	// get Roi names
{
  int i;
  char sz[40];
  HANDLE hcm;
  char *roinam;
  if (nDisplay < 0 || nDisplay >= MAXDSP) return 4;
  if (!bSetting[nDisplay]) return 4;
  sprintf(sz,"MPA3_RN_%d",nDisplay);
  if (!(hcm = OpenFileMapping(FILE_MAP_READ, FALSE, sz)))
	return 4;
  if (!(roinam = (char *)MapViewOfFile(hcm,
          FILE_MAP_READ, 0, 0, 0))) {
    CloseHandle(hcm);
    return 4;
  }
  for (i=0; i<4032; i++)		// (MAXREGIONS + MAXROI) * ROINAMELEN
    strp[i] = roinam[i];
  UnmapViewOfFile(roinam);
  CloseHandle(hcm);
  return 0;
}

int APIENTRY LVGetSpecLength(int nDisplay)
{
  if (bSetting[nDisplay]) 
	  return DLLSetting[nDisplay].range;
  else
	  return -1;
}

int APIENTRY LVGetDatSetting(LVDATSETTING *Defdat, LPSTR filename, LPSTR specfile, LPSTR command)
{
  int i;
  if (bDat) {
    _fmemcpy((LPSTR FAR *)Defdat,(LPSTR FAR *)&DLLdat,sizeof(LVDATSETTING));
	for (i=0; i<256; i++) {
	  filename[i] = DLLdat.filename[i];
	  specfile[i] = DLLdat.specfile[i];
	  command[i] = DLLdat.command[i];
	}
	return 0;
  }
  return -1;
}

int APIENTRY LVGetReplaySetting(LVREPLAYSETTING *Repldat, LPSTR filename)
{
  int i;
  if (bRepl) {
    _fmemcpy((LPSTR FAR *)Repldat,(LPSTR FAR *)&DLLRepl,sizeof(LVREPLAYSETTING));
	for (i=0; i<256; i++) {
	  filename[i] = DLLRepl.filename[i];
	}
	return 0;
  }
  return -1;
}

