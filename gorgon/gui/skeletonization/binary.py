from ..window import Window
from ..explorer.volume_viewer import VolumeViewer
from ..explorer.skeleton_viewer import SkeletonViewer
from ..skeletonization.binary_form import VolumeBinarySkeletonizationForm


class BinarySkeletonization(Window):

    def __init__(self, args):
        super(BinarySkeletonization, self).__init__(args, VolumeBinarySkeletonizationForm)

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon Binary Skeletonization Gui"
        parser.add_argument('volume')

    def load(self):
        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)
        self.form.volume = self.volume
        self.form.skeleton = self.skeleton
        self.form.createUI()
    
        self.mainCamera.append_scenes([self.skeleton, self.volume])
        
        self.volume.load(self.args.volume)
        
        super(BinarySkeletonization, self).load()
