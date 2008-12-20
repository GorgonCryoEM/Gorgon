#!/bin/bash
mv ~/Gorgon/lib/libpyGORGON.so ~/
rm -rf ~/Gorgon
mkdir ~/Gorgon
mkdir ~/Gorgon/lib
mv ~/libpyGORGON.so ~/Gorgon/lib

#cp *.py ~/Gorgon/
cp *.py? ~/Gorgon/
#rm ~/Gorgon/_build*
cp gorgon-linux.sh ~/Gorgon/gorgon.sh
cp gorgon.icns ~/Gorgon/
cp gorgon.ico ~/Gorgon/
cp splash.png ~/Gorgon/

mkdir ~/Gorgon/correspondence
#cp correspondence/*.py ~/Gorgon/correspondence/
cp correspondence/*.py? ~/Gorgon/correspondence/

mkdir ~/Gorgon/seq_model
#cp seq_model/*.py ~/Gorgon/seq_model/
cp seq_model/*.py? ~/Gorgon/seq_model/

mkdir ~/Gorgon/themes
cp -rL ../resources ~/Gorgon/themes

cp /usr/lib/libboost_python-gcc42-1_34_1.so.1.34.1 ~/Gorgon/lib/
cp -P /usr/lib/libQtCore.so* ~/Gorgon/lib/
cp -P /usr/lib/libQtGui.so* ~/Gorgon/lib/
cp -P /usr/lib/libQtOpenGL.so* ~/Gorgon/lib/
cp -P /usr/lib/libaudio.so* ~/Gorgon/lib/

mkdir ~/Gorgon/bin
cp -r /usr/bin/python* ~/Gorgon/bin/
cp -rL /usr/lib/python2.5 ~/Gorgon/lib/

rm -r ~/Gorgon/lib/python2.5/site-packages
mkdir ~/Gorgon/lib/python2.5/site-packages
cp -rL /usr/lib/python2.5/site-packages/OpenGL ~/Gorgon/lib/python2.5/site-packages/
cp -rL /usr/lib/python2.5/site-packages/PyOpenGL* ~/Gorgon/lib/python2.5/site-packages/
cp -L /usr/lib/python2.5/site-packages/pkg_resources.py* ~/Gorgon/lib/python2.5/site-packages/
mkdir ~/Gorgon/lib/python2.5/site-packages/PyQt4
cp -L /usr/lib/python2.5/site-packages/PyQt4/__init__.py* ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/lib/python2.5/site-packages/PyQt4/Qt.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/lib/python2.5/site-packages/PyQt4/QtCore.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/lib/python2.5/site-packages/PyQt4/QtGui.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/lib/python2.5/site-packages/PyQt4/QtOpenGL.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/lib/python2.5/site-packages/sip* ~/Gorgon/lib/python2.5/site-packages/
