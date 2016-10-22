import os
import sys

from PyQt4 import QtCore, QtGui

from .explorer.skeleton_viewer import SkeletonViewer
from .calpha.viewer import CAlphaViewer
from .sse.viewer import SSEViewer
from .sse.volume_builder_form import VolumeSSEBuilderForm
from .explorer import Camera
from .explorer.volume_viewer import VolumeViewer
from .window import Window


class SSEIdentification(Window):

    def __init__(self, version, args):
        super(SSEIdentification, self).__init__(version, VolumeSSEBuilderForm, args)
        
        self.hasSemiAtomicPlacementForm = False
        
        self.structPred = None

        self.viewers = {}
        
        self.calpha = CAlphaViewer(self)
        self.sse = SSEViewer(self)
        
        self.scene.append(self.calpha)
        self.scene.append(self.sse)
        
        self.form.init_again()
        
#         for i in xrange(-100, 101, 50):
#             self.scene.append(Grid3D(self, (i, -100, i), (i, 100, i)))
#
#         for i in xrange(-100, 101, 50):
#             self.scene.append(Grid3D(self, (-100, i, -100), (100, i, 100)))
        
    def load(self):
        super(SSEIdentification, self).load()
        
        self.skeleton.load(self.args.skeleton)
        # self.calpha.loadSSEHunterData('pseudoatoms_thr_20.pdb')
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
