README file accompanying the MPA-3 External Control Software

MPA-3 Server Version 1.50 (May-16-2003) 
- New commands RROI(dspno,roino) and PROI(roiid) send the ROI sum of a selected
  ROI via LAN or serial connection. (External Control software required)

Remote control from any computer using RS232:
---------------------------------------------
Connect both computers with a Nullmodem cable on a COM port:
Pin 1  <--->  Pin 1
Pin 2  <--->  Pin 3
Pin 3  <--->  Pin 2
Pin 4  <--->  Pin 5
Pin 5  <--->  Pin 4
Pin 6  <--->  Pin 20
Pin 7  <--->  Pin 7
Pin 8  <--->  Pin 20
Pin 20 <--->  Pin 8

Commands can be sent using any terminal program, for example on any
Windows 9x / NT system the MTTTY.EXE from the TTY subdirectory on the
installation diskette.


Remote Control from any computer via TCP/IP net:
------------------------------------------------
This is possible using the Remote-shell daemon program mpa3net.exe.
It is necessary that you have TCP/IP network protocol installed.

MPA3NET.EXE:
This is a program which accepts commands via TCP/IP from a remote
machine and executes it on the local machine. It must be started on the
computer with the MPA-3 multiparameter system.

To check the program you need on the remote computer a remote shell program
like RSH which is usually installed in the system. An example is the
freeware WinRSH program by Denicomp, http://www.denicomp.com

The syntax for a mpant command is:
rsh machine_name command

Instead of machine_name also the IP address can be used.

Example:
to start press:
rsh machine_name start
to get the MPA status
rsh machine_name MPA?
to get the status of ADC1
rsh machine_name ADC1?


The syntax for a win command is:
rsh machine_name @command
Example:
to start mpa3 and mpant:
rsh machine_name @mpa3
to start the Windows accessory notepad and inspect the mpa3.ini file:
rsh machine_name @notepad mpa3.ini

Short installation guide:
=========================
You can test the software without a MPA-3 system. On one PC you 
should just install the MPA-3 software and mpa3net.exe in the MPA3 
folder. On the other PC you don't need anything to install, if you have 
Windows NT or 2000 on it. Both PC's must have a network card and TCP/IP 
installed. You should know the IP address of the PC with the MPA3 software 
installed (the local computer). To explore it, check the Local area 
connection properties and then the properties of the TCP/IP protocol. 
Let's assume the IP address is 192.168.123.194. Start the MPA3NET.EXE 
program in the MPA3 folder. On the other computer (the remote computer) 
start a DOS box and type in:

rsh 192.168.123.194 @mpa3

This should start the MPA3 software on the local computer.
Now try some commands like:

rsh 192.168.123.194 start
rsh 192.168.123.194 halt
rsh 192.168.123.194 ?
rsh 192.168.123.194 ADC1?
rsh 192.168.123.194 run test.ctl
rsh 192.168.123.194 exit

Windows 7:

In Windows Vista and 7 the RSH utility is not longer available.
It can be installed the following way:

- In the control panel in "Programs and features.. 
   Activate or deactivate Windows functions.."
  activate "Subsystem for UNIX-based Applications"

- Download the Utilities and SDK for UNIX-based Applications
  from Microsoft: 

http://www.microsoft.com/en-us/download/details.aspx?id=2391

- Install the service programs. Choose the Custom Installation. 
  You need only the „Basic Utilities“ and the „GNU Utilities“. 
  The expanded security options for „su“, please let all deactivated.
  After rebooting you can use rsh. Call it from the path
  %systemroot%\SUA\bin.

- The Windows Firewall blocks the rsh access. 
  For correct function the following TCP-Ports must be opened
  for incoming and outgoing traffic:

    514
    1022
    1023

