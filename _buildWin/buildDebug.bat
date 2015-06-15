@echo off
set version=2.0.0
md ..\..\bin\gorgon
md ..\..\bin\gorgon\gorgon_%version%_win32

..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc debug

copy ..\..\bin\gorgon\msvc-8.0\debug\threading-multi\*.pyd ..\..\source\Gorgon\src_py

