TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#======== DETECTOR LIBRARY ==============

# The following gets rid of the -fno-implicit-templates flag on vxWorks, 
# so we get automatic template instantiation.
# This is what we want for miscellaneous/asynPortDriver.cpp
ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
CODE_CXXFLAGS=
endif

INC += MPA3Detector.h
INC += dmpa3.h

LIBRARY_IOC = MPA3Detector
LIB_SRCS += MPA3Detector.cpp

DBD += MPA3DetectorSupport.dbd

  # Optional modules
    
 ifdef MODBUS
	$(DBD_NAME)_DBD  += drvAsynIPPort.dbd
	$(DBD_NAME)_DBD  += drvAsynSerialPort.dbd
    $(DBD_NAME)_DBD  += modbusSupport.dbd
    $(DBD_NAME)_DBD += modbus.dbd
    PROD_LIBS        += modbus
  endif


include $(ADCORE)/ADApp/commonLibraryMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

