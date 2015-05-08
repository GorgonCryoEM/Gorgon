@echo off
call _setPathsWin64.bat

md ..\bin\gorgon
md ..\bin\gorgon\gorgon_%VERSION%_%PLATFORM%

cd Gorgon
..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc release
cd ..

copy ..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd Gorgon\src_py /Y
copy ..\bin\boost\bin.v2\libs\python\build\msvc-8.0\release\threading-multi\boost_python-vc80-mt-1_34_1.dll Gorgon\src_py /Y
copy ExternalLibraries\FFTW\Win32\*.dll Gorgon\src_py /Y
copy ExternalLibraries\GL\glut32.dll Gorgon\src_py /Y
copy ExternalLibraries\msvc\msvcr80.dll Gorgon\src_py /Y
copy ExternalLibraries\msvc\msvcp80.dll Gorgon\src_py /Y

cd Gorgon\src_py
call _buildWin64.cmd
cd ..\..

xcopy Gorgon\src_py\dist\* ..\bin\gorgon\gorgon_%VERSION%_%PLATFORM%\ /E /Y /Q > NUL
del ..\bin\gorgon\gorgon_%VERSION%_%PLATFORM%.zip
cd ..\bin\gorgon\

%ZIP7_PATH%\7z.exe a gorgon_%VERSION%_%PLATFORM%.zip gorgon_%VERSION%_%PLATFORM%\*
cd ..\..

ftp -s:ftpUpload.txt
