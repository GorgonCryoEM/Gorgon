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
from PyQt4 import QtGui
from main_window_form import MainWindowForm
from about_form import AboutForm


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    window = MainWindowForm()
    window.addModule(WindowManager(window))
    window.showMaximized()
    sys.exit(app.exec_())
    
