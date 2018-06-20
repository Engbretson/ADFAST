#define  WINDOWSNT
#undef  WINDOWS95
#undef  WINDOWS31

#ifdef WINDOWS31

#define GET_WM_COMMAND_ID(w)  w
#define GET_WM_COMMAND_CMD(w,l) HIWORD(l)
#define GET_WM_COMMAND_HWND(l) LOWORD(l)
#define GET_WM_SCRHWND(l) HIWORD(l)
#define GET_WM_SCROLLPOS(w,l) LOWORD(l)
#define FIND_WINDOW(a,b) FindWindow(b,a)
#define HUGE huge
#define USHORT unsigned short
#define SetForegroundWindow(w)
#define APIENTRY FAR PASCAL

#else

#define GET_WM_COMMAND_ID(w)  LOWORD(w)
#define GET_WM_COMMAND_CMD(w,l) HIWORD(w)
#define GET_WM_COMMAND_HWND(l) l
#define GET_WM_SCRHWND(l) l
#define GET_WM_SCROLLPOS(w,l) (short)HIWORD(w)
#define FIND_WINDOW(a,b) FindWindow(a,b)
#define HUGE
#define _fmemcpy memcpy
#define _fstrcpy strcpy

#endif


#define ST_LIVETIME   0
#define ST_DEADTIME   1
#define ST_TOTALSUM   2
#define ST_ROISUM     3
#define ST_TOTALRATE  4
#define ST_NETSUM     5

typedef struct{
  unsigned long val;    // Maximum value in spectrum 
  unsigned long val1;   // reserved
  double cnt[6];        // ADC status: Livetime in msec, Deadtime in percent, 
						// total sum, roi sum, total rate, net sum
} ACQSTATUS;

#define ST_REALTIME   0
#define ST_RUNTIME    1
#define ST_SINGLESUM  2
#define ST_COINCSUM   3
#define ST_SGLRATE    4
#define ST_COIRATE    5

    // MPA status: real time in msec, elapsed computer-runtime, total single events
	// total coinc events, rate of single events, rate of coinc events
	// val=aquisition status: 0= HALT, 1= ON

typedef struct {
  long savedata;         // 1 if auto save after stop
  long autoinc;          // 1 if auto increment filename
  long fmt;              // format type: 0 == ASCII, 1 == binary,
                         // 2 == GANAAS
  long sepfmt;           // format for seperate spectra: 
  long sephead;          // seperate Header 
  long smpts;            // number of points for smoothing operation
  long caluse;			 // 1 for using calibration for shifted spectra summing
  char filename[256];    // mpa data filename
  char specfile[256];	 // seperate spectra filename
  char command[256];	 // command
} DATSETTING;

typedef struct {
  long use;              // 1 if Replay Mode ON
  long modified;         // 1 if different settings are used
  long limit;            // 0: all, 
                        // 1: limited time range
  long speed;			// replay speed in units of 100 kB / sec
  double timefrom;      // first time (sec)
  double timeto;        // last time (sec)
  double timepreset;    // last time - first time
  char filename[256];
} REPLAYSETTING;

typedef struct{
  long range;            // spectrum length
  long prena;            // bit 0: livetime preset enabled
                         // bit 1: ROI preset enabled
  long roimin;           // lower ROI limit
  long roimax;           // upper limit: roimin <= channel < roimax
  long nregions;         // number of regions
  long caluse;           // bit0: 1 if calibration used, higher bits: formula
  long calpoints;        // number of calibration points
  long param;            // for MAP and POS: LOWORD=x, HIGHWORD=y (rtime=256, RTC=257)
  long offset;           // zoomed MAPS: LOWORD: xoffset, HIGHWORD, yoffset
  long xdim;			// x resolution of maps
  long timesh;          // bitshift for timespectra
  long active;          // Spectrum definition words for ADC1..16:
                           // active & 0xF  ==0 not used 
                           //               ==1 single
                           //               ==2 coinc with any
						   // bit 4..7 in group 1..4 for ADCs
						// Spectrum definition words for calc. spectra:
                           // active & 0xF  ==3 MAP, ((x-xoffs)>>xsh) x ((y-yoffs)>>ysh)
                           //               ==0xB TIM, MAP with RTC or rtime as x or y
                           //                 ((x-xoffs)>>xsh) x ((y-timeoffs)>>timesh)  
                           //              or ((x-timeoffs)>>timesh x ((y-yoffs)>>ysh)  
						   //         bit4=1: x zoomed MAP
						   //         bit5=1: y zoomed MAP
						   //               ==4 POS, (y<<xsh) /(x + y)
						   //               ==5 SUM, (x + y)
						   //               ==6 DIV, (x<<xsh) / y
                           // bit 8..11 xsh, bit 12..15 ysh or bit 8..15 xsh
                           // bit 8..11 xsh, bit 12..15 ysh
  double roipreset;      // ROI preset value
  double ltpreset;      // livetime preset value
  double timeoffs;		// offset for time spectra
  double dwelltime;     // binsize for time spectra
} ACQSETTING;

typedef struct{
  long range;            // spectrum length
  long prena;            // bit 0: livetime preset enabled
                         // bit 1: ROI preset enabled
  long roimin;           // lower ROI limit
  long roimax;           // upper limit: roimin <= channel < roimax
  long nregions;         // number of regions
  long caluse;           // bit0: 1 if calibration used, higher bits: formula
  long calpoints;        // number of calibration points
  long param;            // for MAP and POS: LOWORD=x, HIGHWORD=y (rtime=256, RTC=257)
  long offset;           // zoomed MAPS: LOWORD: xoffset, HIGHWORD: yoffset
  long xdim;			// x resolution of maps
  long timesh;          // bitshift for timespectra
  long active;          // Spectrum definition words for ADC1..16:
                           // active & 0xF  ==0 not used 
                           //               ==1 single
                           //               ==2 coinc with any
						   // bit 4..7 in group 1..4 for ADCs
						// Spectrum definition words for calc. spectra:
                           // active & 0xF  ==3 MAP, ((x-xoffs)>>xsh) x ((y-yoffs)>>ysh)
                           //               ==0xB TIM, MAP with RTC or rtime as x or y
                           //                 ((x-xoffs)>>xsh) x ((y-timeoffs)>>timesh)  
                           //              or ((x-timeoffs)>>timesh x ((y-yoffs)>>ysh)  
						   //         bit4=1: x zoomed MAP
						   //         bit5=1: y zoomed MAP
						   //               ==4 POS, (y<<xsh) /(x + y)
						   //               ==5 SUM, (x + y)
						   //               ==6 DIV, (x<<xsh) / y
                           // bit 8..11 xsh, bit 12..15 ysh or bit 8..15 xsh
                           // bit 8..11 xsh, bit 12..15 ysh
  double roipreset;      // ROI preset value
  double ltpreset;      // livetime preset value
  double timeoffs;		// offset for time spectra
  double dwelltime;     // binsize for time spectra
						// MPANT or Server private saved settings:
  long type;			// 0=single, 1=MAP, 2=ISO...
  long ydim;			// y resolution of maps
  long reserved[12];
} EXTACQSETTING;

typedef struct{
  unsigned long HUGE *s0;         // pointer to spectrum
  unsigned long *region;          // pointer to regions
  unsigned char *comment0; // pointer to strings
  double *cnt;             // pointer to counters
  HANDLE hs0;
  HANDLE hrg;
  HANDLE hcm;
  HANDLE hct;
} ACQDATA;

typedef struct {
  unsigned int sen;              // Start Enable Register
				// 1 in Bit 0(..15) means ADC 1A(..4D) 
				// starts coincidence window
  unsigned int coi;      // Coincidence Control Register
				// 1 in Bit 0(..15) means ADC 1A(..4D) 
				// is in coincidence mode, 
				// otherwise in single mode
  unsigned int ctm;              // Coinc. Time in units of 50 ns
  unsigned int dtm;      // Data Ready Timeout in units of 50 ns
  unsigned int tct;      // Time Stamp Control Register
				// Bit 0..1: Count Source: 
				//	   0=20MHz,    1=AUX2edge, 
				//	   2=AUX1edge, 3=REJedge
				// Bit 2..3: Counter Enable:
				//	   0=ON,       1=AUX2&ON, 
				//     2=AUX1&ON,  3=REJ&ON
				// Bit 4..7: Time Capture:
				//		0=OR-ed DEAD edge(DOR), 
				//		1=DOR+AUX2, 
				//		2=DOR+AUX1,
				//		3=DOR+REJ,
				//		4=coinc_start
				//		5=coinc_end
				//		6=coinc_ok
				//		7=Software
				// Bit8..10: Timer Load:
				//		0=Software, 1=AUX2
				//		2=AUX1, 3=REJ,
				//		4=Wrap around
				// Bit 11: timer 'preset reached' stops acquisition
				// Bit 12: timer 'preset reached' in PCI status
				// Bit 13: enable preset
  unsigned int tp0;      // Timer Preset 0 Register  0..65535
  unsigned int tp1;      // Timer Preset 1 Register
  unsigned int tp2;      // Timer Preset 2 Register
				// The timer preset is 
				// (tp2 * 65536 + tp1) * 65536 + tp0
  unsigned int aui;      // Aux In Control
				// Bit 0: AUX2 polarity, 1=active low
				// Bit 1: AUX1 polarity 
				// Bit 2: AUX2 start coinc.window enable
				// Bit 3: AUX1 start enable
				// Bit 4: AUX2 coinc. mode
				// Bit 5: AUX1 coinc. mode
				// Bit 12: REJ coinc. mode
				// Bit 13: REJ polarity
				// Bit 15: Reject mode:
				//	0=instant, 1=at end of coinc.time
  unsigned int auo;      // Aux Out Control
				// Bit 0..3: AUX2 output:
				//		0=coinc_start,  1=coinc_run,
				//		2=coinc_active, 3=coinc_ok,
				//		4=dead_store,   5=enca (ON),
				//		6=1, 7=0
				// Bit 4..7: AUX1 output:
				//		0=coinc_start,  1=coinc_run,
				//		2=coinc_active, 3=coinc_ok,
				//		4=dead_store,   5=enca (ON),
				//		6=1, 7=0, 
				//		8=preset reached, 9=timer load
				// Bit 8:  AUX2 output enable
				// Bit 9:  AUX1 output enable
				// Bit 10: AUX2 mirror GO line (Bit0..3=7)
				// Bit 11: AUX1 mirror GO line (Bit4..7=7)
  unsigned int dor;              // Timer Dead OR
				// 1 in Bit 0 (..15) for ADC 1A(..4D) 
				// to capture RTC time, see tct description
  unsigned int bk0;              // Block Routing Control 0
				// Allows to route more than 16 FMP bus subadresses
				// default=0 for 1 to 1 routing of 16 ADC interfaces
  unsigned int bk1;      // Block Routing Control 1, default=0
  unsigned int rtcuse;   // Bit 0: Timestamp in datastream
				// Bit 1: Halt when preset reached
  unsigned int dac;      // Bit 0..7: DAC output value (8 bit)
  unsigned int diguse;   // Usage of DIG I/O
                // Bit 0:  DIG I/O bit 7 output status
                // Bit 1:  Invert Polarity
                // Bit 2:  Input bit 6 Trigger System
                // Bit 6:  Output digval and increment digval after stop
                // Bit 7:  Input Mode: =0 Resistive, =1 Tristate
                // Bit 8:  Input Mode: =0 Pullup, =1 Pulldown
  unsigned int digval ;  // DIG I/O Output value
  unsigned int rtprena;  // Realtime Preset enable
  double rtpreset;	// Realtime Preset
} ACQMP3;

typedef struct {
  int nDevices;          // Number of connected ADC Interfaces = max. 16
  int nDisplays;         // Number of histograms = nDevices + Positions + Maps
  int nSystems;          // Number of independent systems = 1
  int bRemote;           // 1 if server controlled by MPANT
  int auxsys;			 // System definition words for AUXx  
                           // auxsys & 0xFF ==0 not used 
                           //               ==2 coinc with any
						   // bit 4..7 in group 1..4
  int sys0[16];          // System definition words for ADC1..16:
                         // see active definition in ACQSETTING
  int sys1[16];          // ADC in System (always 1)
} ACQDEF;

