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
        
        self.viewers = {}
        
        self.volumeViewer = VolumeViewer(self)
        self.skeletonViewer = SkeletonViewer(self)
        self.sseViewer    = SSEViewer(self)
        self.calphaViewer = CAlphaViewer(self)
        self.viewers['volume'] = self.volumeViewer
        self.viewers['skeleton'] = self.skeletonViewer
        self.viewers['sse'] = self.sseViewer
        self.viewers['calpha'] = self.calphaViewer
        
        self.scene = []
        self.scene.append(self.volumeViewer)
        self.scene.append(self.skeletonViewer)
        self.scene.append(self.sseViewer)
        self.scene.append(self.calphaViewer)
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
#         exit()
        
    def load(self):
        self.form = self.sseViewer.helixCorrespondanceFinder
#         self.form.show()
#
        self.volumeViewer.load(self.args.volume)
        self.skeletonViewer.load(self.args.skeleton)
        self.sseViewer.loadHelixDataFromFile(self.args.helix)
        self.calphaViewer.loadSeq(self.args.sequence)

        self.form.ui.lineEditSkeletonFile.setText(self.args.skeleton)
        self.form.ui.lineEditSequenceFile.setText(self.args.sequence)
        self.form.ui.lineEditHelixLocationFile.setText(self.args.helix)
#         self.form.loadDefaults()
#         self.form.setConstants()
# #         self.form.lineEditSheetLocationFile.setText()
#         self.form.drawOverlay()
        self.form.checkOk()
        self.form.ui.pushButtonOk.setEnabled(True)
        self.form.ui.tabWidget.setCurrentIndex(4)
        self.form.accept()
#         self.form.accept()

#         self.form.viewer.sheetFileName    = QtCore.QString('groel-segment.seq')
#         self.volumeViewer.load(self.args.volume)
#         self.form.modelLoaded()
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
