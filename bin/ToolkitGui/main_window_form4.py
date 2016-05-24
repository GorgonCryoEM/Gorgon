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
        
        self.volumeViewer = VolumeViewer(self)
        self.skeletonViewer = SkeletonViewer(self)
        self.calphaViewer = CAlphaViewer(self)
        self.sseViewer    = SSEViewer(self)
        
        self.scene = [self.volumeViewer, self.skeletonViewer, self.calphaViewer, self.sseViewer]
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def load(self):
        self.form = self.sseViewer.helixCorrespondanceFinder
        self.sseViewer.helixCorrespondanceFinder.viewer = self.sseViewer
#         self.form.show()
#
        self.volumeViewer.load(self.args.volume)
        self.skeletonViewer.load(self.args.skeleton)
        self.calphaViewer.loadSeq('groel-segment.seq')
#         self.sseViewer.loadHelixDataFromFile('groel-segment.seq')
#         self.sseViewer.loadHelixData()
#         self.sseViewer.helixCorrespondanceFinder.viewer = self.sseViewer
        self.form.sequenceFileName = QtCore.QString('groel-segment.seq')
        self.form.helixFileName    = QtCore.QString('helices-densityMap.wrl')
#
        self.sseViewer.loadHelixDataFromFile('helices-densityMap.wrl')
#         self.sseViewer.loadHelixData('helices-densityMap.wrl')
        self.form.ui.lineEditSkeletonFile.setText(self.args.skeleton)
        self.form.ui.lineEditSequenceFile.setText('groel-segment.seq')
        self.form.ui.lineEditHelixLocationFile.setText('helices-densityMap.wrl')
#         self.form.loadDefaults()
#         self.form.setConstants()
# #         self.form.lineEditSheetLocationFile.setText()
#         self.form.drawOverlay()
        self.form.checkOk()
#         self.form.accept()

#         self.form.viewer.sheetFileName    = QtCore.QString('groel-segment.seq')
#         self.volumeViewer.load(self.args.volume)
#         self.form.modelLoaded()
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
