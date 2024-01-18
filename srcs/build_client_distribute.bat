@echo off

@premake5.exe --os=windows vs2022

@call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

@msbuild /t:Rebuild /p:Configuration=Distribute build/UserInterface.vcxproj

@pause
