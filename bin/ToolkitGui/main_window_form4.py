from PyQt4 import QtCore, QtGui
import sys, os

from Explorer import Camera
from Explorer.volume_viewer import VolumeViewer
from Explorer.skeleton_viewer import SkeletonViewer
from .sse_helix_correspondence_finder_form import SSEHelixCorrespondenceFinderForm
from .calpha_viewer import CAlphaViewer
from .sse_viewer import SSEViewer


class MainWindowForm4(QtGui.QMainWindow):

    def __init__(self, version, args):
        super(MainWindowForm4, self).__init__()
        
        self.args = args
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        
#         self.volumeViewer = VolumeViewer(self)
        self.skeletonViewer = SkeletonViewer(self)
        self.calphaViewer = CAlphaViewer(self)
        self.sseViewer    = SSEViewer(self)
        
        self.scene = [self.skeletonViewer, self.calphaViewer, self.sseViewer]
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.form = SSEHelixCorrespondenceFinderForm(self)
        self.form.show()
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def load(self):
        self.volumeViewer.load(self.args.volume)
        self.skeletonViewer.load(self.args.skeleton)
        self.form.modelLoaded()
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
