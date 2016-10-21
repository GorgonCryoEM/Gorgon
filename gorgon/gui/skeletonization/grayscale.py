from ..window import Window
from ..skeletonization.grayscale_form import VolumeGrayscaleSkeletonizationForm


class GrayscaleSkeletonization(Window):

    def __init__(self, args):
        super(GrayscaleSkeletonization, self).__init__(VolumeGrayscaleSkeletonizationForm, args)
