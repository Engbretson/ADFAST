set EPICS_CA_ADDR_LIST=164.54.118.42
set EPICS_DISPLAY_PATH=Z:\epics\synApps_6_0\areaDetector-R3-4\ADFAST\FASTApp\op\adl;Z:\epics\synApps_6_0\areaDetector-R3-4\ADCore-R3-4\ADApp\op\adl

start medm -x -macro "P=29iddMPA:, R=det1:" MPA3Detector.adl

