 set EPICS_CA_ADDR_LIST=164.54.118.42
set CAQTDM_DISPLAY_PATH=Z:\epics\synApps_6_1\support-v7\areaDetector-R3-9\ADFAST\FASTApp\op\ui\autoconvert;Z:\epics\synApps_6_1\support-v7\areaDetector-R3-9\ADCore-R3-9\ADApp\op\ui;Z:\epics\synApps_6_1\support-v7\areaDetector-R3-9\ADCore-R3-9\ADApp\op\ui\autoconvert

rem start caqtdm -x -macro "P=29iddMPA:, R=det1:" MPA3Detector.ui ad_cam_image.ui
start caqtdm -x -macro "P=29iddMPA:, R=det1:"  ad_cam_image.ui
