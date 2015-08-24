from PyQt4 import QtGui, QtCore
from camera import Camera
from volume_viewer import VolumeViewer
import datetime

class WindowManager(QtGui.QWidget):
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.createChildWindows()

    def createChildWindows(self):
        self.volumeViewer = VolumeViewer(self.app)
        self.mainCamera = Camera([self.volumeViewer], self.app)
        self.app.setCentralWidget(self.mainCamera)
