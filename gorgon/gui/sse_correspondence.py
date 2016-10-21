import os
import sys

from PyQt4 import QtGui

from .explorer.skeleton_viewer import SkeletonViewer
from .calpha.viewer import CAlphaViewer
from .sse.helix_correspondence_finder_form import SSEHelixCorrespondenceFinderForm
from .sse.viewer import SSEViewer
from .explorer import Camera
from ..toolkit.sse.correspondence.StructurePrediction import StructurePrediction
from .window import Window


class SSECorrespondence(Window):

    def __init__(self, version, args):
        super(SSECorrespondence, self).__init__(version, SSEHelixCorrespondenceFinderForm, args)
        
        self.hasSemiAtomicPlacementForm = False

        self.structPred = StructurePrediction.load(self)
        
        self.viewers = {}
        
#         self.volumeViewer = VolumeViewer(self)
        self.sse    = SSEViewer(self)
        self.calpha = CAlphaViewer(self)
#         self.viewers['volume'] = self.volumeViewer

        self.scene.append(self.sse)
        self.scene.append(self.calpha)
        
        self.form.createUI()

    def load(self):
#         self.form.show()
#
#         self.volumeViewer.load(self.args.volume)
        self.skeleton.load(self)
        self.sse.loadHelixDataFromFile(self.args.helix)
        # self.calpha.loadSeq(self.args.sequence)

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
        # self.form.accept()

        # self.form.create_all_helices()

#         self.form.viewer.sheetFileName    = QtCore.QString('groel-segment.seq')
#         self.volumeViewer.load(self.args.volume)
#         self.form.modelLoaded()
