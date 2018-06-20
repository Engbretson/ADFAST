// tstrxy.c: Program to create a table of Correction factors R(x,y)
// for test purposes

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>

#define SQ8LN2	2.35482

double xx0 = 127.5;
double yy0 = 127.5;
double fwhm = 1024.;
double sigma;
int ndim=256;
char filename[256]="testrxy.txt";

int readstr(char *buff, int buflen)
{
  int i=0,ic;

  while ((ic=getchar()) != 10) {
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

double rxy(long x, long y)
{
	double rx, ry;
	rx = (x-xx0) / sigma;
	rx = rx * rx;
	rx = exp( -0.5 * rx);

	ry = (y-yy0) / sigma;
	ry = ry * ry;
	ry = exp( -0.5 * ry);

	return (1./(rx*ry));
}

void main(int argc, char *argv[])  
{
	double v;
	long x,y;
	FILE *f;
	char command[80];
	
	printf("Create a table of n x n correction factors for test purposes...\n");
	printf("Assumed is a Gaussian shape for the spatial detector energy calibration factor\n");
	printf("n= (256)");
	readstr(command, 80);
	sscanf(command, "%d", &ndim);

	v = ((double)(ndim-1))/2.;
	printf("x0= (%lg) ", v);
	readstr(command, 80);
	sscanf(command, "%lf", &xx0);
	if (xx0 <= 0.) xx0 = v;

	printf("y0= (%lg) ", v);
	readstr(command, 80);
	sscanf(command, "%lf", &yy0);
	if (yy0 <= 0.) yy0 = v;

	fwhm = 4. * ndim;
	printf("fwhm= (%lg) ", fwhm);
	readstr(command, 80);
	sscanf(command, "%lf", &fwhm);
	if (fwhm <= 0.) fwhm = 4. * ndim;
    sigma=fwhm/SQ8LN2;

	printf("Filename: (%s) ", filename);
	readstr(command, 80);
	if (!stricmp(command, "N")) exit(1);
	if (command[0] && strlen(command) > 1) strcpy(filename, command);

	f = fopen(filename, "wt");
	fprintf(f,"xdim=%ld\n", ndim);
	fprintf(f,"ydim=%ld\n", ndim);
	fprintf(f,"[DATA] x y R\n");
	for (y=0; y<ndim; y++) {
	  for (x=0; x<ndim; x++) {
		v = rxy(x,y);
	    fprintf(f,"%ld\t%ld\t%lg\n", x, y, v);
	  }
	}
	fclose(f);
}
