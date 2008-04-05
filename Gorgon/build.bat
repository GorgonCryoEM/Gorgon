@echo off
md ..\..\bin\gorgon
del ..\..\bin\gorgon\*.* /Q
..\ExternalLibraries\boost_1_34_1\bin\bjam --build-dir=..\..\bin --toolset=msvc release
copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\bin\gorgon\
copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\source\Gorgon\src_py
copy ..\..\bin\boost\bin.v2\libs\python\build\msvc-8.0\release\threading-multi\*.dll ..\..\bin\gorgon\
copy ..\ExternalLibraries\GL\*.dll ..\..\bin\gorgon\

copy src_py\*.py ..\..\bin\gorgon\
copy src_py\*.pyw ..\..\bin\gorgon\
echo @python gorgon.pyw > ..\..\bin\gorgon\r.bat