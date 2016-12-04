from .explorer.volume_viewer import VolumeViewer
from .explorer.skeleton_viewer import SkeletonViewer
from .calpha.sequence_dock import CAlphaSequenceDock
from .calpha.viewer import CAlphaViewer
from .explorer import Camera
from ..toolkit.sse.correspondence.StructurePrediction import StructurePrediction
from .window import Window
from .explorer.grid3d import Grid3D


class CalphaAtomPlacement(Window):

    def __init__(self, args):
        super(CalphaAtomPlacement, self).__init__(args, None)

        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        
        self.hasSemiAtomicPlacementForm = True

        self.structPred = StructurePrediction.load(self.args.calpha, self)
        
        self.calpha = CAlphaViewer(self)
        
        self.mainCamera.append_scenes([self.volume, self.skeleton, self.calpha, Grid3D(self, [0,-100,0], [0,100,0])])
        
    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon Calpha Atom Placement Gui"
        parser.add_argument('volume')
        parser.add_argument('skeleton')
        parser.add_argument('calpha')

    def load(self):
        self.volume.load(self.args.volume)
        self.skeleton.load(self.args.skeleton)
        self.calpha.loadData(self.args.calpha)
        print "self.calpha.main_chain:\n", self.calpha.main_chain
        print "self.structPred.chain\n", self.structPred.chain
        self.structPred.chain = self.calpha.main_chain 
        CAlphaSequenceDock.changeDockVisibility(self, self.calpha, self.structPred, self.structPred.chain)

        super(CalphaAtomPlacement, self).load()
