@ECHO off
if "%1" == "G" copy .\obj\libUsbGlobal.LIB ..\..\..\lib\libUsbGlobal.LIB
if "%1" == "A" copy .\obj\libUsbA.LIB ..\..\..\lib\libUsbA.LIB
if "%1" == "B" copy .\obj\libUsbB.LIB ..\..\..\lib\libUsbB.LIB
if "%1" == "C" copy .\obj\libUsbC.LIB ..\..\..\lib\libUsbC.LIB
if "%1" == "D" copy .\obj\libUsbD.LIB ..\..\..\lib\libUsbD.LIB