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
#   Revision 1.13  2008/11/30 00:13:41  ssa1
#   Build Configuration scripts on MacOSX
#
#   Revision 1.12  2008/11/29 04:48:28  ssa1
#   Icon support and Redirecting help to website.
#
#   Revision 1.11  2008/11/28 19:47:18  ssa1
#   Splash screen for public beta 1 release
#
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
pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
if(sys.platform == 'win32'):
    sys.path.append(pathname + "\\setuptools-0.6c9-py2.5.egg")
    sys.path.append(pathname + "\\pyopengl-3.0.0b4-py2.5.egg")
elif(sys.platform == 'darwin'):
    if os.environ.has_key('RESOURCEPATH') :
        sys.path = [os.path.join(os.environ['RESOURCEPATH'], 'lib', 'python2.5', 'lib-dynload')] + sys.path
###########################################################################################################


from window_manager import WindowManager
from PyQt4 import QtGui, QtCore
from main_window_form import MainWindowForm
from about_form import AboutForm
import time


if __name__ == '__main__':
    gorgonVersion = '1.0.1'
    app = QtGui.QApplication(sys.argv)    
    pixmap = QtGui.QPixmap(pathname + "/splash.png")
    splash = QtGui.QSplashScreen(pixmap, QtCore.Qt.WindowStaysOnTopHint)
    splash.setMask(pixmap.mask())
    splash.show()
    app.processEvents()
    
    window = MainWindowForm(gorgonVersion)
    window.addModule(WindowManager(window))
    window.showMaximized()
    splash.finish(window)
    sys.exit(app.exec_())
    
