# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that manages all the windows of the main form. 


from PyQt4 import QtGui, QtCore
# from help_menus import HelpMenus
from camera import Camera
from volume_viewer import VolumeViewer
# from skeleton_viewer import SkeletonViewer
# from sse_viewer import SSEViewer
# from calpha_viewer import CAlphaViewer
# from calpha_reference_viewer import CAlphaReferenceViewer
import datetime

class WindowManager(QtGui.QWidget):
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.createUI()

    def createUI(self):
        self.createChildWindows()        
        
    def createChildWindows(self):
#         self.helpMenus = HelpMenus(self.app)
        self.volumeViewer = VolumeViewer(self.app)
#         self.skeletonViewer = SkeletonViewer(self.app)
#         self.sseViewer = SSEViewer(self.app)
#         self.calphaViewer = CAlphaViewer(self.app)
#         self.calphaReference = CAlphaReferenceViewer(self.app)
#         self.mainCamera = Camera([self.calphaViewer, self.calphaReference, self.sseViewer, self.skeletonViewer, self.volumeViewer], self.app)
        self.mainCamera = Camera([self.volumeViewer], self.app)
        self.app.mainCamera = self.mainCamera   
        self.app.setCentralWidget(self.mainCamera)
        
    def closeSession(self):
        if (QtGui.QMessageBox.warning(self, self.tr("Are you sure?"), self.tr("You are about to close the session.  All unsaved data will be lost!"), QtGui.QMessageBox.Yes, QtGui.QMessageBox.Cancel) == QtGui.QMessageBox.Yes):
            if (self.volumeViewer.loaded) :
                self.volumeViewer.unloadData()
            if (self.skeletonViewer.loaded) :
                self.skeletonViewer.unloadData()
            if (self.sseViewer.loaded) :
                self.sseViewer.unloadData()
            if (self.calphaViewer.loaded) :
                self.calphaViewer.unloadData()
            if (self.calphaReference.loaded) :
                self.calphaReference.unloadData()
