@echo off
set version=1.0.2b
md ..\..\bin\gorgon
md ..\..\bin\gorgon\gorgon_%version%_win32

..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc release

copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\source\Gorgon\src_py

