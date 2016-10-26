from ..window import Window
from ..explorer.volume_viewer import VolumeViewer
from ..explorer.skeleton_viewer import SkeletonViewer
from ..skeletonization.grayscale_form import VolumeGrayscaleSkeletonizationForm


class GrayscaleSkeletonization(Window):

    def __init__(self, args):
        super(GrayscaleSkeletonization, self).__init__(args, VolumeGrayscaleSkeletonizationForm)

        self.volume = VolumeViewer(self)
        self.skeleton = SkeletonViewer(self)

        self.mainCamera.append_scenes([self.skeleton, self.volume])

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon GrayScale Skeletonization Gui"
        parser.add_argument('volume')
