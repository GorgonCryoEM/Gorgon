from PyQt4 import QtCore, QtGui
import sys, os

from Explorer import Camera
from Explorer.volume_viewer import VolumeViewer
from Explorer.skeleton_viewer import SkeletonViewer
from .volume_sse_builder_form import VolumeSSEBuilderForm
from .calpha_viewer import CAlphaViewer
from .sse_viewer import SSEViewer


class MainWindowForm3(QtGui.QMainWindow):

    def __init__(self, version, args):
        super(MainWindowForm3, self).__init__()
        
        self.args = args
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        
        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        self.calphaViewer = CAlphaViewer(self)
        self.sseViewer    = SSEViewer(self)
        
        self.scene = [self.volume, self.skeleton, self.calphaViewer, self.sseViewer]
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.form = VolumeSSEBuilderForm(self, self.volume, self.skeleton, self.args)
        self.form.show()
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def load(self):
        self.volume.load(self.args.volume)
        self.skeleton.load(self.args.skeleton)
        self.form.modelLoaded()
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
