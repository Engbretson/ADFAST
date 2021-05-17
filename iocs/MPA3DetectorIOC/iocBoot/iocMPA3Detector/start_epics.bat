@echo off

z:\epics\base-7.0.3.1\bin\windows-x64-static\caget "29iddMPA:det1:AcquireTime" >nul  2>&1
echo Exit Code is %errorlevel% > nul 

if "%errorlevel%"=="0" goto :action1
if "%errorlevel%"=="1" goto :action2

goto :end

:action1
Echo "Do Action #1, The PV exists."
goto :end

:action2
Echo "Do Action #2, The PV does not exist. Start the softIOC."

rem start "c:\Program Files\EPICS Windows Tools\medm" -x -macro "P=29iddMPA:, R=det1:" MCPDetector.adl
z:
cd Z:\epics\synApps_6_1\support-v7\areaDetector-R3-9\ADFAST\iocs\MPA3DetectorIOC\iocBoot\iocMPA3Detector
..\..\bin\windows-x64-static\MPA3DetectorApp st.cmd 

pause

goto :end

:end





