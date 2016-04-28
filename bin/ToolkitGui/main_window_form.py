from PyQt4 import QtCore, QtGui
from Explorer import Camera
from .scene import Scene

from Explorer.volume_viewer import VolumeViewer
from Explorer.skeleton_viewer import SkeletonViewer

import sys, os


class MainWindowForm(QtGui.QMainWindow):

    def __init__(self, version, args):
        super(MainWindowForm, self).__init__()
        
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        
        self.volumeViewer = VolumeViewer(self)
        self.skeletonViewer = SkeletonViewer(self)

        self.volumeViewer.load(args.volume)
        self.skeletonViewer.load(args.skeleton)

        scenes = [self.skeletonViewer, self.volumeViewer]

        self.mainCamera = Camera(scenes, self)
        self.setCentralWidget(self.mainCamera)
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
