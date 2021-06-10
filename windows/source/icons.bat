@echo off
rem Define Variables
SET /P MKW-PATH=<path-to-disc-root.txt
rem Create TPL
wimgt copy "%~1" -rqo %MKW-PATH%\files\Scene\UI\ct_icons.tpl -x tpl.CMPR
rem Patch Files
cd %MKW-PATH%\files\Scene\UI
mkdir button
mkdir control
mkdir timg
move timg control
mkdir timg
move timg button
copy ct_icons.tpl .\button\timg
move ct_icons.tpl .\control\timg
wszst x --quiet Channel.szs
wszst x --quiet MenuMulti.szs
wszst x --quiet MenuSingle.szs
copy /y /b button Channel.d
copy /y /b button MenuSingle.d
copy /y /b button MenuMulti.d
copy /y /b control Channel.d
copy /y /b control MenuSingle.d
copy /y /b control MenuMulti.d
wszst c *.d -o --quiet
rmdir /s /q *.d
pause
exit