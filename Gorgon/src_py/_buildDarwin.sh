#!/bin/bash

topdir="/Users/durmaz/Files/eclipse_workspace/workspace_work/gitGorgonBitBucket/Gorgon/src_py"
fboost="/opt/local/lib/libboost_python-mt.dylib"

rm -r build dist libpyGORGON.so libpyGORGON.dylib

python _buildExecutableDarwin.py py2app

mkdir -p dist/gorgon.app/Contents/Frameworks
mkdir -p dist/gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/

cp -v ~/Gorgon/lib/libpyGORGON.dylib .
cp -v $fboost . 
cp -v /opt/local/lib/libfftw3f.dylib .

cp -v libpyGORGON.dylib      dist/gorgon.app/Contents/Frameworks
cp -v $fboost                dist/gorgon.app/Contents/Resources/ 
cp -v libfftw3f.dylib        dist/gorgon.app/Contents/Resources/

#cd dist
mkdir dist/plugins
cp -v plugins/*.py dist/plugins
#cd ..

#cd dist/gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/
cp -av     libpyGORGON.dylib     dist/gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/libpyGORGON.so
chmod +rx  dist/gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/libpyGORGON.so

cd dist/gorgon.app/Contents/Resources/lib/python2.7/lib-dynload/
ln -sv   ../OpenGL
chmod +rx  OpenGL
cd $topdir
#cd ../../../../../../../
cd dist/gorgon.app/Contents/Resources
ln -sv   ../../../../plugins

#cd ../../../..
cd $topdir
#cp -av libpyGORGON.dylib libpyGORGON.so
