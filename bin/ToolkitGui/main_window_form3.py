from PyQt4 import QtCore, QtGui
import sys, os

from Explorer import Camera
from Explorer.volume_viewer import VolumeViewer
from Explorer.skeleton_viewer import SkeletonViewer
from ToolkitGui.sse.volume_builder_form import VolumeSSEBuilderForm
from ToolkitGui.calpha.viewer import CAlphaViewer
from ToolkitGui.sse.viewer import SSEViewer
from Explorer import Grid3D
from Toolkit.sse.correspondence.StructurePrediction import StructurePrediction


class MainWindowForm3(QtGui.QMainWindow):

    def __init__(self, version, args):
        super(MainWindowForm3, self).__init__()
        
        self.args = args
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        self.docks = []
        
        self.structPred = StructurePrediction.load(self.args.sequence, self)

        self.viewers = {}
        
        self.volumeViewer = VolumeViewer(self)
        self.skeletonViewer = SkeletonViewer(self)
        self.calphaViewer = CAlphaViewer(self)
        self.sseViewer = SSEViewer(self)
        
        self.scene = []
        self.scene.append(self.volumeViewer)
        self.scene.append(self.skeletonViewer)
        self.scene.append(self.calphaViewer)
        self.scene.append(self.sseViewer)
        
#         for i in xrange(-100, 101, 50):
#             self.scene.append(Grid3D(self, (i, -100, i), (i, 100, i)))
#
#         for i in xrange(-100, 101, 50):
#             self.scene.append(Grid3D(self, (-100, i, -100), (100, i, 100)))
        
        self.mainCamera = Camera(self.scene, self)
        self.setCentralWidget(self.mainCamera)
        
        self.form = VolumeSSEBuilderForm(self, self.volumeViewer, self.skeletonViewer, self.args)
        self.form.show()
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def load(self):
        self.volumeViewer.load(self.args.volume)
        self.skeletonViewer.load(self.args.skeleton)
        # self.calphaViewer.loadSSEHunterData('pseudoatoms.pdb')
        self.form.modelLoaded()
#         self.volumeViewer.renderer.printVertices()
        minDensity = self.volumeViewer.renderer.getMinDensity()
        maxDensity = self.volumeViewer.renderer.getMaxDensity()
        defaultDensity = (minDensity + maxDensity) / 2
        self.form.lineEditMin.setText("%.2f" % minDensity)
        self.form.lineEditMax.setText("%.2f" % maxDensity)
        self.form.lineEditMean.setText("%.2f" % defaultDensity)
        self.form.lineEditMin.setReadOnly(True)
        self.form.lineEditMax.setReadOnly(True)
        self.form.lineEditMean.setReadOnly(True)
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
