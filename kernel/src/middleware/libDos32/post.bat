@ECHO off
if "%1" == "A" copy .\obj\libDos32_A.LIB ..\..\..\lib\libDos32_A.LIB
if "%1" == "B" copy .\obj\libDos32_B.LIB ..\..\..\lib\libDos32_B.LIB
if "%1" == "C" copy .\obj\libDos32_C.LIB ..\..\..\lib\libDos32_C.LIB
if "%1" == "D" copy .\obj\libDos32_D.LIB ..\..\..\lib\libDos32_D.LIB