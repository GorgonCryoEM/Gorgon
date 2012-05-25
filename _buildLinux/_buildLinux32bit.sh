#!/bin/bash

# Get new code and build libpyGORGON.so
cd ..
cvs update -d
cmake .
#make clean
make
cd _buildLinux

#Remove old files in build directory, but keep libpyGORGON.so
mv ~/Gorgon/lib/libpyGORGON.so ~/
rm -rf ~/Gorgon
mkdir ~/Gorgon
mkdir ~/Gorgon/lib
mv ~/libpyGORGON.so ~/Gorgon/lib/
cp run_gorgon.sh ~/Gorgon/gorgon.sh

# Copy from the source directory to the build directory
# It would probably be better to configure this with CMake so that "make install" does this... 
cd ~/src/graphics/ssa1/source/Gorgon/src_py
cp -r *.py ~/Gorgon/lib
#rm ~/Gorgon/_build*
cp gorgon.pyw ~/Gorgon
cp gorgon.icns ~/Gorgon/
cp gorgon.ico ~/Gorgon/
cp splash.png ~/Gorgon/

mkdir ~/Gorgon/correspondence
cp correspondence/*.py ~/Gorgon/correspondence/
mkdir ~/Gorgon/plugins
cp plugins/*.py ~/Gorgon/plugins/
mkdir ~/Gorgon/seq_model
cp seq_model/*.py ~/Gorgon/seq_model/
mkdir ~/Gorgon/themes
cp -L ../resources/* ~/Gorgon/themes

# Replace the *.py files with the *.pyc files because we aren't releasing source code yet.
# If we release source code in the future, *.py files should be bundled instead even for binary releases that include a Python interpreter.
cd ~/Gorgon
# Compile *.py files
python -m compileall .
# Remove Python source code (but leave gorgon.pyw and plugins directory source)
rm -r correspondence/*.py
rm -r lib/*.py
rm -r seq_model/*.py


# Now, copy the libraries needed by libpyGORGON.so that won't conflict with system libraries on a user's machine
# Discover what files are needed with the "ldd" command.

#cp -P /usr/lib64/libglut.so* ~/Gorgon/lib/
cp -P /usr/lib/libglut.so* ~/Gorgon/lib/
cp -P /usr/local/lib/libboost_python-gcc41-1_34_1.so* ~/Gorgon/lib/
cp -P /usr/local/lib/libpython2.5.so* ~/Gorgon/lib/
cp -P /usr/local/lib/libfftw3f.so* ~/Gorgon/lib/
cp -P /usr/local/Trolltech/Qt-4.7.4/lib/libQtCore.so* ~/Gorgon/lib/
cp -P /usr/local/Trolltech/Qt-4.7.4/lib/libQtGui.so* ~/Gorgon/lib/
cp -P /usr/local/Trolltech/Qt-4.7.4/lib/libQtOpenGL.so* ~/Gorgon/lib/

# We can't expect libpyGORGON.so to work with the user's system Python, so we need to bundle a Python interpreter, and required Python libraries.
mkdir ~/Gorgon/bin
cp -r /usr/local/bin/python* ~/Gorgon/bin/
cp -rL /usr/local/lib/python2.5 ~/Gorgon/lib/

# We may not need everything that is in site-packages so remove it and copy just the required parts to keep our tar.bz2 file size small
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

cd ~
GORGON_BINARY_FILENAME=gorgon_2.1.1b_linux_32.tar.bz2
rm $GORGON_BINARY_FILENAME
tar -cjvf $GORGON_BINARY_FILENAME ~/Gorgon/*
scp $GORGON_BINARY_FILENAME coleman.r@imbrium.seas.wustl.edu:/project/research-www/gorgon/nightlyUpload/
