@echo off

%PYTHON_ROOT%\python _buildExecutableWin32.py py2exe 

md dist\OpenGL > NUL
xcopy %PYTHON_ROOT%\Lib\site-packages\OpenGL\* dist\OpenGL\ /E /Y /Q > NUL

