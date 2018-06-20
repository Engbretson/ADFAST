program Testmpa3;
{$APPTYPE CONSOLE}
{$X+}
uses
  Windows, sysutils;

const   ST_LIVETIME  =  0;
        ST_DEADTIME  =  1;
        ST_TOTALSUM  =  2;
        ST_ROISUM    =  3;
        ST_TOTALRATE =  4;
        ST_NETSUM    =  5;
        ST_REALTIME  = 0;
        ST_RUNTIME   = 1;
        ST_SINGLESUM = 2;
        ST_COINCSUM  = 3;
        ST_SGLRATE   = 4;
        ST_COIRATE   = 5;

type    SmallIntPointer = ^SmallInt;


{Diese Typdefinitionen wurden vom File struct.h übernommen und in Delphi
 übersetzt}
   AcqStatusTyp = RECORD           //Status information
                val       : Cardinal;	// ADC : max value or MPA: aquisition status
                val1      : Cardinal;   // reserved
  		cnt       : array [0..5] of Double; // ADC status: Livetime in msec,
                                        // Deadtime in percent, total sum,
                                        // roi sum, total rate, net sum
                                        // MPA: realtime, runtime, singlesum,
                                        // coincsum, sglrate, coirate
                End;
   AcqStatusTypPointer = ^AcqStatusTyp;

   DatSettingTyp = RECORD               // Data settings
                savedata    : Cardinal; // 1 if auto save after stop
                autoinc     : Cardinal; // 1 if auto increment filename
                fmt         : Cardinal; // format type: 0 == ASCII, 1 == binary,
                                        // 2 == GANAAS
                sepfmt      : Cardinal; // format for seperate spectra
                sephead     : Cardinal; // seperate Header
                smpts       : Cardinal; // number of points for smoothing operation
                caluse      : Cardinal; // 1 for using calibration for shifted spectra summing
                filename    : array [0..255] of Char; // mpa data filename
                specfile    : array [0..255] of Char; // seperate spectra filename
                command     : array [0..255] of Char; // command
                End;

   ReplaySettingTyp = RECORD            // Replay settings
                use         : Cardinal; // 1 if Replay Mode ON
                modified    : Cardinal; // 1 if different settings are used
                limit       : Cardinal; // 0: all, 1: limited time range
                speed       : Cardinal; // replay speed in units of 100 kB / sec
                timefrom    : Double;   // first time (sec)
                timeto      : Double;   // last time (sec)
                timepreset  : Double;   // last time - first time
                filename    : array [0..255] of Char;
                End;

   AcqSettingTyp = RECORD		  // ADC or spectra Settings
  		range       : Cardinal;   // spectrum length, ADC range
  		prena       : Cardinal;   // bit 0: livetime preset enabled
                                          // bit 1: ROI preset enabled
   		roimin	    : Cardinal;   // lower ROI limit
  		roimax      : Cardinal;   // upper limit: roimin <= channel < roimax
  		nregions    : Cardinal; // number of regions
  		caluse      : Cardinal; // bit 0 == 1 if calibration used, higher bits: formula
  		calpoints   : Cardinal; // number of calibration points
  		param       : Cardinal; // for MAP and POS: LOWORD=x, HIGHWORD=y (rtime=256, RTC=257)
                offset      : Cardinal; // zoomed MAPS: LOWORD: xoffset, HIGHWORD: yoffset
  		xdim	    : Cardinal; // x resolution of maps
                timesh      : Cardinal; // bitshift for timespectra
  		active      : Cardinal; // Spectrum definition words for ADC1..16:
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
  		roipreset   : Double;   // ROI preset value
  		ltpreset    : Double;   // Livetime preset value (MCA)
  		timeoffs    : Double;   // offset for time spectra
                dwelltime   : Double;   // binsize for time spectra
                End;
   AcqSettingTypPointer = ^AcqSettingTyp;

   ExtAcqSettingTyp = RECORD		// Settings
  		range       : Cardinal;   // spectrum length, ADC range
  		prena       : Cardinal;   // bit 0: livetime preset enabled
                                          // bit 1: ROI preset enabled
   		roimin	    : Cardinal;   // lower ROI limit
  		roimax      : Cardinal;   // upper limit: roimin <= channel < roimax
  		nregions    : Cardinal; // number of regions
  		caluse      : Cardinal; // bit 0 == 1 if calibration used, higher bits: formula
  		calpoints   : Cardinal; // number of calibration points
  		param       : Cardinal; // for MAP and POS: LOWORD=x, HIGHWORD=y (rtime=256, RTC=257)
                offset      : Cardinal; // zoomed MAPS: LOWORD: xoffset, HIGHWORD: yoffset
  		xdim	    : Cardinal; // x resolution of maps
                timesh      : Cardinal; // bitshift for timespectra
  		active      : Cardinal; // Spectrum definition words for ADC1..16:
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
  		roipreset   : Double;   // ROI preset value
  		ltpreset    : Double;   // Livetime preset value (MCA)
  		timeoffs    : Double;   // offset for time spectra
                dwelltime   : Double;   // binsize for time spectra
		vtype       : Cardinal; // 0=single, 1=MAP, 2=ISO...
                ydim        : Cardinal; // y resolution of maps
		reserved    : array [0..12] of LongInt;
                End;
   ExtAcqSettingTypPointer = ^ExtAcqSettingTyp;

   AcqDataTyp = RECORD
    		s0          : Array of LongInt; // pointer to spectrum
  		region	    : Array of Cardinal; // pointer to regions
  		comment0    : Array of Char;     // pointer to strings
  		cnt	    : Array of Double;   // pointer to counters
  		hs0	    : Integer;
  		hrg	    : Integer;
  		hcm	    : Integer;
  		hct	    : Integer;
		End;

   AcqMp3Typ = RECORD
                sen         : Cardinal;       // Start Enable Register
				// 1 in Bit 0(..15) means ADC 1A(..4D)
				// starts coincidence window
                coi         : Cardinal;       // Coincidence Control Register
				// 1 in Bit 0(..15) means ADC 1A(..4D)
				// is in coincidence mode,
				// otherwise in single mode
                ctm         : Cardinal;       // Coinc. Time in units of 50 ns
                dtm         : Cardinal;       // Data Ready Timeout in units of 50 ns
                tct         : Cardinal;       // Time Stamp Control Register
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
                tp0         : Cardinal;       // Timer Preset 0 Register  0..65535
                tp1         : Cardinal;       // Timer Preset 1 Register
                tp2         : Cardinal;       // Timer Preset 2 Register
				// The timer preset is
				// (tp2 * 65536 + tp1) * 65536 + tp0
                aui         : Cardinal;       // Aux In Control
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
                auo         : Cardinal;       // Aux Out Control
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
                dor         : Cardinal;       // Timer Dead OR
				// 1 in Bit 0 (..15) for ADC 1A(..4D)
				// to capture RTC time, see tct description
                bk0         : Cardinal;       // Block Routing Control 0
				// Allows to route more than 16 FMP bus subadresses
				// default=0 for 1 to 1 routing of 16 ADC interfaces
                bk1         : Cardinal;       // Block Routing Control 1, default=0
                rtcuse      : Cardinal;       // Bit 0: Timestamp in datastream
				// Bit 1: Halt when preset reached
                dac         : Cardinal;       // Bit 0..7: DAC output value (8 bit)
                diguse      : Cardinal;       // Usage of DIG I/O
                // Bit 0:  DIG I/O bit 7 output status
                // Bit 1:  Invert Polarity
                // Bit 2:  Input bit 6 Trigger System
                // Bit 6:  Output digval and increment digval after stop
                // Bit 7:  Input Mode: =0 Resistive, =1 Tristate
                // Bit 8:  Input Mode: =0 Pullup, =1 Pulldown
                digval      : Cardinal;       // DIG I/O Output value
                rtprena     : Cardinal;       // Realtime Preset enable
                rtpreset    : Double;         // Realtime Preset
                End;


   AcqDefTyp = RECORD
  		nDevices    : Cardinal;    // Number of connected ADC Interfaces = max. 16
  		nDisplays   : Cardinal;    // Number of histograms = nDevices + Positions + Maps
  		nSystems    : Cardinal;    // Number of independent systems = 1
      		bRemote	    : Cardinal;    // 1 if server controlled by MPANT
  		auxsys	    : Cardinal;     // System definition words for AUXx
                                // auxsys & 0xFF ==0 not used
                                //               ==2 coinc with any
			        // bit 4..7 in group 1..4
                sys0        : array [0..15] of Cardinal;
                                // System definition words for ADC1..16:
                                // see active definition in ACQSETTING
                sys1        : array [0..15] of Cardinal;
                                // ADC in System (always 1)
                End;
   AcqDefTypPointer = ^AcqDefTyp;


   LpGet = function (var Setting : AcqSettingTyp;  // Get Settings stored in the DLL
                     nDisplay : Cardinal) : LongInt; stdcall;

   LpStat = function (var Status  : AcqSTatusTyp;   // Get Status stored in the DLL
                     nDisplay : Cardinal) : LongInt; stdcall;

   LpRun  = procedure (nDisplay : LongInt;      // Executes command
		     Cmd	  : PChar) ; stdcall;

   LpCnt  = function (var cntp    : Double;       // Copies Cnt numbers to an array
                     nDisplay : Cardinal) : LongInt; stdcall;

   LpRoi  = function (var roip    : Cardinal;     // Copies the ROI boundaries to an array
                     nDisplay : Cardinal) : LongInt; stdcall;

   LpDat  = function (var datp    : LongInt;      // Copies the spectrum to an array
                     nDisplay : Cardinal) : LongInt; stdcall;

   LpStr  = function (var strp    : Char;         // Copies strings to an array
                    nDisplay : Cardinal) : LongInt; stdcall;

   LpServ = function (ClientWnd : Cardinal) : Cardinal; stdcall;  // Register Client MCDWIN.EXE

   LpNewStat = function (nDevice : Cardinal) : LongInt; stdcall;  // Request actual Status from Server

   LpGetMp3 = function (var Defmp3 : AcqMp3Typ) : LongInt; stdcall; // Get MPA3 Settings from DLL

   LpGetDatSet = function (var Defdat : DatSettingTyp) : LongInt; stdcall;
                                        // Get Data Format Definition from DLL

   LpDigInOut = function (value  :  Cardinal;           // controls Dig I/0, returns digin
                          enable :  Cardinal) : LongInt; stdcall;

   LpDacOut = function (value : Cardinal)  : LongInt; stdcall;
                                        // output Dac value as analogue voltage


var  Handle 	: Integer;
     TGet 	: LpGet;
     TStat 	: LpStat;
     TRun	: LpRun;
     TCnt	: LpCnt;
     TRoi       : LpRoi;
     TDat       : LpDat;
     TStr       : LpStr;
     TServ      : LpServ;
     TNewStat   : LpNewStat;
     TMp3       : LpGetMp3;
     TDatset    : LpGetDatSet;
     TDiginout  : LpDigInOut;
     Setting    : AcqSettingTyp;
     Mp3set     : AcqMp3Typ;
     {Data	: AcqDataTyp;
     Def        : AcqDefTyp;}
     Status     : AcqStatusTyp;
     Adc        : Cardinal;
     cmd        : String;
     Err        : LongInt;
     Spec       : Array[0..8191] of LongInt;

procedure PrintMpaStatus(var stat: AcqStatusTyp);
begin
  with stat do
  begin
    if val <> 0 then
      writeln('ON')
    else
      writeln('OFF');
    writeln('realtime= ', cnt[ST_REALTIME]);
    writeln('runtime=  ', cnt[ST_RUNTIME]);
    writeln('single_ev= ', cnt[ST_SINGLESUM]);
    writeln('coinc_ev= ', cnt[ST_COINCSUM]);
    writeln('sglrate=  ', cnt[ST_SGLRATE]);
    writeln('coirate=  ', cnt[ST_COIRATE]);
  end;
end;

procedure PrintStatus(var stat: AcqStatusTyp);
begin
  with stat do
  begin
    writeln('livetime=  ', cnt[ST_LIVETIME]);
    writeln('deadtime%= ', cnt[ST_DEADTIME]);
    writeln('totalsum=  ', cnt[ST_TOTALSUM]);
    writeln('roisum=    ', cnt[ST_ROISUM]);
    writeln('netsum=    ', cnt[ST_NETSUM]);
    writeln('totalrate= ', cnt[ST_TOTALRATE]);
  end;
end;

procedure PrintDatSetting(var datsett: DatSettingTyp);
begin
  with datsett do
  begin
    writeln('savedata= ', savedata);
    writeln('autoinc=  ', autoinc);
    writeln('fmt=      ', fmt);
    writeln('sepfmt=   ', sepfmt);
    writeln('sephead=  ', sephead);
    writeln('filename= ', String(filename));
  end;
end;

procedure PrintMp3Setting(var mpsett: AcqMp3Typ);
begin
  with mpsett do
  begin
    writeln('rtcuse=   ', rtcuse);
    writeln('dac=      ', dac);
    writeln('diguse=   ', diguse);
    writeln('digval=   ', digval);
    writeln('rtprena=  ', rtprena);
    writeln('rtpreset= ', rtpreset);
  end;
end;

procedure PrintSetting(var sett: AcqSettingTyp);
begin
  with sett do
  begin
    writeln('range=    ', range);
    writeln('prena=    ', prena);
    writeln('roimin=   ', roimin);
    writeln('roimax=   ', roimax);
    writeln('nregions= ', nregions);
    writeln('caluse=   ', caluse);
    writeln('calpoints=', calpoints);
    writeln('param=    ', param);
    writeln('offset=   ', offset);
    writeln('xdim=     ', xdim);
    writeln('timesh=   ', timesh);
    writeln('active=   ', active);
    writeln('roipreset=', roipreset);
    writeln('ltpreset= ', ltpreset);
    writeln('timeoffs= ', timeoffs);
    writeln('dwelltime=', dwelltime);
  end;
end;

procedure PrintDat(len: Cardinal);
  var i: Integer;
begin
  writeln('first 30 of ', len, ' datapoints:');
  for i:= 0 to 29 do
    writeln(Spec[i]);
end;

procedure help;
begin
  writeln('Commands:');
  writeln('Q     Quit');
  writeln('H     Help');
  writeln('S     Status');
  writeln('G     Settings');
  writeln('ADC=x Switch to ADC #x (0=MPA)');
  writeln('D     Get Data');
  writeln('... more see command language in MPANT Help');
end;

function run(command : String) : LongInt;
begin
  run := 0;
  if command = 'H' then
  help
  else if command = 'Q' then
  begin
    run := 1;
  end
  else if command = 'S' then
  begin
    if @TStat <> nil then
    begin
      Err := TStat(Status, Adc);
      PrintStatus(Status);
    end;
  end
  else if command = 'G' then
  begin
    if @TGet <> nil then
    begin
      if Adc > 0 then
      begin
        Err := TGet(Setting, Adc-1);
        PrintSetting(Setting);
      end
      else
      begin
        Err := TMp3(Mp3set);
        PrintMp3Setting(Mp3set);
      end;
    end;
  end
  else if AnsiPos('ADC=',command) = 1 then
  begin
     Val(String(PChar(command)+4), Adc, Err);
     TRun(0, PChar(command));
  end
  else if command = 'D' then
  begin
    if @TGet <> nil then
    begin
      if Adc > 0 then
      begin
        Err := TGet(Setting, Adc-1);
        if @TDat <> nil then
        begin
          Err := TDat(Spec[0], Adc-1);
          PrintDat(Setting.range);
        end;
      end;
    end;
  end
  else
  begin
    if @TRun <> nil then
    begin
      TRun(0, PChar(command));
      writeln(command);
    end;
  end;
end;

begin
  SetLength(cmd, 100);
  Adc := 0;
  Handle := LoadLibrary('dmpa3.dll');
  if Handle <> 0 then
  begin
    @TGet := GetProcAddress(Handle, 'GetSettingData');
    @TStat := GetProcAddress(Handle, 'GetStatusData');
    @TRun := GetProcAddress(Handle, 'RunCmd');
    @TCnt := GetProcAddress(Handle, 'LVGetCnt');
    @TRoi := GetProcAddress(Handle, 'LVGetRoi');
    @TDat := GetProcAddress(Handle, 'LVGetDat');
    @TStr := GetProcAddress(Handle, 'LVGetStr');
    @TServ := GetProcAddress(Handle, 'ServExec');
    @TNewStat := GetProcAddress(Handle, 'GetStatus');
    @TDatset := GetProcAddress(Handle, 'GetDatSetting');
    @TMp3 := GetProcAddress(Handle, 'GetMP3Setting');
    @TDiginout := GetProcAddress(Handle, 'DigInOut');

    if @TNewStat <> nil then
      Err := TNewStat(0);

 {   if @TStat <> nil then
    begin
      Err := TStat(Status, 0);
      PrintStatus(Status);
    end;

    if @TGet <> nil then
    begin
      Err := TGet(Setting, 0);
      PrintSetting(Setting);
    end; }
    help;

    repeat
      readln(cmd);
    until run(cmd) <> 0;

    FreeLibrary(Handle);
  end;
end.



