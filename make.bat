@echo off
setlocal
set PATH=C:\MinGW\bin:%PATH%
C:\MinGW\bin\make.exe CC=C:\MinGW\bin\gcc.exe MINGW=YES
endlocal
