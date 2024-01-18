@echo off
set PATH=C:\Python27;bin;%PATH%
if %1!==! goto build_with_param
set arg1=%1
"FoxFSArchiver" make_xml/%1_create.xml
"FoxFSArchiver" xml/%1_create.xml
echo "Done"
goto end
:build_with_param
set /p id="Enter File to make: " %=%
"FoxFSArchiver" make_xml/%id%_create.xml
"FoxFSArchiver" xml/%id%_create.xml > output.txt
goto build_with_param
pause
:end
echo "Finished Execution"