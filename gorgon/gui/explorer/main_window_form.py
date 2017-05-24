from ..window import Window
from .volume_viewer import VolumeViewer
from .skeleton_viewer import SkeletonViewer
from ..sse.viewer import SSEViewer
from ..calpha.viewer import CAlphaViewer
from ...toolkit.sse.correspondence.StructurePrediction import StructurePrediction


class MainWindowForm(Window):

    def __init__(self, args):
        super(MainWindowForm, self).__init__(args, None)
        if args.volume != "none":
                self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        self.sse    = SSEViewer(self)
        
        self.hasSemiAtomicPlacementForm = False
        self.structPred = StructurePrediction.load(self.args.calpha, self)
        self.calpha = CAlphaViewer(self)
        if args.calpha2 != None:
            self.calpha2 = CAlphaViewer(self)
        if args.volume != "none":
            if args.calpha2 != None:
                self.mainCamera.append_scenes([self.skeleton, self.volume, self.sse, self.calpha, self.calpha2])
            else:
                self.mainCamera.append_scenes([self.skeleton, self.volume, self.sse, self.calpha])
        else:
            if args.calpha2 != None:
                self.mainCamera.append_scenes([self.skeleton, self.sse, self.calpha, self.calpha2])
            else:
                self.mainCamera.append_scenes([self.skeleton, self.sse, self.calpha])

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon Explorer"
        parser.add_argument('volume')
        parser.add_argument('skeleton')
        parser.add_argument('sequence')
        parser.add_argument('helix')
        parser.add_argument('calpha')
        parser.add_argument('--calpha2', action="store")

    def load(self):
        super(MainWindowForm, self).load()
        if self.args.volume != "none":
            self.volume.load(self.args.volume)
        self.skeleton.skeletonFile = self.args.skeleton
        self.skeleton.loadFile2()
        self.sse.loadHelixDataFromFile(self.args.helix)
        self.calpha.loadData(self.args.calpha)
        if self.args.calpha2 != None:
            self.calpha2.loadData(self.args.calpha2)