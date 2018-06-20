/***************************************************************************
  MODUL:    custom.C
  PURPOSE:  Customer module of DMAP3.DLL to communicate with MPA3 Server
****************************************************************************/

#include "windows.h"
#include <string.h>
#include <stdio.h>
#define DLL
#include "dmpa3.h"
#include "custom.h"

long xdim=256, ydim=256, ndim;
HANDLE hTab=0;
double *lpTab=NULL;

int freadstr(FILE *stream, char *buff, int buflen)
{
  int i=0,ic;

  while ((ic=getc(stream)) != 10) {
    if (ic == EOF) {
      buff[i]='\0';
      return 1;
    }
    if (ic == 13) ic=0;
    buff[i]=(char)ic;
    i++;
    if (i==buflen-1) break;
  }
  buff[i]='\0';
  return 0;
}

VOID APIENTRY CloseTab()
{
  if (lpTab) 
    GlobalUnlock(lpTab);
  if (hTab) 
    GlobalFree(hTab);
  hTab=0;
  lpTab=NULL;
}

int APIENTRY IniTab(LPSTR filename)
{
	FILE *f;
	long i, j, x, y;
	int ret;
	double *pf;
    char txt[256];

	if (!filename) return -1;
	if (!(f = fopen(filename, "rb"))) {
	  sprintf(txt, "Filename %s not found!", filename);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
	  return -1;
	}
    while(!freadstr(f,txt,256)) {
      if(!strnicmp("xdim=", txt, 5)) {
        sscanf(txt+5,"%ld",&xdim);
      }
      else if(!strnicmp("ydim=", txt, 5)) {
        sscanf(txt+5,"%ld",&ydim);
      }
      if(!strnicmp("[DATA", txt, 5)) {
        break;
      }
	}
	ndim = xdim * ydim;
    hTab = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,(DWORD)sizeof(double)*ndim);
    if (!hTab) {
	  MessageBox(NULL, "No memory!", "DMPA3.DLL", MB_OK);
 	  return -1;
	}
	lpTab = (double *) GlobalLock(hTab);
	pf = lpTab;
    for (j=0; j<ydim; j++) {
	  for (i=0; i<xdim; i++) {
		ret = (freadstr(f, txt, 256));
		sscanf(txt, "%ld %ld %lf", &x, &y, pf);
		pf++;
		if (ret) goto out;
	  }
	}
out:
	if (j < ydim) {
	  sprintf(txt, "Table ends at x=%ld y=%ld", i, j);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
	  CloseTab();
	  return -1;
	}
	return 0;
}

unsigned long APIENTRY RTab(long x, long y, long z)
{
  long val;
  if (lpTab==NULL) return 0;
  if ((x >= xdim) || (y >= ydim)) return 0;
  val = (long) (z * lpTab[y*xdim + x]);
  if (val > 0) return (unsigned long)val;
  else return 0;
}

unsigned long sweep=0;
long swroimin=0;
long swroimax=65536;
long swstep=1;
int APIENTRY IniSweep(LPSTR filename)
{
	FILE *f;
    char txt[256];

	sweep=0;
	swroimin=0;
	swroimax=65536;
	swstep=1;
	if (!filename) return -1;
	if (!strnicmp("dummy", filename, 5)) return 0;
	if (!(f = fopen(filename, "rb"))) {
	  sprintf(txt, "Filename %s not found!", filename);
	  MessageBox(NULL, txt, "DMPA3.DLL", MB_OK);
	  return -1;
	}
    while(!freadstr(f,txt,256)) {
      if(!strnicmp("swroimin=", txt, 9)) {
        sscanf(txt+9,"%ld",&swroimin);
      }
      else if(!strnicmp("swroimax=", txt, 9)) {
        sscanf(txt+9,"%ld",&swroimax);
      }
      else if(!strnicmp("swstep=", txt, 7)) {
        sscanf(txt+7,"%ld",&swstep);
		if (swstep <= 0) swstep = 1;
      }
	}
    fclose(f);
	return 0;
}

unsigned long APIENTRY IncSweep(long x, long y, long z)
{
  if ((x >= swroimin) && (x < swroimax)) sweep++;
  return (sweep / swstep);
}

