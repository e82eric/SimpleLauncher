@ECHO off
rd /s /q bin
mkdir bin
CL /W4 /EHsc /nologo SimpleLauncher.c /DUNICODE /D_UNICODE /Zi /Fd"bin\SimpleLauncher.pdb" /Fo"bin\SimpleLauncher.obj" /Fe"bin\SimpleLauncher.exe"
