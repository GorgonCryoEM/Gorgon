from ..window import Window
from ..skeletonization.binary_form import VolumeBinarySkeletonizationForm


class BinarySkeletonization(Window):

    def __init__(self, args):
        super(BinarySkeletonization, self).__init__(VolumeBinarySkeletonizationForm, args)
