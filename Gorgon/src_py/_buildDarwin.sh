#!/bin/bash

topdir="/Users/durmaz/Files/eclipse_workspace/workspace_work/gitGorgonBitBucket"
fboost="libboost_python.dylib"

cp -v ~/Gorgon/lib/libpyGORGON.dylib libpyGORGON.dylib
cp -v /Users/durmaz/Downloads/boost/boost_1_57_0/stage/lib/libboost_python.dylib $fboost 
cp -v ${topdir}/ExternalLibraries/FFTW/MacOS_x86_10.5/lib/libfftw3f.dylib libfftw3f.dylib

rm libpyGORGON.so
rm -R dist
rm -R build
python _buildExecutableDarwin.py py2app

mkdir -p dist/gorgon.app/Contents/Frameworks
mkdir -p dist/gorgon.app/Contents/Resources/
mkdir -p dist/gorgon.app/Contents/Resources/
mkdir -p dist/gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/

cp -v libpyGORGON.dylib dist/gorgon.app/Contents/Frameworks
cp -v $fboost dist/gorgon.app/Contents/Resources/ 
cp -v libfftw3f.dylib dist/gorgon.app/Contents/Resources/

cd dist
mkdir plugins
cp -v ../plugins/*.py plugins
cd ..

cd dist/gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/
cp -av ../../../../Frameworks/libpyGORGON.dylib libpyGORGON.so
chmod +rx libpyGORGON.so
ln -sv ../OpenGL OpenGL
chmod +rx OpenGL
cd ../../../../../../../
cd dist/gorgon.app/Contents/Resources
ln -sv ../../../plugins/ plugins
cd ../../../..


cp -av libpyGORGON.dylib libpyGORGON.so
