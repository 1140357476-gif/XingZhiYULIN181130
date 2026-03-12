@ECHO OFF
if "%1" == "view" copy .\obj\libPb.LIB ..\..\..\lib\libPb.LIB
if "%1" == "edit" copy .\obj\libPbEdit.LIB ..\..\..\lib\libPbEdit.LIB
if "%1" == "global" copy .\obj\libPbGlobal.LIB ..\..\..\lib\libPbGlobal.LIB