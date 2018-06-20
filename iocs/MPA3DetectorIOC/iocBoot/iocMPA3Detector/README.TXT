README.TXT file of MPA3 software news
Please look also at the actual MPA-3 FAQ page on our web-site
www.fastcomtec.com

This is 32-bit Software for Windows XP/7/8/10
For Windows 8/10 please install a Start Menu from classicshell.net

MPA-3 server version 2.11 (Jan-18-2016):
- For dualparameter and calculated spectra now a livetime is 
  calculated and shown in the 11th comment line for the 
  respective spectrum. The livetime counter is incremented if 
  all ADC's that contribute to the calculated spectrum are not
  busy in the ms timer event.

MPA-3 server version 2.1 (Nov-20-2014):
- A log file containing all counters like real und live time,
  totalsum, ROI sum etc. can be written. Configure with the 
  "Log.." button from the data operations dialog.

MPANT v 2.1 (Jan-13-2014):
- New Display mode: Beside max, min and mean pixel now also all
  data points can be displayed if the pixel size is greater 
  than the data resolution.

MPANT v 2.0 (Sep-19-2013):
- Bug fixes.

MPANT v 1.98 (Mar-19-2013):
- Bugfix, display of projections of ROIs.

MPANT v 1.96 (Jun-26-2012):
- Small Bugfix, grid not on axes, scale updating.

MPANT v 1.95 (May-08-2012):
- Bugfix, TotalSum in zoomed 2D spectra.

MPA-3 server version 1.94 (Mar-27-2012):
- New drivers for Windows XP and Windows 7, including 64 bit. The 
  drivers are now digitally signed.
- It is not longer necessary to care about blocksize and dma timeout.

MPA-3 Server version 1.93 (Oct-31-2011):
- Combining of conditions works now also for OR of not coincident
  ADCs and up to any nesting depth.
- You can copy an ROI from a ORSUM spectra (see v 1.87) to all spectra
  generating the ORSUM, just make that ROI "active" in the ROI editing
  dialog. The different calibrations are taken into account!

MPA-3 Server version 1.92 (Jul-14-2011):
- Bugfix, writing of .als listfiles.

MPANT v 1.90 (Feb-18-2011):
- Bugfix, printing of 2D spectra.

MPA-3 Server version 1.89 (Dec-21-2010):
- The Replay software can now read *.als ASCII List files. Such files
  can be written with the Replay software and may be modified by 
  customer software (see below version 1.17).

MPA-3 Server version 1.87 (Jul-21-2010):
- It is now possible to create spectra containing the sum of counts
  from several ADC's. It can be used to create a "superdetector"
  from multiple smaller detectors. It doesn't matter if the ADC's
  are used in single or coincidence mode, the sum spectra is 
  calculated from the complete single spectra. 
  Choose Spectra... from the MPANT options menu or the corresponding
  toolbar icon, click "Add Calc..." and select the 
  "Or = Sum of counts" radio button. In the "Sum of counts" dialog
  you can select which ADCs will contribute to the superdetector.
  It is recommended to enable "Use Calibration" and to perform 
  a careful calibration of all selected ADC's:
  Make a short acquisition using a pulser or a calibration source
  so you get a peak in each used ADC. In each ADC zoom into the 
  spectra, then by keeping the right mouse button pressed drag from
  left to right over the peak to mark a fit region and click on the
  "Fit" toolbar icon to make a Gaussian peak fit. Note the position 
  value of the peak in the first ADC. For the first ADC you can just 
  enable "Use Calibration" in the calibration dialog and use the 
  standard parameters p0=0, p1=1. In the next ADC enter two 
  calibration points: for the first point enter 0 for the Channel 
  and 0 for the value and click "Add>>". For the second calibration 
  point use the result of the fit: Click "Fit" to enter the Channel 
  value of the fitted peak position. For the "Value" enter the fit 
  result of the peak position in the first ADC. Click "Calibrate" 
  and enable "Use Calibration", then click "OK". Proceed this way 
  until all used ADCs are calibrated. You will immediately get the 
  effect: Instead of a peak multiplet you will see a single sharp 
  peak in the sum spectra.
  The automatic generated spectra name is for example "OR_3_C".
  The 3 is here an hexadecimal bit pattern indicating that ADC1 
  (bit 0) and ADC2 (bit 1) contribute to the OR sum. The C at the
  end of the name means that "Use Calibration" is enabled.

MPA-3 Server Version 1.84 (Apr-01-2010):
- A new mode "Time Differences" is possible for time spectra
  using the RTC or 1 msec realtimer. So you can now record an absolute
  time for each event using "Software only" as "Preset Load Mode" 
  in the RTC dialog. The "Preset" must be longer than the time 
  of your acquisition. Using "Time Differences" in the definition 
  dialog for time spectra you can then record time spectra relative 
  to a reference to an event in any selected ADC, also in replay mode.
  The time of the next event in the reference ADC after the end of 
  the time spectra is used as a new reference time.

Version 1.83 (Feb-22-2010):
- Spectra can now be read and recorded in EMSA format.

MPANT and MPA-3 Server Version 1.81 (Oct-16-2009):
- Small bugfix concerning conditions on ROIs in RTC timespectra 

MPANT and MPA-3 Server Version 1.78 (Mar-09-2009):
- The middle mouse button or a mouse wheel now can be used for quickly 
  zooming into a spectra.
- The MPANT main Window starts maximized if the corresponding new checkbox
  in the axis dialog is checked and the configuration saved.
- New calculated spectra ADCx - ADCy, suited for position dependent
  detectors with delay line readout.
MPA-3 Server v 1.75 (Apr-15-2008):
- DigI/O bit 0 now can be used for the Status as described in the manual.
  Up to now it was actually bit 7.
MPA-3 Server v 1.74 (Feb-29-2008):
- Messageboxes after "continue" can be suppressed by a line 
  nomessages=1 in mpa3.ini

Version 1.71 (Feb-27-2007):
- ROI names are now implemented. The name can be entered in the ROI
  editing dialog. Press "Modify" to insert a new name from the edit
  field of the selected ROI into the list. The name is saved
  with the ROI data in the corresponding line after a semicolon.
  Conditions can be set on named ROIs. When reading named
  ROIs from a file or command, existing ROIs with the same name
  will be overwritten.
- New commands: 
  delnamedroi roiname	; deletes named ROI
  selnamedroi roiname	; selects named ROI in MPANT spectra displays
- New functions in DMPA3.DLL:
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
- A new LabVIEW VI library is included in the optional DLL software 
  using the new functions.

Version 1.70 (Feb-07-2007):
- expansions to create spectra via single commands of the command language:
  as an example, create a dualparameter spectra by sending the following
  commands:
  [MAP0] 1A x 1B
  range=65536
  param=10000
  xdim=256
  active=3
  newspec

  the "newspec" is a new command that finalizes the spectra generation, 
  the other parameters are known from header lines in a .mpa file:
  [MAP0] 1A x 1B  ; this defines a new map spectra 
		  ; with index adcnum+0 and title "1A x 1B"
  range=65536     ; spectra length, xdim x ydim
  param=10000     ; in hex, low word means index of first parameter,
                  ; i.e. 0 for ADC 1A, high word is index of 2nd
		  ; parameter, i.e. 1 for ADC 1B
  xdim=256        ; the x dimension
  active=3        ; type of spectra, see settings documentation
 		  ; in DLL manual.

Version 1.69 (Oct-27-2006):
- DMA timeout in mpa3.ini now in units of msec

Version 1.67 (Feb-01-2006):
- New commands:
  deleteallrois deletes all ROIs in the active MPANT window.
  deleteallrois SP_# allows to delete all ROIs of a specified 
  spectra (1 for # means ADC1A..).

Version 1.66 (Nov-11-2005):
- Using slice region shape with calibration, dx and dy differences 
  are displayed and can be measured.

Version 1.65 (Oct-20-2005):
- New item "Fit ROIs Output.." in the Options menu.
- Enhanced output of "Fit ROIs" and expanded Fit ROIs Output options.
- Improved ROI info in display windows, start time is shown.
- New command OPENALL opens all displays in MPANT.

Version 1.61 (Apr-22-2005):
- The maximum number of iterations for Gaussian peak fits is 40 by default, 
  but this may be not enough if the FWHM of the peaks is larger than 
  about 10 bins. The default value now can be changed by editing the 
  MPANT.CFG file, the value here is now 
  /maxiter=100
- The DLL function dialog allows now to enter also the name of the DLL.
  So it is not longer necessary to modify the DMPA3.DLL for own DLL
  functions and any compiler can be used for writing own DLLs. How to 
  use it, is documented in the optional available DLL software 
  interface. If the name is left empy or equals DMPA3.DLL, still 
  the DMPA3.DLL is used.

Version 1.59 (Jan-07-2005):
- A new command CLOSEDSP SP_# allows to close all views of a specified 
  spectra (1 for # means ADC1A..) in MPANT. May be used to free computer
  resources by avoiding the permanent display refresh of large MAPS. The 
  spectra display can be reopened from the MPANT File menu with "Open All".

Version 1.58 (Dec-02-2004):
- Dialogs from the Server are now in pushed into the foreground.

Version 1.57 (Nov-25-2004):
- Graphic input of ROIs via right mouse button is improved, it can now also
  be finished with a double click and not only with the End key.
- The file dmpa3.dll is now installed into the Windows\System32 directory.

Version 1.56 (Nov-09-2004):
- Auto calibration is now possible with only one peak, (0,0) is
  then used as a second calibration point.
- When deleting ROIs corresponding peak values are also deleted. 

Version 1.55 (Sep-21-2004)
- New commands pushname and popname allow to save and recall the mpa filename 
  in a script.

Version 1.54 (Jun-30-2004)
- Projections and slices now work in any 2D-display, also if transformed from
  a single spectra by specifying an x-dimension in the display options.

Version 1.53 (Jan-28-2004)
- A new radio button in the calculated spectra dialog allows to define a history
  plot of an ADC as a new spectra. The length of this spectra can be defined free.

Version 1.52 (Oct-16-2003)
- Filename in fit-results logfile now .mpa and .mp.
- Small bugfix concerning scaling of isometric display 
- Drop 'zero events' now suppresses completely histogramming into channel zero.

Version 1.51 (Jun-23-2003)
- Bugfix concerning coincidences with AUX input and spectra with conditions

Version 1.50 (May-16-2003) 
- New commands RROI(dspno,roino) and PROI(roiid) send the ROI sum of a selected
  ROI via LAN or serial connection. (External Control software required)

Version 1.48 (Nov-08-2002)
- Improvements for prompt starting and stopping via Dig 6 input checking.
- A new checkbox in the Listmode box of the data operations dialog named 
  [Drop 'zero events'] allows to drop coincidence events with all ADC data 
  being zero. Such events may arise strongly if the input signals to the ADC 
  have some noise as such noise signals can make a Deadtime signal which starts 
  a coincidence window also if the signal is too low for a valid conversion.
  Such events will then also be eliminated before writing it into a listfile.

Version 1.46 (Aug-29-2002)
- It is now easier to browse and load MPA-3 Configuration files *.CNF.

Version 1.44 (Feb-14-2002)
- For external controlled start and stop using Dig 6 input checking a new 
  checkbox "clear" was introduced in the Dig, DAC dialog to allow clearing the
  spectra before a controlled restart. It is recommended to disable DMA mode 
  by setting nodma=1 in the MPA3.INI file and running the MPA3.EXE in normal 
  priority (start directly, not using the Launchmpa program) when using this
  Dig 6 input polling.

Version 1.42 (Sep-13-2001)
- Calculated spectra like slices and ROI projections now can be seperately saved
  and reloaded.

Version 1.40
- The font inside the display windows can be selected in the axis dialog 
  between Small and Large.
- For the selected ROI now also the mean counts per channel and the centroid 
  is displayed. 

Version 1.39 (May-10-2001):
- The Display Options dialog for single spectra has a new selection box "Symbol" 
  that allows to select various symbols for the data points. They can be filled 
  and error bars can be displayed using the respective checkboxes. 

  Expansions for differential measurements using a tagbit:
- Look at the example control files spin.ctl and flip.ctl. Two new commands were
  introduced:
  if(even)command
  if(odd)command
  executes a command if the current run number is even or odd, respectively. 
  Edit the spin.ctl to change the number of runs and duration of a short 
  acquisition. After each short acquisition the AUX2 output is flipped 
  and the measurement continued. Connect the AUX2 output with your experimental 
  setup and a high bit (for example bit 14) of the ADC interface that you want 
  to observe. If bit 14 is used set the ADC range to 32768 also if your actual
  ADC conversion range is lower than 16k.
- A special DLL named FMPA3.DLL allows to communicate with MPANT for customer-
  calculated spectra. This DLL including sourcecode is now part of the DLL 
  software interface that is available as an option.
  Installation: Copy the FMPA3.DLL into the working directory (usually C:\MPA3).
  Use: This DLL allows to calculate spectra and to display it with the MPANT 
     program. Select a single spectra and open the Display options dialog. 
     Press then the button labeled "Custom formula...". A dialog box "Custom-
     transformed spectra" is opened. Here the formula for the calculation can 
     be selected and the parameters can be edited. A new display window showing 
     the calculated spectra can be created. Error Bars can be activated in the 
     display options dialog, for the calculation of the error bars also the DLL 
     is used. 
  Example: The supplied DLL is for acquisitions marked by a tag bit in a high 
     bit of the ADC interface. It allows to calculate the Sum=x+x', 
     Difference=x-x' and relative difference Delta=F*(x-x')/(x+x') of spectra 
     marked by the tag bit.
- Improved Printing Dialog allows to change the font size and to print comment
  lines.

  (May-23-2001):
- Start the software using a shortcut to LaunchMP.EXE to run the mpa3 server
  with a high priority. This reduces the loss of events due to a full FIFO when 
  the computer is busy with other applications.

Version 1.36 (Mar-09-2001):
- Projections of ROIs in dualparameter spectra to the x- and y-axes are now 
  implemented. There are new menu items "X-Projection" and "Y-Projection"
  in the Region menu. They create new single spectra displays showing the 
  respective projection of the selected ROI in a dualparameter spectra in the
  active window.
- It has been observed that the DMA data transfer does not work properly on 
  some computer systems. Therefore now at the start of the software a test is 
  performed to see whether DMA mode works properly. If the test fails, you 
  are prompted to insert a line "nodma=1" into the mpa3.ini file. The test will 
  then not longer made in the future and DMA will be disabled. The maximum 
  throughput will be less by approximately a factor of 3. It is possible that 
  the test is sometimes passed though DMA does not work properly. If it fails 
  the first time, please change your mpa3.ini file to insert the line nodma=1.
  If the test is always successfull, you can insert a line "nodma=-1" to allow 
  DMA mode and bypass the test at every program start.

Version 1.32 (Sep-26-2000):
- The Calculated Spectrum Setting dialog was expanded by a new option named
  "DLL function" and a button named "Edit". By selecting the "DLL function"
  and pressing this button a dialog "Dll Function" is opened. The calculated
  spectrum is a function programmed in the DMPA3.DLL fDLL(Xpar, Ypar, Zpar).
  Run the TSTRXY.EXE program to create a sample table file and try the
  dlltest.cnf configuration: For a 2D position dependent energy spectra
  correction now as a sample a function named
          unsigned long RTab(long x, long y, long z)
  is implemented in the DLL. It is calculated by looking up in a table as
  z * Tab[x,y] and must be initialised by using the DLL function named
  int IniTab(LPSTR filename);
  The Cleanup function CloseTab() frees the memory allocated by IniTab.
  A sample table can be calculated by using a program tstrxy.exe. It assumes
  a Gaussion function for the position dependent energy calibration of a
  2D position dependent detector.
  The names of these DLL functions and the filename containing the table
  must be provided in the DLL Function dialog, and also the dimensions
  XRange and YRange of the table. The Xpar and Ypar parameters are shifted
  to fit their spectra range into the table dimensions.
  In the optional available DLL software interface for LabVIEW and Visual
  Basic the complete sources of the DMPA3.DLL and TSTRXY.EXE are included
  to allow own written applications for calculated spectra based on this
  example (the customer dependent functions of the DLL are in a small module
  "custom.c").

Version 1.3 (May-08-2000):
- The Slice option allows to create new single spectra displays, showing a 
  slice in a dualparameter spectra. Click on a dualparameter spectra display 
  to make it the active window, then select the Slice menu item or toolbar 
  icon. The Slice dialog box is displayed. Select 'x=const' or 'y=const' 
  for the slice direction, and the coordinate. Clicking on the "create" 
  button creates the new display window. In the title bar of the new window 
  the name of the dualparameter spectra and the slice coordinate is shown. 
  The slice position can be changed using the scroll bar in the Slice dialog, 
  or by entering the value in the edit fileld and pressing the button which 
  is labeled 'Set' after creation of the slice view. The Slice dialog can be 
  closed by clicking its close field. Created slice spectra displays remain 
  visible and their coordinates can be changed later using the Slice utility 
  again. The position of the Slice dialog with respect to the MPANT main 
  window can be saved in the MPANT.CFG file. Rectangular ROIs are visible in 
  the slice spectra display and can be created here.
- A new ASCII format with extension .CSV is available. After a small header
  ending with a line [DATA] follow the data, in each line channel number(s)
  and spectra value, seperated with tabs. For dualparameter spectra it means
  (x y value), for singleparameter spectra (x value). Channels containing a
  zero value are omitted for dualparameter spectra to save space. The
  header is not read and only introduced for compatibility with the MPAWIN
  software; actually the CSV format is the MPAWIN ASC format. To read a CSV
  file with the MPAWIN software, just rename the extension into ASC.

Version 1.21 (Dec-27-1999):
- ROI conditions can now be defined in a dialog entered via a new button in
  the spectra definition dialog. Slice ROIs in single spectra are available
  as well as rectangular, polygonal, circular, annular and pie ROIs in
  dualparameter and time spectra. Also combined conditions via NOT, OR and
  AND operations are possible up to two levels. For any dualparameter and
  calculated spectra the conditions can be set in its definition dialog as
  a necessary acquisition condition. The calculated spectra dialog has a
  new COPY type of spectra. It allows to set conditions for copies of
  primary ADC spectra.
- It is now possible to reduce the size of list files by writing reduced timer
  data. When "Write Listfile" is enabled in the data operations dialog, a
  checkbox "Reduce timer data" will be visible and you have a choice between
  a factor of 10, 100 or 1000. Of course the resolution of the real- and
  live time will be reduced by this factor when evaluating the listfile.

Version 1.20 (Dec-03-1999):
- The software now can be used also under Windows 98 and Windows 2000.
  After installation of the PCI board in the computer and starting the system
  you will be asked to provide a driver for the new found hardware. Enter
  A:\wdmdriv
  The DMA timeout entered in the mpa3.ini file in the line timeout=.. now
  for the new WDM driver under Win 98/2k can be smaller than 2 seconds,
  for example 0.2 (minimum value is 0.005)

Version 1.17 (Oct-11-1999):
- The Replay dialog has an additional Checkbox "Write ASCII (.als)". It allows
  to convert the binary listfile into an ASCII file. It has the usual header
  and then the listfile data in the following ASCII format
  (1 line per event tag):
C 0   268    ; Coincidence ADC data in ADC 0 at channel 268 (decimal)
S 1   534    ; Single ADC data in ADC 1 at channel 534 (decimal)
T fff8       ; Timer (msec) event, ADC0..2 are "dead", the others "alive"
             ; (hex fff8)
ES a         ; Single event flag, data in ADC1 and ADC3 (hex a)
EC 5         ; Coincidence event flag, data in ADC0 and ADC2 (hex 5)
RTC   576     0       0   ; RTC data rtc0, rtc1, rtc2 (decimal)

Version 1.15 (Jun-08-1999):
- Polygonal and curved ROI's in MAP displays are now implemented in addition
  to the rectangular ROIs. The boundaries can be polygonal, circular, annular
  or pie. Make a choice of the shape using the Menu Region...Shape or the
  corresponding toolbar icon. Define the Roi using the right mouse button,
  the TAB and the END key (please read the help in the status bar) and save
  the ROI using the toolbar icon "Create new ROI". Advance the selected ROI
  to the next or previous one using the "+" or "-" key from the numeric keypad.
  The ROI boundary is displayed by lines or circles and the ROI area can in
  addition be displayed in inverted color by enabling the checkbox
  "Invert ROI Pixel" in the MAP Display options dialog. The sum and net sum of
  counts within the selected ROI is displayed. The net sum is calculated by
  subtracting a background integral by summing line integrals S(y) for each
  y coordinate using the mean value of the counts at the lowest and highest
  x coordinate of the ROI.

Version 1.11 (Mar-30-1999):
- Now all presets (realtime, livetime, roi) are hold exactly, events after
  a preset reached are dropped.


Version 1.10 (Mar-10-1999):
- New driver fastmpa.sys. After pressing the STOP button it takes now only
  1..2 sec until the OFF status is displayed.

Version 1.09 (Feb-24-1999):
- The MS-12 12-fold 100 MHz Scaler card is now supported in the software.
  Up to 4 MS-12 cards can be used simultaneously. By connecting the GO-line
  between PCI-card and MS-12, Start and Stop can be synchronized by hardware.

Version 1.08 (Jan-20-1999):
- A log file can now be written. The log file options can be edited in
  a dialog that can be entered from the data options dialog. A set of new
  control commands supports opening, closing and writing text into log
  files. The commands are described in the on-line help.

Version 1.06 (Dec-29-1998):
- New variables watchpriority and evalpriority in the MPA3.INI file allow
  adjusting the thread priorities. watchpriority=1 can improve the
  performance (on NT 4.0 with servicepack 3). Set both priorities to zero
  if you observe that your system hangs after start.

Version 1.05 (Dec-11-1998):
- Stabilizing works now also in Replay mode. Set in the Replay dialog a
  limited time range and you can then use the same control files stabi.ctl,
  stab.ctl and stabf.ctl as for the stabilization during the acquisition.
  Try also the rstabi.ctl, rstab.ctl and rstabf.ctl, which are similar but
  faster as the not stabilized sum is not made.

Version 1.04 (Dec-04-1998):
- Calibration is now implemented, also for two dimensional MAPs. The
  calibration can be defined in a single parameter spectrum and is then
  available also in all dual parameter spectra using this parameter.

- Stabilizer: Using the autocal function together with shifted summing of
  spectra according to a calibration it is possible to make an automatic
  spectrum stabilization. The spectrum should contain at least two well
  distinct peaks. Define ROIs around the peaks and enter calibration values
  for the peaks in the Roi..Edit dialog. To do it, enter the peak energy
  for the selected peak in the list and press "Add" to enter the energy 
  in the list. It works also with only one peak, then as a second point 
  (0,0) is taken for the calibration.
  Try the Autocal operation in the Roi menu. Look at the control file examples 
  stabi.ctl, stab.ctl and stabf.ctl. It is assumed that two ADCs are used. 
  Test it with ADCs immediately after power on. Set a realtime preset of, 
  say 60 sec and start the measurement by loading stabi.ctl. A number of 
  acquisitions is done as defined in stabi.ctl and stabilized sum spectra is 
  accumulated in file sum.mpa simultaneously with not stabilized spectra 
  in sum1.mpa. Also two dimensional MAPs of ADC1 and ADC2 (define 
  zoomed MAPs around the two peaks) are stabilized.

Description of the listfile format:
===================================

  The listfile starts with an ASCII header containing the
  settings as described in the manual. The header ends with a line
  containing [LISTDATA]. Then the listdata follow in binary format. The
  structure is as follows:

  Every millisec there is a timer event. It is a 32 bit word containing
  0x4000 as high word and as low word for ADC 1 a 1 in bit 0 if it's
  "alive" and a 0 if its "dead", the same for ADC2 in bit 1 and so on.
  Counting the corresponding set bits directly gives the live time in
  milliseconds for each ADC, counting the timer words gives the
  realtime of the run in milliseconds. It is possible that the timer is 
  written only every 10, 100 or 1000 milliseconds. This is then indicated 
  by a line "timerreduce=10" (100, 1000, respectively) immediately 
  preceding the [LISTDATA] tag in the header of the listfile.

  After a timer event can follow either again a timer event or a
  synchron mark, i.e. a double word containing 0xFFFFFFFF and then
  event data.

  Event data start always with a event signal double word containing in
  the high word some flags and a zero in bit 30 (to distinguish it
  from a timer event), and in the low word for each ADC that has data a
  1 in the corresponding bit (bit 0 for ADC1 ...). The ADC data are 16
  bit and the data structure is in a 32 bit raster, therefore there
  must be a 16 bit dummy word inserted for an odd number of ADCs
  containing data to get an even number of 16 bit data words. If such a
  dummy was inserted, this is marked in bit 31 of the event signal
  word.

  RTC Option: If data of the 48 bit realtime clock (RTC) are in the event,
  this is marked in bit 28 of the event signal word. If RTC data are
  signaled in the event signal double word, the three 16 bit RTC data
  rtc0, rtc1 and rtc2 follow immediately after the signal word in the
  next low, high and low words. The rtc value is
  (rtc2 * 65536  + rtc1) * 65536 + rtc0. It starts from a (preset)
  value that can be set by software and counts down with 20 MHz (or
  extern clock). After rtc2 follows in a high word the dummy word if
  bit 31 in the event signal double word was set, or ADC data.

  If RTC data were not signaled, immediately after the signal dword
  follows in the next low word either a dummy word (if signaled in bit
  31) or ADC data (starting from the lowest ADC that has data). From
  the signal dword it is clear how many ADC data follow and also that
  it is an even number of 16 bit words, so after this set of data can
  either again follow a new set  starting with a new event signal
  dword, or a new livetime event.

