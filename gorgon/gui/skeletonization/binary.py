from ..window import Window
from ..skeletonization.binary_form import VolumeBinarySkeletonizationForm


class BinarySkeletonization(Window):

    def __init__(self, args):
        super(BinarySkeletonization, self).__init__(args, VolumeBinarySkeletonizationForm)

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon Binary Skeletonization Gui"
        parser.add_argument('volume')
