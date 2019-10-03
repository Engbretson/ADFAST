< envPaths
errlogInit(20000)

epicsEnvSet("IOC","ioc29iddMPA")
epicsEnvSet("engineer","Engbretson")
epicsEnvSet("location","ID-29-D")
epicsEnvSet("group","ID-29-D")

dbLoadDatabase("$(TOP)/dbd/MPA3DetectorApp.dbd")
MPA3DetectorApp_registerRecordDeviceDriver(pdbbase) 

# Prefix for all records
epicsEnvSet("PREFIX", "29iddMPA:")
# The port name for the detector
epicsEnvSet("PORT",   "MCP1")
# The queue size for all plugins
epicsEnvSet("QSIZE",  "20")
# The maximim image width; used for row profiles in the NDPluginStats plugin
epicsEnvSet("XSIZE",  "8")
# The maximim image height; used for column profiles in the NDPluginStats plugin
epicsEnvSet("YSIZE",  "2000")
# The maximum number of time series points in the NDPluginStats plugin
epicsEnvSet("NCHANS", "2048")
# The maximum number of time series points in the NDPluginTimeSeries plugin
epicsEnvSet("TSPOINTS", "2048")
# The maximum number of frames buffered in the NDPluginCircularBuff plugin
epicsEnvSet("CBUFFS", "500")
# The search path for database files

epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db")

epicsEnvSet("T1", "Sin(x)")
epicsEnvSet("T2", "Cos(x)")
epicsEnvSet("T3", "SquareWave(x)")
epicsEnvSet("T4", "Sawtooth(x)")
epicsEnvSet("T5", "Noise")
epicsEnvSet("T6", "Sin(x)+Cos(x)")
epicsEnvSet("T7", "Sin(x)*Cos(x)")
epicsEnvSet("T8", "SinSums")

asynSetMinTimerPeriod(0.001)

# The EPICS environment variable EPICS_CA_MAX_ARRAY_BYTES needs to be set to a value at least as large
# as the largest image that the standard arrays plugin will send.
# That vlaue is $(XSIZE) * $(YSIZE) * sizeof(FTVL data type) for the FTVL used when loading the NDStdArrays.template file.
# The variable can be set in the environment before running the IOC or it can be set here.
# It is often convenient to set it in the environment outside the IOC to the largest array any client 
# or server will need.  For example 10000000 (ten million) bytes may be enough.
# If it is set here then remember to also set it outside the IOC for any CA clients that need to access the waveform record.  
# Do not set EPICS_CA_MAX_ARRAY_BYTES to a value much larger than that required, because EPICS Channel Access actually
# allocates arrays of this size every time it needs a buffer larger than 16K.
# Uncomment the following line to set it in the IOC.
#epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "10000000")

# Create an ADCimDetector driver
# MPA3DetectorConfig(const char *portName, int numTimePoints, int dataType,
#                      int maxBuffers, int maxMemory, int priority, int stackSize)
MPA3DetectorConfig("$(PORT)", $(YSIZE), 7, 0, 0)
dbLoadRecords("$(ADFAST)/db/MPA3Detector.template",  "P=$(PREFIX),R=det1:,  PORT=$(PORT),ADDR=0,TIMEOUT=1")


# Create a standard arrays plugin, set it to get data from ADCSDetector driver.
NDStdArraysConfigure("Image1", 3, 0, "$(PORT)", 0)
# This creates a waveform large enough for 100000x8 arrays.
dbLoadRecords("NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=800000")

## Load all other plugins using commonPlugins.cmd
< $(ADCORE)/iocBoot/commonPlugins.cmd
set_requestfile_path("$(ADFAST)/FASTApp/Db")

#asynSetTraceIOMask("$(PORT)",0,2)
#asynSetTraceMask("$(PORT)",0,255)

< modbus.cmd

iocInit()

# save things every thirty seconds
create_monitor_set("auto_settings.req", 30, "P=$(PREFIX)")
