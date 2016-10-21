import os
import sys

from PyQt4 import QtGui

from .explorer.skeleton_viewer import SkeletonViewer
from .calpha.sequence_dock import CAlphaSequenceDock
from .calpha.viewer import CAlphaViewer
from .explorer import Camera
from .explorer.volume_viewer import VolumeViewer
from ..toolkit.sse.correspondence.StructurePrediction import StructurePrediction


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
        
        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        self.calpha = CAlphaViewer(self)
        self.viewers['skeleton'] = self.skeleton
        self.viewers['calpha'] = self.calpha
        
        self.scene = []
        self.scene.append(self.volume)
        self.scene.append(self.skeleton)
        self.scene.append(self.calpha)
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
#         exit()
        
    def load(self):
        self.volume.load(self.args.volume)
        self.skeleton.load(self.args.skeleton)
        self.calpha.loadData(self.args.calpha)
        print "self.calpha.main_chain:\n", self.calpha.main_chain
        print "self.structPred.chain\n", self.structPred.chain
        self.structPred.chain = self.calpha.main_chain 
        CAlphaSequenceDock.changeDockVisibility(self, self.calpha, self.structPred, self.structPred.chain)

    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
