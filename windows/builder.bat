@echo off
SET FLG=0x00
:START
copy .\bin\dummy.txt CTFILE.txt
echo Please put in the data that will be requested from you.
echo It will automatically continue in 10 seconds.
.\bin\timeout.exe /t 10
GOTO trackdefstart
:trackdefstart
cls
SET /P CUPNAME=Enter the Cupname here: 
SET /P S0=Enter the Slot (see README.txt): 
SET /P M0=Enter the Music Slot (see README.txt): 
SET /P TNAME0=Enter the name of the SZS File without the extension: 
SET /P FULLTNAME0=Enter the name that should show up in MKW: 
cls
SET /P S1=Enter the Slot (see README.txt): 
SET /P M1=Enter the Music Slot (see README.txt): 
SET /P TNAME1=Enter the name of the SZS File without the extension: 
SET /P FULLTNAME1=Enter the name that should show up in MKW: 
cls
SET /P S2=Enter the Slot (see README.txt): 
SET /P M2=Enter the Music Slot (see README.txt): 
SET /P TNAME2=Enter the name of the SZS File without the extension: 
SET /P FULLTNAME2=Enter the name that should show up in MKW: 
cls
SET /P S3=Enter the Slot (see README.txt): 
SET /P M3=Enter the Music Slot (see README.txt): 
SET /P TNAME3=Enter the name of the SZS File without the extension: 
SET /P FULLTNAME3=Enter the name that should show up in MKW: 
cls
(
echo C "%CUPNAME%"
echo T %S0%  ; %M0%  ; %FLG%  ; "%TNAME0%"; "%FULLTNAME0%"; "%FULLTNAME0%"
echo T %S1%  ; %M1%  ; %FLG%  ; "%TNAME1%"; "%FULLTNAME1%"; "%FULLTNAME1%"
echo T %S2%  ; %M2%  ; %FLG%  ; "%TNAME2%"; "%FULLTNAME2%"; "%FULLTNAME2%"
echo T %S3%  ; %M3%  ; %FLG%  ; "%TNAME3%"; "%FULLTNAME3%"; "%FULLTNAME3%"
echo lecodetoolz

) >> "%~1"
.\bin\fart.exe "%~1" "lecodetoolz" " "
GOTO trackdefstart