
set EPICS_DISPLAY_PATH=Z:\epics\synApps_6_1\support-v7\areaDetector-R3-9\adcore-R3-9\ADApp\op\adl;Z:\epics\synApps_6_1\support-v7\areaDetector-R3-9\ADFAST\FASTApp\op\adl
set EPICS_CA_ADDR_LIST=164.54.104.10

call medm -x -macro "P=29iddMPA:, R=cam1" MPA3Detector.adl