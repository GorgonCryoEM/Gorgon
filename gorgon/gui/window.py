import os
import sys

from PyQt4 import QtGui

from .explorer.volume_viewer import VolumeViewer
from .explorer.skeleton_viewer import SkeletonViewer
from .explorer import Camera


class Window(QtGui.QMainWindow):

    def __init__(self, version, form, args):
        super(Window, self).__init__()
        
        self.args = args
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        self.docks = []
        
        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        
        self.scene = []
        self.scene.append(self.volume)
        self.scene.append(self.skeleton)
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.form = form(self)
        self.form.show()
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def load(self):
        self.volume.load(self.args.volume)
        self.form.modelLoaded()
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
