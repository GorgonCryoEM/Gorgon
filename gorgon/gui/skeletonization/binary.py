from .window import Window
from ..skeletonization.binary_form import VolumeBinarySkeletonizationForm


class BinarySkeletonization(Window):

    def __init__(self, version, args):
        super(BinarySkeletonization, self).__init__(version, VolumeBinarySkeletonizationForm, args)
