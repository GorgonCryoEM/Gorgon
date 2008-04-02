@echo off
md ..\..\bin\gorgon
del ..\..\bin\gorgon\*.* /Q
bjam --build-dir=..\..\bin --toolset=msvc release
copy ..\..\bin\gorgon\msvc-8.0\release\threading-multi\*.pyd ..\..\bin\gorgon\
copy src_py\*.* ..\..\bin\gorgon\
echo @python gorgon.pyw > ..\..\bin\gorgon\r.bat