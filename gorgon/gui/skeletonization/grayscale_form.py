from form import Form
from ui_grayscale import Ui_DialogVolumeGrayscaleSkeletonization


class VolumeGrayscaleSkeletonizationForm(Form):

    def __init__(self, parent):
        self.citationHtml = "Segmentation-free skeletonization of grayscale volumes for shape understanding", "Sasakthi Abeysinghe, Matthew Baker, Wah Chiu and Tao Ju", "IEEE International Conference on Shape Modeling and Applications, 2008 (Accepted)"
        super(VolumeGrayscaleSkeletonizationForm, self).__init__(parent, Ui_DialogVolumeGrayscaleSkeletonization, "Grayscale")

    def getStepCount(self):
        return self.ui.horizontalSliderStepCount.value()
    
    def getCurveRadius(self):
        return self.ui.spinBoxCurveRadius.value()
    
    def getSurfaceRadius(self):
        return self.ui.spinBoxSurfaceRadius.value()
    
    def getSkeletonRadius(self):
        return self.ui.spinBoxSkeletonRadius.value()

    def skeletonize(self):
        skeleton = self.volume.renderer.performGrayscaleSkeletonizationAbeysinghe2008(self.getDensityThreshold(),
                                                                                      self.getStepCount(),
                                                                                      self.getMinCurveLength(),
                                                                                      self.getMinSurfaceSize(),
                                                                                      self.getCurveRadius(),
                                                                                      self.getSurfaceRadius(),
                                                                                      self.getSkeletonRadius())
        return skeleton
