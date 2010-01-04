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
#   Revision 1.19  2009/12/24 01:38:53  ssa1
#   Fixing bug in macos where color dialogs automatically change when camera changes.  Bug ID 4
#
#   Revision 1.18  2009/04/14 20:17:35  ssa1
#   Changing scripts for PyOpenGL 3.0.0
#
#   Revision 1.17  2009/04/14 18:46:20  ssa1
#   Changing how the WindowManager and PluginManager are loaded
#
#   Revision 1.16  2009/04/08 19:54:59  ssa1
#   Adding in plugin functionality
#
#   Revision 1.15  2008/12/18 15:19:31  ssa1
#   Moving About Form functionality into HelpMenus
#
#   Revision 1.14  2008/12/17 16:00:04  ssa1
#   Changing Version information for next public release
#
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
    sys.path.append(pathname)
elif(sys.platform == 'darwin'):
    if os.environ.has_key('RESOURCEPATH') :
        sys.path = [os.path.join(os.environ['RESOURCEPATH'], 'lib', 'python2.5', 'lib-dynload')] + sys.path
###########################################################################################################


from PyQt4 import QtGui, QtCore
from main_window_form import MainWindowForm

import time


if __name__ == '__main__':
    gorgonVersion = '2.0.0'
    app = QtGui.QApplication(sys.argv)    
    pixmap = QtGui.QPixmap(pathname + "/splash.png")
    splash = QtGui.QSplashScreen(pixmap, QtCore.Qt.WindowStaysOnTopHint)
    splash.setMask(pixmap.mask())
    splash.show()
    app.processEvents()
    
    window = MainWindowForm(gorgonVersion)
    window.showMaximized()
    splash.finish(window)
    window.loadPlugins()
    sys.exit(app.exec_())
    
