from form import Form
from ui_binary import Ui_DialogVolumeBinarySkeletonization


class VolumeBinarySkeletonizationForm(Form):

    def __init__(self, parent):
        self.citationHtml = "Computing a family of skeletons of volumetric models for shape description", "Tao Ju, Matthew Baker and Wah Chiu", "Computer Aided Design 39(5) 2007"
        super(VolumeBinarySkeletonizationForm, self).__init__(parent, Ui_DialogVolumeBinarySkeletonization, "Binary")

    def skeletonize(self):
        return self.volume.renderer.performBinarySkeletonizationJu2007(self.getDensityThreshold(),
                                                                       self.getMinCurveLength(),
                                                                       self.getMinSurfaceSize())
