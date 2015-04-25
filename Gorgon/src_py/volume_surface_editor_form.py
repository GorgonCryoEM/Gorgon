# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to alter the surface properties of a volume 

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/volume_surface_editor_form.py,v $
#   $Revision: 1.23 $
#   $Date: 2011/08/28 01:46:18 $
#   $Author: coleman.r $
#   $State: Exp $
#
# History Log: 
#   $Log: volume_surface_editor_form.py,v $
#   Revision 1.23  2011/08/28 01:46:18  coleman.r
#   changing the display radius from a slider to a spinbox, saving screen space (good for laptops)
#
#   Revision 1.22  2010/01/17 18:34:59  ssa1
#   Histogram for density visualization
#
#   Revision 1.21  2010/01/16 16:14:23  ssa1
#   Disabling wireframe and flat shading with solid and cross section rendering modes (BugID: 64)
#
#   Revision 1.20  2009/12/24 05:09:30  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior
#
#   Revision 1.19  2009/07/01 22:00:27  ssa1
#   Centering the volume cropped using a radius around the point selected by the atom selection tool.
#
#   Revision 1.18  2009/03/24 16:22:27  ssa1
#   Better cross section & Solid Rendering viewing
#
#   Revision 1.17  2009/03/24 15:18:15  ssa1
#   Better cross section & Solid Rendering viewing
#
#   Revision 1.16  2008/12/02 05:34:16  ssa1
#   editable iso level
#
#   Revision 1.15  2008/12/02 05:13:07  ssa1
#   allowing manual edit of iso surface
#
#   Revision 1.14  2008/12/01 21:54:11  ssa1
#   Improving performance when changing viewing type
#
#   Revision 1.13  2008/09/03 19:53:20  ssa1
#   First loading the model, and then changing the viewing position
#
#   Revision 1.12  2008/09/03 19:48:19  ssa1
#   Maximizing performance of volume visualization by minimizing method call count
#
#   Revision 1.11  2008/09/03 18:41:07  ssa1
#   Setting a display radius for surfaces
#
#   Revision 1.10  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from ui_dialog_volume_surface_editor import Ui_DialogVolumeSurfaceEditor
from delayed_filter import DelayedFilter
from base_dock_widget import BaseDockWidget
from histogram_slider_widget import HistogramSliderWidget
import threading

class VolumeSurfaceEditorForm(BaseDockWidget):
    ViewingTypeIsoSurface, ViewingTypeCrossSection, ViewingTypeSolid = range(3)
    
    def __init__(self, main, volumeViewer, parent=None):
        BaseDockWidget.__init__(self, 
                                main, 
                                "&Volume - Surface Editor", 
                                "Modify the volume surface", 
                                "show_VolumeSurfaceEditor", 
                                "window-VolumeSurfaceEditor", 
                                "window", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.BottomDockWidgetArea,
                                parent)
        self.app = main
        self.viewer = volumeViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoadedPreDraw()"), self.modelLoadedPreDraw)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.createUI()
        self.createActions()
        self.createMenus()

    def createUI(self):
        self.ui = Ui_DialogVolumeSurfaceEditor()
        self.ui.setupUi(self)       
 
        self.filterIsoValue = DelayedFilter(self.thread())
        self.filterIsoValueMax = DelayedFilter(self.thread())
        self.filterDisplayRadius = DelayedFilter(self.thread())
        self.ui.labelIsoLevelMax.setVisible(False)
        self.ui.doubleSpinBoxDensityMax.setVisible(False)
        
        self.ui.histogram.setSliderType(HistogramSliderWidget.HistogramSliderTypeValue)
        
        self.connect(self.ui.histogram,QtCore.SIGNAL("lowerValueChanged(float)"),self.isoValueIndicatorChanged)
        self.connect(self.ui.histogram,QtCore.SIGNAL("higherValueChanged(float)"),self.isoValueMaxIndicatorChanged)
        self.connect(self.ui.histogram, QtCore.SIGNAL("widgetResized()"), self.histogramResized)
        self.connect(self.ui.histogram,QtCore.SIGNAL("lowerValueChanged(float)"),self.filterIsoValue.setValue)
        self.connect(self.ui.histogram,QtCore.SIGNAL("higherValueChanged(float)"),self.filterIsoValueMax.setValue)
        
        self.connect(self.ui.comboBoxSamplingInterval, QtCore.SIGNAL("currentIndexChanged(int)"), self.samplingChanged)
        self.connect(self.ui.spinBoxDisplayRadius, QtCore.SIGNAL("valueChanged(int)"),self.filterDisplayRadius.setValue)
        self.connect(self.filterIsoValue, QtCore.SIGNAL("valueChanged(float)"), self.isoValueChanged )
        self.connect(self.filterIsoValueMax, QtCore.SIGNAL("valueChanged(float)"), self.isoValueMaxChanged )
        self.connect(self.filterDisplayRadius, QtCore.SIGNAL("valueChanged(float)"), self.displayRadiusChanged )
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
        self.filterIsoValue.enabled = False
        self.filterDisplayRadius.enabled = False        
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
        maxRadius = int(max(self.viewer.renderer.getMax(0)/2, self.viewer.renderer.getMax(1)/2, self.viewer.renderer.getMax(2)/2));        
        self.ui.spinBoxDisplayRadius.setMaximum(maxRadius)
        self.ui.spinBoxDisplayRadius.setValue(maxRadius)
        self.viewer.renderer.setSampleInterval(self.getSamplingValue())
        self.viewer.renderer.setSurfaceValue(defaultDensity)
        self.viewer.renderer.setDisplayRadius(maxRadius)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setChecked(True)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setEnabled(True)
        self.showWidget(True)
        self.filterIsoValue.enabled = True
        self.filterDisplayRadius.enabled = True
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
        self.ui.histogram.setHistogram(histogramData, volume.getMin(), volume.getMax())
        
    def histogramResized(self):
        self.populateHistogram()
        
    
    def modelUnloaded(self):
        self.viewer.renderer.enableDraw(False)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setEnabled(False)
        self.showWidget(False)            
        self.ui.histogram.clearData()
        
        
    def createActions(self):               
        self.displayAct.setEnabled(False)
  
    def createMenus(self):
        pass
    
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
        self.viewer.emitModelChanged()           
        
    def updateIsoValueMax(self, newLevel):        
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.setMaxSurfaceValue(newLevel)
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.emitModelChanged()        
    
    def getSamplingValue(self):
        return int(self.ui.comboBoxSamplingInterval.itemText(self.ui.comboBoxSamplingInterval.currentIndex()))
    
    def samplingChanged(self, ix):
        self.viewer.renderer.setSampleInterval(self.getSamplingValue())
        self.viewer.emitModelChanged()
        
    def displayRadiusChanged(self, newRadius):
        newRadius = int(round(newRadius))
        self.viewer.renderer.setDisplayRadius(newRadius)
        self.viewer.emitModelChanged()
        
    def displayRadiusEnabled(self, enabled):
        self.viewer.renderer.useDisplayRadius(enabled)
        self.viewer.emitModelChanged()
        