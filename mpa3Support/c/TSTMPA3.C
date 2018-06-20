// --------------------------------------------------------------------------
// TSTMPA3.C : DMPA3.DLL Software driver C example
// --------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#undef DLL
#include "dmpa3.h"


HANDLE          hDLL = 0;

IMPAGETSETTING  lpSet=NULL;
IMPANEWSTATUS   lpNewStat=NULL;
IMPAGETSTATUS   lpStat=NULL;
IMPARUNCMD      lpRun=NULL;
IMPAGETCNT      lpCnt=NULL;
IMPAGETROI      lpRoi=NULL;
IMPAGETDAT      lpDat=NULL;
IMPAGETSTR      lpStr=NULL;
IMPASERVEXEC    lpServ=NULL;
IMPAGETDATSET   lpGetDatSet=NULL;
IMPAGETMP3SET   lpGetMp3Set=NULL;
IMPADIGINOUT    lpDigInOut=NULL;
IMPADACOUT      lpDacOut=NULL;
IMPASTART		lpStart=NULL;
IMPAHALT		lpHalt=NULL;
IMPACONTINUE    lpContinue=NULL;
IMPAERASE		lpErase=NULL;

ACQSETTING     Setting={0};
ACQDATA        Data={0};
ACQDEF         Def={0};
ACQSTATUS      Status={0};
DATSETTING     DatSetting={0};
ACQMP3		   MP3Setting={0};

short nDev=0;

void help()
{
	printf("Commands:\n");
	printf("Q		Quit\n");
	printf("?		Help\n");
	printf("S       Show Status\n");
	printf("H		Halt\n");
	printf("T       Show Setting\n");
	printf("ADC=x   Switch to ADC #x (0=MPA)\n");
    printf("(... more see command language in MPANT help)\n");
    printf("\n");
}

void PrintMpaStatus(ACQSTATUS *Stat)
{
  if(Stat->val) printf("ON\n"); else printf("OFF\n");
  printf("realtime= %.2lf\n", Stat->cnt[ST_REALTIME]);
  printf("runtime= %.2lf\n", Stat->cnt[ST_RUNTIME]);
  printf("single_ev= %lf\n", Stat->cnt[ST_SINGLESUM]);
  printf("coinc_ev= %lf\n", Stat->cnt[ST_COINCSUM]);
  printf("sglrate= %.2lf\n", Stat->cnt[ST_SGLRATE]);
  printf("coirate= %.2lf\n", Stat->cnt[ST_COIRATE]);
}

void PrintStatus(ACQSTATUS *Stat)
{
  printf("livetime= %.2lf\n", Stat->cnt[ST_LIVETIME]);
  printf("deadtime%%= %.2lf\n", Stat->cnt[ST_DEADTIME]);
  printf("totalsum= %lf\n", Stat->cnt[ST_TOTALSUM]);
  printf("roisum= %lf\n", Stat->cnt[ST_ROISUM]);
  printf("netsum= %lf\n", Stat->cnt[ST_NETSUM]);
  printf("totalrate= %.2lf\n", Stat->cnt[ST_TOTALRATE]);
}

void PrintDatSetting(DATSETTING *Set)
{
  printf("savedata= %d\n", Set->savedata);
  printf("autoinc= %d\n", Set->autoinc);
  printf("fmt= %d\n", Set->fmt);
  printf("sepfmt= %d\n", Set->sepfmt);
  printf("sephead= %d\n", Set->sephead);
  printf("filename= %s\n", Set->filename);
}

void PrintMP3Setting(ACQMP3 *Set)
{
  printf("rtcuse= %x\n", Set->rtcuse);
  printf("dac= %d\n", Set->dac);
  printf("diguse= %x\n", Set->diguse);
  printf("digval= %d\n", Set->digval);
  printf("rtprena= %x\n", Set->rtprena);
  printf("rtpreset= %lg\n", Set->rtpreset);
}

void PrintSetting(ACQSETTING *Set)
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

int run(char *command)
{
	int err;
	if (!stricmp(command, "?"))           help();
	else if (!stricmp(command,"Q"))       return 1;
	else if (!stricmp(command,"S")) {
      err = (*lpStat)(&Status, nDev);
	  if (nDev) PrintStatus(&Status);
	  else PrintMpaStatus(&Status);
	}
	else if (!stricmp(command,"T")) {
	  if (nDev) {	// spectra settings
        err = (*lpSet)(&Setting, nDev-1);
		printf("ADC %d:\n", nDev);
		PrintSetting(&Setting);
	  }
	  else {        // MPA3 settings
        err = (*lpGetMp3Set)(&MP3Setting);
		PrintMP3Setting(&MP3Setting);
		            // DATSettings
        err = (*lpGetDatSet)(&DatSetting);
		PrintDatSetting(&DatSetting);
	  }
	}
	else if (!stricmp(command,"H")) {
      (*lpHalt)(0);
	}
	else if(!strnicmp(command, "ADC=", 4)) {
	  sscanf(command+4, "%d", &nDev);
	  (*lpRun)(0, command);
	}
	else if (!stricmp(command,"MPA")) {
	  nDev=0;
	  (*lpRun)(0, command);
	}
	else {
		(*lpRun)(0, command);
		printf("%s\n", command);
	}
	return 0;
}

//int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow)
void main(int argc, char *argv[])  
{
  long Errset=0, Erracq=0, Errread=0;
  char command[80];

  hDLL = LoadLibrary("DMPA3.DLL");
  if(hDLL){
      lpSet=(IMPAGETSETTING)GetProcAddress(hDLL,"GetSettingData");
	  lpNewStat=(IMPANEWSTATUS)GetProcAddress(hDLL,"GetStatus");
	  lpStat=(IMPAGETSTATUS)GetProcAddress(hDLL,"GetStatusData");
	  lpRun=(IMPARUNCMD)GetProcAddress(hDLL,"RunCmd");
	  lpCnt=(IMPAGETCNT)GetProcAddress(hDLL,"LVGetCnt");
	  lpRoi=(IMPAGETROI)GetProcAddress(hDLL,"LVGetRoi");
	  lpDat=(IMPAGETDAT)GetProcAddress(hDLL,"LVGetDat");
	  lpStr=(IMPAGETSTR)GetProcAddress(hDLL,"LVGetStr");
	  lpServ=(IMPASERVEXEC)GetProcAddress(hDLL,"ServExec");
	  lpGetDatSet=(IMPAGETDATSET)GetProcAddress(hDLL,"GetDatSetting");
	  lpGetMp3Set=(IMPAGETMP3SET)GetProcAddress(hDLL,"GetMP3Setting");
	  lpDigInOut=(IMPADIGINOUT)GetProcAddress(hDLL,"DigInOut");
	  lpDacOut=(IMPADACOUT)GetProcAddress(hDLL,"DacOut");
	  lpStart=(IMPASTART)GetProcAddress(hDLL,"Start");
	  lpHalt=(IMPAHALT)GetProcAddress(hDLL,"Halt");
	  lpContinue=(IMPACONTINUE)GetProcAddress(hDLL,"Continue");
	  lpErase=(IMPAERASE)GetProcAddress(hDLL,"Erase");
  }
  else return;

  // Initialize parameters
//  Errset = (*lpServ)(0);
  Errset = (*lpNewStat)(0);
  Errset = (*lpStat)(&Status, 0);
  PrintMpaStatus(&Status);

  /*
  (*lpSet)(&Setting, 0);  
  PrintSetting(&Setting);
  */
 
  printf("\nCommands:\n");
  help();

  while(TRUE)
	{
		scanf("%s", command);
		if (run(command)) break;
	}

  FreeLibrary(hDLL);

  return;
}
