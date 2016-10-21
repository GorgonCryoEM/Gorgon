import os
import sys

from PyQt4 import QtCore, QtGui

from .explorer.skeleton_viewer import SkeletonViewer
from .calpha.viewer import CAlphaViewer
from .sse.viewer import SSEViewer
from .sse.volume_builder_form import VolumeSSEBuilderForm
from .explorer import Camera
from .explorer.volume_viewer import VolumeViewer


class SSEIdentification(QtGui.QMainWindow):

    def __init__(self, version, args):
        super(SSEIdentification, self).__init__()
        
        self.args = args
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        self.docks = []
        self.hasSemiAtomicPlacementForm = False
        
        self.structPred = None

        self.viewers = {}
        
        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        self.calphaViewer = CAlphaViewer(self)
        self.sseViewer = SSEViewer(self)
        
        self.scene = []
        self.scene.append(self.volume)
        self.scene.append(self.skeleton)
        self.scene.append(self.calphaViewer)
        self.scene.append(self.sseViewer)
        
#         for i in xrange(-100, 101, 50):
#             self.scene.append(Grid3D(self, (i, -100, i), (i, 100, i)))
#
#         for i in xrange(-100, 101, 50):
#             self.scene.append(Grid3D(self, (-100, i, -100), (100, i, 100)))
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.form = VolumeSSEBuilderForm(self, self.args, QtCore.Qt.LeftDockWidgetArea)
        self.form.show()
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def load(self):
        self.volume.load(self.args.volume)
        self.skeleton.load(self.args.skeleton)
        # self.calphaViewer.loadSSEHunterData('pseudoatoms_thr_20.pdb')
        self.form.modelLoaded()
#         self.volume.renderer.printVertices()
        minDensity = self.volume.renderer.getMinDensity()
        maxDensity = self.volume.renderer.getMaxDensity()
        defaultDensity = (minDensity + maxDensity) / 2
        self.form.lineEditMin.setText("%.2f" % minDensity)
        self.form.lineEditMax.setText("%.2f" % maxDensity)
        self.form.lineEditMean.setValue(defaultDensity)
        self.form.lineEditMean.valueChanged.connect(self.volume.renderer.setSurfaceValue)
        self.form.lineEditMean.valueChanged.connect(self.volume.modelChanged)
        self.form.lineEditMin.setReadOnly(True)
        self.form.lineEditMax.setReadOnly(True)
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
