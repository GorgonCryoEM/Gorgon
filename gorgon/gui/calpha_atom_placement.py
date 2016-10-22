from .explorer.skeleton_viewer import SkeletonViewer
from .calpha.sequence_dock import CAlphaSequenceDock
from .calpha.viewer import CAlphaViewer
from .explorer import Camera
from .explorer.volume_viewer import VolumeViewer
from ..toolkit.sse.correspondence.StructurePrediction import StructurePrediction
from .window import Window


class CalphaAtomPlacement(Window):

    def __init__(self, args):
        super(CalphaAtomPlacement, self).__init__(args, None)
        
        self.hasSemiAtomicPlacementForm = True

        self.structPred = StructurePrediction.load(self.args.calpha, self)
        
        self.calpha = CAlphaViewer(self)
        
        self.scene.append(self.calpha)
        
    def load(self):
        self.volume.load(self.args.volume)
        self.skeleton.load(self.args.skeleton)
        self.calpha.loadData(self.args.calpha)
        print "self.calpha.main_chain:\n", self.calpha.main_chain
        print "self.structPred.chain\n", self.structPred.chain
        self.structPred.chain = self.calpha.main_chain 
        CAlphaSequenceDock.changeDockVisibility(self, self.calpha, self.structPred, self.structPred.chain)
