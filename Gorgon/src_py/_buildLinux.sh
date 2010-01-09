#!/bin/bash
mv ~/Gorgon/lib/libpyGORGON.so ~/
rm -rf ~/Gorgon
mkdir ~/Gorgon
mkdir ~/Gorgon/lib
mv ~/libpyGORGON.so ~/Gorgon/lib/

#cp *.py ~/Gorgon/
cp *.py? ~/Gorgon/lib/
mv ~/Gorgon/lib/gorgon.pyw ~/Gorgon/
#rm ~/Gorgon/_build*
cp gorgon-linux.sh ~/Gorgon/gorgon.sh
cp gorgon.icns ~/Gorgon/
cp gorgon.ico ~/Gorgon/
cp splash.png ~/Gorgon/

mkdir ~/Gorgon/correspondence
#cp correspondence/*.py ~/Gorgon/correspondence/
cp correspondence/*.py? ~/Gorgon/correspondence/

mkdir ~/Gorgon/plugins
#cp plugins/*.py ~/Gorgon/plugins/
cp plugins/*.py? ~/Gorgon/plugins/

mkdir ~/Gorgon/seq_model
#cp seq_model/*.py ~/Gorgon/seq_model/
cp seq_model/*.py? ~/Gorgon/seq_model/

mkdir ~/Gorgon/themes
cp -L ../resources/* ~/Gorgon/themes

cp -P /usr/lib64/libglut.so* ~/Gorgon/lib/
#cp -P /usr/lib/libglut.so* ~/Gorgon/lib/
cp -P /usr/local/lib/libboost_python.so* ~/Gorgon/lib/
cp -P /usr/local/lib/libpython2.5.so* ~/Gorgon/lib/
cp -P /usr/local/lib/libfftw3f.so* ~/Gorgon/lib/
cp -P /usr/local/Trolltech/Qt-4.6.0/lib/libQtCore.so* ~/Gorgon/lib/
cp -P /usr/local/Trolltech/Qt-4.6.0/lib/libQtGui.so* ~/Gorgon/lib/
cp -P /usr/local/Trolltech/Qt-4.6.0/lib/libQtOpenGL.so* ~/Gorgon/lib/
#cp -P /usr/local/Trolltech/Qt-4.6.0/lib/libaudio.so* ~/Gorgon/lib/
rm ~/Gorgon/lib/*.debug

mkdir ~/Gorgon/bin
cp -r /usr/local/bin/python* ~/Gorgon/bin/
cp -rL /usr/local/lib/python2.5 ~/Gorgon/lib/

rm -r ~/Gorgon/lib/python2.5/site-packages
mkdir ~/Gorgon/lib/python2.5/site-packages
cp -rL /usr/local/lib/python2.5/site-packages/OpenGL ~/Gorgon/lib/python2.5/site-packages/
cp -rL /usr/local/lib/python2.5/site-packages/PyOpenGL* ~/Gorgon/lib/python2.5/site-packages/
#cp -L /usr/local/lib/python2.5/site-packages/pkg_resources.py* ~/Gorgon/lib/python2.5/site-packages/
mkdir ~/Gorgon/lib/python2.5/site-packages/PyQt4
cp -L /usr/local/lib/python2.5/site-packages/PyQt4/__init__.py* ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/local/lib/python2.5/site-packages/PyQt4/Qt.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/local/lib/python2.5/site-packages/PyQt4/QtCore.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/local/lib/python2.5/site-packages/PyQt4/QtGui.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/local/lib/python2.5/site-packages/PyQt4/QtOpenGL.so ~/Gorgon/lib/python2.5/site-packages/PyQt4/
cp -L /usr/local/lib/python2.5/site-packages/sip* ~/Gorgon/lib/python2.5/site-packages/
