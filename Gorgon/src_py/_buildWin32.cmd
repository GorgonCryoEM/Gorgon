@echo off
copy ..\..\ExternalLibraries\GL\glut32.dll > NUL
copy ..\..\ExternalLibraries\msvc\msvcr80.dll > NUL
copy ..\..\ExternalLibraries\msvc\msvcp80.dll > NUL
copy ..\..\ExternalLibraries\FFTW\Win32\libfftw3-3.dll > NUL
copy ..\..\ExternalLibraries\FFTW\Win32\libfftw3f-3.dll > NUL
copy ..\..\ExternalLibraries\FFTW\Win32\libfftw3l-3.dll > NUL

python _buildExecutableWin32.py py2exe 

del msvcr80.dll 
del msvcp80.dll 
del glut32.dll 
del libfftw3-3.dll
del libfftw3f-3.dll
del libfftw3l-3.dll

md dist\OpenGL > NUL
xcopy %PYTHON_ROOT%\Lib\site-packages\OpenGL\* dist\OpenGL\ /E /Y /Q > NUL

