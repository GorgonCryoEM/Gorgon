from PyQt4 import QtCore, QtGui
from ui_dialog_volume_surface_editor import Ui_DialogVolumeSurfaceEditor
from .slider_widget import HistogramSliderWidget
# import threading


class VolumeSurfaceEditorForm(QtGui.QWidget):
    ViewingTypeIsoSurface, ViewingTypeCrossSection, ViewingTypeSolid = range(3)
    
    def __init__(self, app):
    
        QtGui.QWidget.__init__(self, app)
        
        dock = QtGui.QDockWidget("Volume Surface Editor", app)
        dock.setWidget(self)
        dock.setAllowedAreas(QtCore.Qt.AllDockWidgetAreas)
        app.addDockWidget(QtCore.Qt.BottomDockWidgetArea, dock)
        
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
 
        self.ui.labelIsoLevelMax.setVisible(False)
        self.ui.doubleSpinBoxDensityMax.setVisible(False)
        
        self.ui.histogram.setSliderType(HistogramSliderWidget.HistogramSliderTypeValue)
        
        self.connect(self.ui.histogram,QtCore.SIGNAL("lowerValueChanged(float)"),self.isoValueIndicatorChanged)
        self.connect(self.ui.histogram,QtCore.SIGNAL("higherValueChanged(float)"),self.isoValueMaxIndicatorChanged)
        self.connect(self.ui.histogram, QtCore.SIGNAL("widgetResized()"), self.histogramResized)
        
        self.connect(self.ui.comboBoxSamplingInterval, QtCore.SIGNAL("currentIndexChanged(int)"), self.samplingChanged)
        self.connect(self.ui.radioButtonIsoSurface, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        self.connect(self.ui.radioButtonCrossSection, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        self.connect(self.ui.radioButtonSolid, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        self.connect(self.ui.doubleSpinBoxDensity, QtCore.SIGNAL("editingFinished ()"), self.manualValueChanged)
        self.connect(self.ui.doubleSpinBoxDensityMax, QtCore.SIGNAL("editingFinished ()"), self.manualValueMaxChanged)
        self.connect(self.ui.checkBoxUseRadius, QtCore.SIGNAL("toggled(bool)"), self.displayRadiusEnabled)
        
    def setViewingType(self, toggled):
        if(toggled):
            if(self.ui.radioButtonIsoSurface.isChecked()):
                self.ui.labelIsoLevel.setText("Density Threshold:");
                self.viewer.renderer.setViewingType(self.ViewingTypeIsoSurface)
                self.viewer.visualizationOptions.ui.radioButtonFlat.setEnabled(True)
                self.viewer.visualizationOptions.ui.radioButtonWireframe.setEnabled(True)
                self.ui.histogram.setSliderType(HistogramSliderWidget.HistogramSliderTypeValue)

            elif self.ui.radioButtonCrossSection.isChecked():
                self.ui.labelIsoLevel.setText("Minimum Density:");
                self.viewer.renderer.setViewingType(self.ViewingTypeCrossSection)
                self.viewer.visualizationOptions.ui.radioButtonFlat.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonWireframe.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonSmooth.setChecked(True)
                self.ui.histogram.setSliderType(HistogramSliderWidget.HistogramSliderTypeRange)

            elif self.ui.radioButtonSolid.isChecked():
                self.ui.labelIsoLevel.setText("Minimum Density:");
                self.viewer.renderer.setViewingType(self.ViewingTypeSolid)
                
                self.viewer.visualizationOptions.ui.radioButtonFlat.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonWireframe.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonSmooth.setChecked(True)
                self.ui.histogram.setSliderType(HistogramSliderWidget.HistogramSliderTypeRange)
                
            print "setViewingType", QtCore.QThread.currentThreadId()
            self.viewer.emitModelChanged()
    
    def modelLoadedPreDraw(self):
        self.viewer.renderer.enableDraw(False)
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.populateHistogram()
        self.ui.doubleSpinBoxDensity.setMinimum(minDensity)
        self.ui.doubleSpinBoxDensity.setMaximum(maxDensity)
        self.ui.doubleSpinBoxDensityMax.setMinimum(minDensity)
        self.ui.doubleSpinBoxDensityMax.setMaximum(maxDensity)
        if(self.ui.radioButtonIsoSurface.isChecked()):
            defaultDensity = (minDensity + maxDensity) / 2
        else:
            defaultDensity = minDensity
        self.ui.histogram.setLowerValue(defaultDensity)
        self.ui.histogram.setHigherValue(maxDensity)
        maxRadius = int(max(self.viewer.renderer.getMaxPos(0)/2, self.viewer.renderer.getMaxPos(1)/2, self.viewer.renderer.getMaxPos(2)/2));
        self.ui.spinBoxDisplayRadius.setMaximum(maxRadius)
        self.ui.spinBoxDisplayRadius.setValue(maxRadius)
        self.viewer.renderer.setSampleInterval(self.getSamplingValue())
        self.viewer.renderer.setSurfaceValue(defaultDensity)
        self.viewer.renderer.setDisplayRadius(maxRadius)
        self.displayAct.setChecked(True)
        self.displayAct.setEnabled(True)
        self.showWidget(True)
        self.viewer.renderer.enableDraw(True)
        
    def populateHistogram(self):
        binCount = self.ui.histogram.width() - 2*self.ui.histogram.verticalBorderSize
        self.ui.histogram.setBinCount(binCount)
        self.ui.histogram.setUseLogScale(True)
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
        
        maxValue = float(max(newValue, self.ui.doubleSpinBoxDensityMax.value()));
        if(self.ui.doubleSpinBoxDensityMax.value() != maxValue):
            self.ui.doubleSpinBoxDensityMax.setValue(maxValue)
        
    def isoValueMaxIndicatorChanged(self, newValue):
        self.ui.doubleSpinBoxDensityMax.setValue(float(newValue))
        
        minValue = float(min(newValue, self.ui.doubleSpinBoxDensity.value()));
        if(self.ui.doubleSpinBoxDensity.value() != minValue):
            self.ui.doubleSpinBoxDensity.setValue(minValue)
                            
    def manualValueChanged(self):
        newValue = self.ui.doubleSpinBoxDensity.value()
        self.ui.histogram.setLowerValue(newValue)
        
    def manualValueMaxChanged(self):
        newValue = self.ui.doubleSpinBoxDensityMax.value()
        self.ui.histogram.setHigherValue(newValue)
                    
    def isoValueChanged(self, newLevel):
        #threading.Thread(target = self.updateIsoValue, args=(newLevel,)).start()
        self.updateIsoValue(newLevel)

    def isoValueMaxChanged(self, newLevel):
        #threading.Thread(target = self.updateIsoValue, args=(newLevel,)).start()
        self.updateIsoValueMax(newLevel)
        
    def updateIsoValue(self, newLevel):
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.setSurfaceValue(newLevel)
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.modelChanged()
        
    def updateIsoValueMax(self, newLevel):
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.setMaxSurfaceValue(newLevel)
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
        
