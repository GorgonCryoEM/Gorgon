from form import Form
from ui_grayscale import Ui_DialogVolumeGrayscaleSkeletonization


class VolumeGrayscaleSkeletonizationForm(Form):

    def __init__(self, parent):
        super(VolumeGrayscaleSkeletonizationForm, self).__init__(parent, Ui_DialogVolumeGrayscaleSkeletonization, "Grayscale")

    def getStepCount(self):
        return self.ui.horizontalSliderStepCount.value()
    
    def getCurveRadius(self):
        return self.ui.spinBoxCurveRadius.value()
    
    def getSurfaceRadius(self):
        return self.ui.spinBoxSurfaceRadius.value()
    
    def getSkeletonRadius(self):
        return self.ui.spinBoxSkeletonRadius.value()
    
    def accept(self):
        if(self.volume.loaded):
            self.setCursor(QtCore.Qt.BusyCursor)
            method = self.getSkeletonizationMethod()
            if(method == 0):
                self.modelLoaded()
                skeleton = self.volume.renderer.performGrayscaleSkeletonizationAbeysinghe2008(self.getDensityThreshold(), self.getStepCount(), self.getMinCurveLength(), self.getMinSurfaceSize(), self.getCurveRadius(), self.getSurfaceRadius(), self.getSkeletonRadius())
                self.skeleton.loadVolume(skeleton)
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.close()
        else:
            QtGui.QMessageBox.critical(None, "Source volume unloaded", "A volume must be loaded to perform skeletonization", QtGui.QMessageBox.Ok, QtGui.QMessageBox.NoButton)
#         BaseDialogWidget.accept(self)
    
    def loadAndShow(self):
        self.ui.checkBoxPreserveSkeleton.setEnabled(self.parent.viewers["skeleton"].loaded)
        
    def methodChanged(self, id):
        if(id == 0):
            self.ui.textCitation.setHtml(self.getCitationHtml("Segmentation-free skeletonization of grayscale volumes for shape understanding", "Sasakthi Abeysinghe, Matthew Baker, Wah Chiu and Tao Ju", "IEEE International Conference on Shape Modeling and Applications, 2008 (Accepted)"))
        else:
            self.ui.textCitation.setHtml("")
