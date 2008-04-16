# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: A widget that manages all the windows of the main form.

from PyQt4 import QtGui, QtCore
from about_form import AboutForm
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
        self.aboutForm = AboutForm(self.app)
        self.volumeViewer = VolumeViewer(self.app)
        self.skeletonViewer = SkeletonViewer(self.app)
        self.sseViewer = SSEViewer(self.app)
        self.calphaViewer = CAlphaViewer(self.app)
        self.mainCamera = Camera([self.calphaViewer, self.sseViewer, self.skeletonViewer, self.volumeViewer])   
        self.app.setCentralWidget(self.mainCamera)
        
    def createActions(self):
        pass
            
    def createMenus(self):
        pass
