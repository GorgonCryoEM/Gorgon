SET builddir=%SRC_DIR%\..\build_dir
if errorlevel 1 exit 1

rmdir /s /q %builddir%
if errorlevel 1 exit 1
mkdir  %builddir%
if errorlevel 1 exit 1
cd %builddir%
if errorlevel 1 exit 1

cmake %SRC_DIR%
if errorlevel 1 exit 1

cmake --build %builddir% --config Release --target install
if errorlevel 1 exit 1

"%PYTHON%" setup.py install
if errorlevel 1 exit 1
