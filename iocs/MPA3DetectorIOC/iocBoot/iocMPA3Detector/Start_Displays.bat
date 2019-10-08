 set EPICS_CA_ADDR_LIST=164.54.118.42
set EPICS_DISPLAY_PATH=Z:\epics\synApps_6_1\support-v7\areaDetector-R3-7\ADFAST\FASTApp\op\adl;Z:\epics\synApps_6_1\support-v7\areaDetector-R3-7\ADCore-R3-7\ADApp\op\adl

start medm -x -macro "P=29iddMPA:, R=det1:" MPA3Detector.adl

