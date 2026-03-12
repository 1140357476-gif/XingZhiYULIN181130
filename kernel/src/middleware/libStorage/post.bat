@ECHO OFF
if "%1" == "common" copy .\obj\libStorage_com.LIB ..\..\..\lib\libStorage_com.LIB
if "%1" == "sd" copy .\obj\libStorage_sd.LIB ..\..\..\lib\libStorage_sd.LIB
if "%1" == "nand" copy .\obj\libStorage_nand.LIB ..\..\..\lib\libStorage_nand.LIB
if "%1" == "nor" copy .\obj\libStorage_nor.LIB ..\..\..\lib\libStorage_nor.LIB
if "%1" == "spi" copy .\obj\libStorage_spi.LIB ..\..\..\lib\libStorage_spi.LIB
