# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   The Gorgon application 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.10  2008/11/28 17:56:09  ssa1
#   Splash screen for public beta 1 release
#
#   Revision 1.9  2008/11/28 04:36:17  ssa1
#   Removing error message if pyopengl does not exist.  (To make executable building easier to debug)
#
#   Revision 1.8  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

###########################################################################################################
# Following is a hack to overcome a PyOpenGL bug with P2EXE Should be removed when py2exe adds egg support
import sys, os
pathname = os.path.dirname(sys.argv[0])
pathname = os.path.abspath(pathname)
sys.path.append(pathname + "\\setuptools-0.6c9-py2.5.egg")
sys.path.append(pathname + "\\pyopengl-3.0.0b4-py2.5.egg")
###########################################################################################################


from window_manager import WindowManager
from PyQt4 import QtGui, QtCore
from main_window_form import MainWindowForm
from about_form import AboutForm
import time


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    pixmap = QtGui.QPixmap(pathname + "/splash.png")
    splash = QtGui.QSplashScreen(pixmap, QtCore.Qt.WindowStaysOnTopHint)
    splash.setMask(pixmap.mask())
    splash.show()
    app.processEvents()
    
    window = MainWindowForm()
    window.addModule(WindowManager(window))
    window.showMaximized()
    splash.finish(window)
    sys.exit(app.exec_())
    
