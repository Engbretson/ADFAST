TOP=../
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

INC += c/DMPA3.h 
INC += c/STRUCT.h

# Note, it is assumed that the SDK dir is extracted in the photronSupport dir
ifeq (win32-x86, $(findstring win32-x86, $(T_A)))
LIB_INSTALLS_WIN32    += ../dll/32bit(x86)/dmpa3.lib
BIN_INSTALLS_WIN32    += ../dll/32bit(x86)/dmpa3.dll
#BIN_INSTALLS_WIN32    += C:/Windows/System32/vcomp120.dll

else ifeq (windows-x64, $(findstring windows-x64, $(T_A)))
LIB_INSTALLS_WIN32    += ../dll/64bit(x64)/dmpa3.lib
BIN_INSTALLS_WIN32    += ../dll/64bit(x64)/dmpa3.dll

endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

