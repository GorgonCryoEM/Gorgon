from PyQt4 import QtCore, QtGui
import sys, os

from Explorer import Camera
from Explorer.volume_viewer import VolumeViewer
from Explorer.skeleton_viewer import SkeletonViewer
from ToolkitGui.sse.helix_correspondence_finder_form import SSEHelixCorrespondenceFinderForm
from ToolkitGui.sse.viewer import SSEViewer
from Toolkit.sse.correspondence.StructurePrediction import StructurePrediction
from ToolkitGui.calpha.viewer import CAlphaViewer
from ToolkitGui.calpha.sequence_dock import CAlphaSequenceDock


class CalphaAtomPlacement(QtGui.QMainWindow):

    def __init__(self, version, args):
        super(CalphaAtomPlacement, self).__init__()
        
        self.args = args
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        self.docks = []
        self.hasSemiAtomicPlacementForm = True

        self.structPred = StructurePrediction.load(self.args.calpha, self)
        # exit()
        
        self.viewers = {}
        
        self.volumeViewer = VolumeViewer(self)
        self.skeletonViewer = SkeletonViewer(self)
        # self.sseViewer    = SSEViewer(self)
        self.calphaViewer = CAlphaViewer(self)
#         self.viewers['volume'] = self.volumeViewer
        self.viewers['skeleton'] = self.skeletonViewer
        # self.viewers['sse'] = self.sseViewer
        self.viewers['calpha'] = self.calphaViewer
        
        self.scene = []
        self.scene.append(self.volumeViewer)
        self.scene.append(self.skeletonViewer)
        # self.scene.append(self.sseViewer)
        self.scene.append(self.calphaViewer)
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
#         exit()
        
    def load(self):
        self.volumeViewer.load(self.args.volume)
        self.skeletonViewer.load(self.args.skeleton)
        # self.sseViewer.loadHelixDataFromFile(self.args.helix)
        self.calphaViewer.loadData(self.args.calpha)
        print "self.calphaViewer.main_chain:\n", self.calphaViewer.main_chain
        print "self.structPred.chain\n", self.structPred.chain
        # exit()
        self.structPred.chain = self.calphaViewer.main_chain 
        CAlphaSequenceDock.changeDockVisibility(self, self.calphaViewer, self.structPred, self.structPred.chain)

    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
