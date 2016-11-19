from ..window import Window
from .volume_viewer import VolumeViewer
from .skeleton_viewer import SkeletonViewer
from ..sse.viewer import SSEViewer
from ..calpha.viewer import CAlphaViewer
from ...toolkit.sse.correspondence.StructurePrediction import StructurePrediction


class MainWindowForm(Window):

    def __init__(self, args):
        super(MainWindowForm, self).__init__(args, None)

        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        self.sse    = SSEViewer(self)
        
        self.hasSemiAtomicPlacementForm = False
        self.structPred = StructurePrediction.load(self.args.calpha, self)
        self.calpha = CAlphaViewer(self)

        self.mainCamera.append_scenes([self.skeleton, self.volume, self.sse, self.calpha])

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon Explorer"
        parser.add_argument('volume')
        parser.add_argument('skeleton')
        parser.add_argument('sequence')
        parser.add_argument('helix')
        parser.add_argument('calpha')

    def load(self):
        super(MainWindowForm, self).load()

        self.volume.load(self.args.volume)
        self.skeleton.load(self.args.skeleton)
        self.sse.loadHelixDataFromFile(self.args.helix)
        self.calpha.loadData(self.args.calpha)
