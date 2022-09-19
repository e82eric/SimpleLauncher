@ECHO off
rd /s /q bin
mkdir bin

CL /c /W4 /EHsc /nologo SimpleLauncherLib.c /DUNICODE /D_UNICODE /Zi /Fd"bin\SimpleLauncherLib.pdb" /Fo"bin\SimpleLauncherLib.obj"
CL /c /W4 /EHsc /nologo Config.c /DUNICODE /D_UNICODE /Zi /Fd"bin\Config.pdb" /Fo"bin\Config.obj"
CL /c /W4 /EHsc /nologo SimpleLauncher.c /DUNICODE /D_UNICODE /Zi /Fd"bin\SimpleLauncher.pdb" /Fo"bin\SimpleLauncher.obj"
LINK /DEBUG bin\SimpleLauncherLib.obj bin\Config.obj bin\SimpleLauncher.obj /OUT:bin\SimpleLauncher.exe
