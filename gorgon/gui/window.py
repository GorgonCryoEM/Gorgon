import os
import sys

from PyQt4 import QtGui

from .explorer.volume_viewer import VolumeViewer
from .explorer.skeleton_viewer import SkeletonViewer
from .explorer import Camera


class Window(QtGui.QMainWindow):

    def __init__(self, args, form):
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
        
        if form:
            self.form = form(self)
            if(hasattr(self.form, 'show')):
                self.form.show()
        
        self.setWindowTitle(self.tr("Gorgon GUI"))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    @classmethod
    def set_parser(cls, parser):
        pass

    def load(self):
        self.volume.load(self.args.volume)
        self.form.modelLoaded()
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
