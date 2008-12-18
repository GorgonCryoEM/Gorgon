# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that manages all the windows of the main form. 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.9  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtGui, QtCore
from help_menus import HelpMenus
from camera import Camera
from volume_viewer import VolumeViewer;
from skeleton_viewer import SkeletonViewer;
from sse_viewer import SSEViewer
from calpha_viewer import CAlphaViewer

class WindowManager(QtGui.QWidget):
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.createUI()

    def createUI(self):
        self.createChildWindows()        
        self.createActions()
        self.createMenus()
        
    def createChildWindows(self):
        self.helpMenus = HelpMenus(self.app)
        self.volumeViewer = VolumeViewer(self.app)
        self.skeletonViewer = SkeletonViewer(self.app)
        self.sseViewer = SSEViewer(self.app)
        self.calphaViewer = CAlphaViewer(self.app)
        self.mainCamera = Camera([self.calphaViewer, self.sseViewer, self.skeletonViewer, self.volumeViewer], self.app)
        self.app.mainCamera = self.mainCamera   
        self.app.setCentralWidget(self.mainCamera)
        
    def createActions(self):
        pass
            
    def createMenus(self):
        pass
