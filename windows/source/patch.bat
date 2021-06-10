@echo off
rem Check if files exist
IF NOT EXIST path-to-disc-root.txt (
echo [!] path-to-disc-root.txt not found!
pause
exit
)
IF NOT EXIST path-to-lecode-build.txt (
echo [!] path-to-lecode-build.txt not found!
pause
exit
)
rem Define Variables
SET /P LE-BUILD=<path-to-lecode-build.txt
SET /P MKW-PATH=<path-to-disc-root.txt
rem Patch LECODE
copy .\szs-files\*.szs %MKW-PATH%\files\Race\Course\
wlect patch %LE-BUILD%/*.bin --le-define "%~1" --lpar "%~2" --track-dir %MKW-PATH%/files/Race/Course --copy-tracks %MKW-PATH%/files/Race/Course
pause
exit