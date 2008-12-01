@echo off
copy ..\..\ExternalLibraries\GL\glut32.dll > NUL
copy "C:\Program Files\Mozilla Firefox\plugins\Microsoft.VC80.CRT\msvcr80.dll" > NUL
copy "C:\Program Files\Mozilla Firefox\plugins\Microsoft.VC80.CRT\msvcp80.dll" > NUL

python _buildExecutableWin32.py py2exe 

del msvcr80.dll
del msvcp80.dll
del glut32.dll
copy  C:\Programs\Python25\Lib\site-packages\setuptools-0.6c9-py2.5.egg dist\ > NUL
md dist\pyopengl-3.0.0b4-py2.5.egg > NUL
xcopy C:\Programs\Python25\Lib\site-packages\pyopengl-3.0.0b4-py2.5.egg\* dist\pyopengl-3.0.0b4-py2.5.egg\ /E /Y /Q > NUL

