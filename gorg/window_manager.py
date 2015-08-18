from PyQt4 import QtGui, QtCore
from camera import Camera
from volume_viewer import VolumeViewer
import datetime

class WindowManager(QtGui.QWidget):
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.createUI()

    def createUI(self):
        self.createChildWindows()        
        
    def createChildWindows(self):
        self.volumeViewer = VolumeViewer(self.app)
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
