from PyQt4 import QtCore, QtGui
from ui_dialog_volume_surface_editor import Ui_DialogVolumeSurfaceEditor
from .slider_widget import HistogramSliderWidget
# import threading


class VolumeSurfaceEditorForm(QtGui.QWidget):
    
    def __init__(self, app):
    
        QtGui.QWidget.__init__(self, app)
        
        
        self.createUI()
        
    def setViewer(self, viewer):
        self.viewer = viewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoadedPreDraw()"), self.modelLoadedPreDraw)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)

    def toggleViewAction(self):
        return self.dock.toggleViewAction()

    def createUI(self):
        self.ui = Ui_DialogVolumeSurfaceEditor()
        self.ui.setupUi(self)
 
        self.ui.histogram.setSliderType(HistogramSliderWidget.HistogramSliderTypeValue)
        
        self.connect(self.ui.histogram,QtCore.SIGNAL("lowerValueChanged(float)"),self.isoValueIndicatorChanged)
        self.connect(self.ui.histogram, QtCore.SIGNAL("widgetResized()"), self.histogramResized)
        
        self.connect(self.ui.comboBoxSamplingInterval, QtCore.SIGNAL("currentIndexChanged(int)"), self.samplingChanged)
        self.connect(self.ui.doubleSpinBoxDensity, QtCore.SIGNAL("editingFinished ()"), self.manualValueChanged)
        self.connect(self.ui.checkBoxUseRadius, QtCore.SIGNAL("toggled(bool)"), self.displayRadiusEnabled)
        self.connect(self.ui.histogram, QtCore.SIGNAL("valueChanged(float)") , self.updateIsoValue)
        
    def modelLoadedPreDraw(self):
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.populateHistogram()
        self.ui.doubleSpinBoxDensity.setMinimum(minDensity)
        self.ui.doubleSpinBoxDensity.setMaximum(maxDensity)
        defaultDensity = (minDensity + maxDensity) / 2
        self.ui.histogram.setLowerValue(defaultDensity)
        self.ui.histogram.setHigherValue(maxDensity)
        maxRadius = int(max(self.viewer.renderer.getMaxPos(0)/2, self.viewer.renderer.getMaxPos(1)/2, self.viewer.renderer.getMaxPos(2)/2));
        self.ui.spinBoxDisplayRadius.setMaximum(maxRadius)
        self.ui.spinBoxDisplayRadius.setValue(maxRadius)
        self.viewer.renderer.setSampleInterval(self.getSamplingValue())
        self.viewer.renderer.setSurfaceValue(defaultDensity)
        self.viewer.renderer.setDisplayRadius(maxRadius)
        
    def populateHistogram(self):
        binCount = self.ui.histogram.width() - 2*self.ui.histogram.verticalBorderSize
        self.ui.histogram.setBinCount(binCount)
        self.ui.histogram.setUseLogScale(True)
        if hasattr(self, 'viewer'):
            volume = self.viewer.renderer.getVolume()
            volume.buildHistogram(binCount)
            histogramData = []
            self.ui.histogram.clearData()
            for i in range(binCount):
                histogramData.append(volume.getHistogramBinValue(i))
            self.ui.histogram.setHistogram(histogramData, volume.getMinDensity(), volume.getMaxDensity())
        
    def histogramResized(self):
        self.populateHistogram()
        
    def modelUnloaded(self):
        self.viewer.renderer.enableDraw(False)
        self.displayAct.setEnabled(False)
        self.showWidget(False)
        self.ui.histogram.clearData()
        
    def isoValueIndicatorChanged(self, newValue):
        self.ui.doubleSpinBoxDensity.setValue(float(newValue))
        
    def manualValueChanged(self):
        newValue = self.ui.doubleSpinBoxDensity.value()
        self.ui.histogram.setLowerValue(newValue)
        
    def updateIsoValue(self, newLevel):
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.setSurfaceValue(newLevel)
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.modelChanged()
        
    def getSamplingValue(self):
        return int(self.ui.comboBoxSamplingInterval.itemText(self.ui.comboBoxSamplingInterval.currentIndex()))
    
    def samplingChanged(self, ix):
        self.viewer.renderer.setSampleInterval(self.getSamplingValue())
        self.viewer.modelChanged()
        
    def displayRadiusChanged(self, newRadius):
        newRadius = int(round(newRadius))
        self.viewer.renderer.setDisplayRadius(newRadius)
        self.viewer.modelChanged()
        
    def displayRadiusEnabled(self, enabled):
        self.viewer.renderer.useDisplayRadius(enabled)
        self.viewer.modelChanged()
        
