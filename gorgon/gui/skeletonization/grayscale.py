from ..window import Window
from ..skeletonization.grayscale_form import VolumeGrayscaleSkeletonizationForm


class GrayscaleSkeletonization(Window):

    def __init__(self, version, args):
        super(GrayscaleSkeletonization, self).__init__(version, VolumeGrayscaleSkeletonizationForm, args)
