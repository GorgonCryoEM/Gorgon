@echo off
set version=1.0.1
md ..\..\bin\gorgon
md ..\..\bin\gorgon\gorgon_%version%_win32

..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc release

copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\source\Gorgon\src_py

cd src_py
call _buildWin32.cmd
cd ..

xcopy src_py\dist\* ..\..\bin\gorgon\gorgon_%version%_win32\ /E /Y /Q > NUL
del ..\..\bin\gorgon\gorgon_%version%_win32.zip
cd ..\..\bin\gorgon\
zip -r -9 gorgon_%version%_win32.zip gorgon_%version%_win32\*
cd ..\..\source\Gorgon

copy /Y ..\..\bin\gorgon\gorgon_%version%_win32.zip C:\wwwroot\HomePage\gorgon\resources\
copy /Y ..\..\bin\gorgon\gorgon_%version%_win32.zip Z:\web\gorgon\resources\ 