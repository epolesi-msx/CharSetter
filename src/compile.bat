@echo off

SET PROGRAM=CharSetter

if not exist ..\bin mkdir ..\bin > nul
if exist ..\bin\%PROGRAM%*.rom del ..\bin\%PROGRAM%*.rom

call :compile %PROGRAM% single_arabian CS_ARABIAN
if errorlevel 1 goto end
call :compile %PROGRAM% single_brazilian_g CS_BRAZIL_G
if errorlevel 1 goto end
call :compile %PROGRAM% single_brazilian_s CS_BRAZIL_S
if errorlevel 1 goto end
call :compile %PROGRAM% single_brazilian_g10 CS_BRAZIL_G10
if errorlevel 1 goto end
call :compile %PROGRAM% single_brazilian_s11 CS_BRAZIL_S11
if errorlevel 1 goto end
call :compile %PROGRAM% single_german CS_GERMAN
if errorlevel 1 goto end
call :compile %PROGRAM% single_international CS_INTERNAT
if errorlevel 1 goto end
call :compile %PROGRAM% single_japanese CS_JAPANESE
if errorlevel 1 goto end
call :compile %PROGRAM% single_korean CS_KOREAN
if errorlevel 1 goto end
call :compile %PROGRAM% single_russian CS_RUSSIAN
if errorlevel 1 goto end
call :compile %PROGRAM% multi_menu MULTI_CS
goto end

:compile
call :clean %1
echo -------- Compilation of %1 (%3)
echo .
sdcc -mz80 -D%3 --no-std-crt0 --opt-code-size --code-loc 0x4010 --data-loc 0 --allow-unsafe-read %1.c
if not exist %1.ihx goto error1
hex2bin –e bin %1.ihx
if not exist %1.bin goto error2
copy /b ..\resource\rom_header.bin + %1.bin ..\bin\%1_%2.rom > nul
if not exist ..\bin\%1_%2.rom goto error3
if not "%3"=="MULTI_CS" ..\resource\fillfile.exe ..\bin\%1_%2.rom 16384
if "%3"=="MULTI_CS" ..\resource\fillfile.exe ..\bin\%1_%2.rom 32768
if errorlevel 1 goto error4
echo .
echo OK
call :clean %1
exit /b 0

:error1
echo .
echo ERROR during compilation!
exit /b 1

:error2
echo .
echo ERROR generating bin file!
exit /b 1

:error3
echo .
echo ERROR generating rom file!
exit /b 1

:error4
echo .
echo ERROR filling rom file!
exit /b 1

:clean
if exist %1.o   del %1.o
if exist %1.sym del %1.sym
if exist %1.map del %1.map
if exist %1.asm del %1.asm
if exist %1.ihx del %1.ihx
if exist %1.lst del %1.lst
if exist %1.lnk del %1.lnk
if exist %1.bin del %1.bin
if exist %1.lk  del %1.lk
if exist %1.rel del %1.rel
if exist %1.noi del %1.noi
exit /b

:end
SET PROGRAM=