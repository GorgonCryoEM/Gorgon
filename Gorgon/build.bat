@echo off
md ..\..\bin\gorgon

..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc release

copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\source\Gorgon\src_py

cd src_py
call _buildWin32.cmd
cd ..

xcopy src_py\dist\* ..\..\bin\gorgon\ /E /Y /Q > NUL
