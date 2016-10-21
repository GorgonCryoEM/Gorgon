from ..window import Window
from ..skeletonization.grayscale_form import VolumeGrayscaleSkeletonizationForm


class GrayscaleSkeletonization(Window):

    def __init__(self, args):
        super(GrayscaleSkeletonization, self).__init__(args, VolumeGrayscaleSkeletonizationForm)

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon GrayScale Skeletonization Gui"
        parser.add_argument('volume')
