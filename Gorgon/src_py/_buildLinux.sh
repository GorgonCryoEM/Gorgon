#!/bin/bash
mkdir ../../../GorgonLinux
#cp *.py ../../../GorgonLinux/
cp *.py? ../../../GorgonLinux/
#rm ../../../GorgonLinux/_build*
cp gorgon-linux.sh ../../../GorgonLinux/gorgon.sh
cp gorgon.icns ../../../GorgonLinux/
cp gorgon.ico ../../../GorgonLinux/
cp splash.png ../../../GorgonLinux/

mkdir ../../../GorgonLinux/correspondence
#cp correspondence/*.py ../../../GorgonLinux/correspondence/
cp correspondence/*.py? ../../../GorgonLinux/correspondence/

mkdir ../../../GorgonLinux/seq_model
#p seq_model/*.py ../../../GorgonLinux/seq_model/
cp seq_model/*.py? ../../../GorgonLinux/seq_model/

cp ~/Gorgon/lib/libpyGORGON.so ../../../GorgonLinux/
cp /usr/lib/libglut.so.3 ../../../GorgonLinux/
cp /usr/lib/libGLU.so.1 ../../../GorgonLinux/
cp /usr/lib/libGL.so.1 ../../../GorgonLinux/
cp /usr/lib/libboost_python-gcc42-1_34_1.so.1.34.1 ../../../GorgonLinux/
cp /usr/lib/libQtCore.so.4 ../../../GorgonLinux/
cp /usr/lib/libQtGui.so.4 ../../../GorgonLinux/
cp /usr/lib/libQtOpenGL.so.4 ../../../GorgonLinux

mkdir ../../../GorgonLinux/site-packages
cp -rL /usr/lib/python2.5/site-packages/OpenGL ../../../GorgonLinux/site-packages/
cp -rL /usr/lib/python2.5/site-packages/PyOpenGL-3.0.0b1.egg-info/ ../../../GorgonLinux/site-packages/
cp /usr/lib/python2.5/site-packages/pkg_resources.py ../../../GorgonLinux/site-packages/
cp /usr/lib/python2.5/site-packages/pkg_resources.pyc ../../../GorgonLinux/site-packages/
mkdir ../../../GorgonLinux/site-packages/PyQt4
cp /usr/lib/python2.5/site-packages/PyQt4/__init__.py ../../../GorgonLinux/site-packages/PyQt4/
cp /usr/lib/python2.5/site-packages/PyQt4/__init__.pyc ../../../GorgonLinux/site-packages/PyQt4/
cp /usr/lib/python2.5/site-packages/PyQt4/Qt.so ../../../GorgonLinux/site-packages/PyQt4/
cp /usr/lib/python2.5/site-packages/PyQt4/QtCore.so ../../../GorgonLinux/site-packages/PyQt4/
cp /usr/lib/python2.5/site-packages/PyQt4/QtGui.so ../../../GorgonLinux/site-packages/PyQt4/
cp /usr/lib/python2.5/site-packages/PyQt4/QtOpenGL.so ../../../GorgonLinux/site-packages/PyQt4/
cp /usr/lib/python2.5/site-packages/sip* ../../../GorgonLinux/site-packages/
