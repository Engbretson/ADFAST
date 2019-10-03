#modbus.cmd

# Use the following commands for TCP/IP
#drvAsynIPPortConfigure(const char *portName, 
#                       const char *hostInfo,
#                       unsigned int priority, 
#                       int noAutoConnect,
#                       int noProcessEos);

drvAsynIPPortConfigure("DL06","164.54.118.35:502",0,1,1)
#drvAsynIPPortConfigure("DL06","164.54.53.200:502",0,1,1)

#modbusInterposeConfig(const char *portName, 
#                      modbusLinkType linkType,
#                      int timeoutMsec,
#                      int writeDelayMsec)
modbusInterposeConfig("DL06",0,5000,10)

# I want no messages from this sucker
asynSetTraceMask("DL06", 0, 0)

#       numbers (no leading zero) or hex numbers can also be used.

# The 440 has bit access to the Xn inputs at Modbus offset 4000 (octal)
# Read bits (X0-X23).  Function code=2.
drvModbusAsynConfigure("X_Bits",      "DL06", 0, 2,  04000, 024,    0,  100, "Koyo")

# The 440 has bit access to the Yn outputs at Modbus offset 4000 (octal)
# Read bits (Y0-Y17).  Function code=1.
drvModbusAsynConfigure("Y_Bits",   "DL06", 0, 1,  04000, 020,    0,  100, "Koyo")

# The 440 has bit access to the Cn bits at Modbus offset 6000 (octal)
# Access 1024 bits (C0-C1777) as inputs.  Function code=1.
drvModbusAsynConfigure("C_Bits",   "DL06", 0, 1,  06000, 02000,   0,  100, "Koyo")

# The 440 has bit access to the Cn bits at Modbus offset 6000 (octal)
# Access 1024 bits (C0-C1777) as outputs.  Function code=5.
drvModbusAsynConfigure("C_Bits_Out",   "DL06", 0, 5,  06000, 02000,   0,  1, "Koyo")

# various blocks of V locations, function code=3.

drvModbusAsynConfigure("V400", "DL06", 0, 3, 0400, 020,    0, 100,  "Koyo")

drvModbusAsynConfigure("V500", "DL06", 0, 3, 0500, 012,    2, 100,  "Koyo")

drvModbusAsynConfigure("V514", "DL06", 0, 3, 0514, 010,    7, 100,  "Koyo")

# Access 1024 bits (SP0-SP777) as inputs.  Function code=2.
# used for hearbeats and to confirm that the plc is actually running.
drvModbusAsynConfigure("SP_Bits",   "DL06", 0, 2,  06000, 01000,   0,  100, "Koyo")

dbLoadTemplate("modbus.substitutions")


