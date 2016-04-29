from PyQt4 import QtCore, QtGui
from ui_dialog_volume_binary_skeletonization import Ui_DialogVolumeBinarySkeletonization


class VolumeBinarySkeletonizationForm(QtGui.QDialog):

    def __init__(self, main, volumeViewer, parent=None):
        super(VolumeBinarySkeletonizationForm, self).__init__(volumeViewer)
        self.app = main
        self.viewer = volumeViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.createUI()
        self.createActions()

    def createUI(self):
        self.ui = Ui_DialogVolumeBinarySkeletonization()
        self.ui.setupUi(self)
        self.connect(self.ui.horizontalSliderIsoLevel,QtCore.SIGNAL("valueChanged(int)"),self.isoValueChanged)
        self.connect(self.ui.comboBoxMethod, QtCore.SIGNAL("currentIndexChanged (int)"), self.methodChanged)
        self.methodChanged(0)
        
    def createActions(self):
        self.displayAct.setEnabled(False)
        self.connect(self.displayAct, QtCore.SIGNAL("triggered()"), self.loadSelf)
        
    def modelLoaded(self):
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderIsoLevel.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderIsoLevel.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        self.ui.horizontalSliderIsoLevel.setValue(defaultDensity)
        self.displayAct.setEnabled(True)
    
    def modelUnloaded(self):
        self.displayAct.setEnabled(False)
        self.close()

    def isoValueChanged(self, newLevel):
        self.ui.labelIsoValueDisplay.setNum(newLevel/100.0)

    def getDensityThreshold(self):
        return self.ui.horizontalSliderIsoLevel.value()/100.0
    
    def getSkeletonizationMethod(self):
        return self.ui.comboBoxMethod.currentIndex()
    
    def getMinCurveLength(self):
        return self.ui.spinBoxMinCurve.value()

    def getMinSurfaceSize(self):
        return self.ui.spinBoxMinSurface.value()
          
    def accept(self):
        print "binary.... accept"
        print self.viewer.loaded
        if(self.viewer.loaded):
            self.setCursor(QtCore.Qt.BusyCursor)
            method = self.getSkeletonizationMethod()
            print method
            if(method == 0):
                self.modelLoaded()
                print "       Values:", self.getDensityThreshold(), self.getMinCurveLength(), self.getMinSurfaceSize()
                skeleton = self.viewer.renderer.performBinarySkeletonizationJu2007(self.getDensityThreshold(), self.getMinCurveLength(), self.getMinSurfaceSize())
                print "Skeleton after skeletonization: ", skeleton.getSize()
                self.app.skeleton.loadVolume(skeleton)
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.close()
        else:
            QtGui.QMessageBox.critical(None, "Source volume unloaded", "A volume must be loaded to perform skeletonization", QtGui.QMessageBox.Ok, QtGui.QMessageBox.NoButton)
        super(VolumeBinarySkeletonizationForm, self).accept()

    def loadSelf(self):
        self.ui.horizontalSliderIsoLevel.setValue(int(self.viewer.getIsoValue()*100))
        
    def getCitationHtml(self, title, author, journal):
        return "<b>" + title + "</b><br>" + author + "<br><i>" + journal + "</i>"
                  
    def methodChanged(self, id):
        if(id == 0):
            self.ui.textCitation.setHtml(self.getCitationHtml("Computing a family of skeletons of volumetric models for shape description", "Tao Ju, Matthew Baker and Wah Chiu", "Computer Aided Design 39(5) 2007"))
        else:
            self.ui.textCitation.setHtml("")
