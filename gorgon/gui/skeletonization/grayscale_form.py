from PyQt4 import QtCore, QtGui
from ui_grayscale import Ui_DialogVolumeGrayscaleSkeletonization
import threading


class VolumeGrayscaleSkeletonizationForm(QtGui.QDialog):

    def __init__(self, parent):
        self.parent = parent
        self.volume = self.parent.volume
        self.skeleton = self.parent.skeleton
        super(VolumeGrayscaleSkeletonizationForm, self).__init__()
        dock = QtGui.QDockWidget("Grayscale", self.volume)
        dock.setWidget(self)
        dock.setAllowedAreas(QtCore.Qt.AllDockWidgetAreas)
        self.parent.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)
        self.connect(self.volume, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.volume, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.createUI()

    def createUI(self):
        self.ui = Ui_DialogVolumeGrayscaleSkeletonization()
        self.ui.setupUi(self)
        self.connect(self.ui.horizontalSliderStartingDensity,QtCore.SIGNAL("valueChanged(int)"),self.startingDensityChanged)
        self.connect(self.ui.comboBoxMethod, QtCore.SIGNAL("currentIndexChanged (int)"), self.methodChanged)
        self.methodChanged(0)
        
    def createMenus(self):
        self.parent.menus.addAction("actions-volume-skeletonization-grayscale", self.parent.actions.getAction("perform_VolumeGrayscaleSkeletonization"), "actions-volume-skeletonization")

    def modelLoaded(self):
        maxDensity = self.volume.renderer.getMaxDensity()
        minDensity = self.volume.renderer.getMinDensity()
        self.ui.horizontalSliderStartingDensity.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderStartingDensity.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        self.ui.horizontalSliderStartingDensity.setValue(defaultDensity)
        
    def modelUnloaded(self):
        self.close()

    def startingDensityChanged(self, newLevel):
        self.ui.labelStartingDensityDisplay.setNum(newLevel/100.0)

    def getStartingDensity(self):
        return self.ui.horizontalSliderStartingDensity.value()/100.0

    def getStepCount(self):
        return self.ui.horizontalSliderStepCount.value()
    
    def getSkeletonizationMethod(self):
        return self.ui.comboBoxMethod.currentIndex()
    
    def getMinCurveLength(self):
        return self.ui.spinBoxMinCurve.value()

    def getMinSurfaceSize(self):
        return self.ui.spinBoxMinSurface.value()
    
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
                skeleton = self.volume.renderer.performGrayscaleSkeletonizationAbeysinghe2008(self.getStartingDensity(), self.getStepCount(), self.getMinCurveLength(), self.getMinSurfaceSize(), self.getCurveRadius(), self.getSurfaceRadius(), self.getSkeletonRadius())
                self.skeleton.loadVolume(skeleton)
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.close()
        else:
            QtGui.QMessageBox.critical(None, "Source volume unloaded", "A volume must be loaded to perform skeletonization", QtGui.QMessageBox.Ok, QtGui.QMessageBox.NoButton)
#         BaseDialogWidget.accept(self)
    
    def getCitationHtml(self, title, author, journal):
        return "<b>" + title + "</b><br>" + author + "<br><i>" + journal + "</i>"
                  
    def loadAndShow(self):
        self.ui.checkBoxPreserveSkeleton.setEnabled(self.parent.viewers["skeleton"].loaded)
        
    def methodChanged(self, id):
        if(id == 0):
            self.ui.textCitation.setHtml(self.getCitationHtml("Segmentation-free skeletonization of grayscale volumes for shape understanding", "Sasakthi Abeysinghe, Matthew Baker, Wah Chiu and Tao Ju", "IEEE International Conference on Shape Modeling and Applications, 2008 (Accepted)"))
        else:
            self.ui.textCitation.setHtml("")
