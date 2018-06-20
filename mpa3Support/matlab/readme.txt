Using the DMPA3.DLL from Matlab
================================

It does not work, to load the DMPA3.DLL in Matlab via loadlibrary
and direct call DLL functions. You have to make mex files as
an intermediate layer.
 
An example is the supplied mxruncmd.c. It can be used to send
commands to the server via the RunCmd DLL function. Try the example:
0. Copy the files dmpa3.h, struct.h, mxruncmd.c, mxruncmd.mexw32
   into the default MPA-3 working directory, usually C:\MPA3
1. Start the MPA3 Server (and MPANT) program.
2. Start Matlab
3. Set the current directory in Matlab to C:\MPA3 via the 
   Current folder bar at the top.
4. Enter the command
   mxruncmd('run test.ctl')

You can also define string commands and use them, for example

s = 'erase'
mxruncmd(s)

To recompile the mex file mxruncmd.mexw32 from mxruncmd.c,
setup the compiler with command

mex -setup

and compile by

mex mxruncmd.c

