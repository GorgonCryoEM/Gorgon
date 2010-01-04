@echo off
set version=2.0.0b
md ..\..\bin\gorgon
md ..\..\bin\gorgon\gorgon_%version%_win32

..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc release

copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\source\Gorgon\src_py
copy ..\..\bin\boost\bin.v2\libs\python\build\msvc-8.0\release\threading-multi\boost_python-vc80-mt-1_34_1.dll ..\..\source\Gorgon\src_py
