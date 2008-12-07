@echo off
md ..\..\bin\gorgon
md ..\..\bin\gorgon\gorgon_1.0.0b_win32

..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc release

copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\source\Gorgon\src_py

cd src_py
call _buildWin32.cmd
cd ..

xcopy src_py\dist\* ..\..\bin\gorgon\gorgon_1.0.0b_win32\ /E /Y /Q > NUL
