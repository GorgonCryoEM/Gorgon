# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to alter the surface properties of a volume 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
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
        self.connect(self.ui.horizontalSliderIsoLevel,QtCore.SIGNAL("valueChanged(int)"),self.isoValueIndicatorChanged)
        self.connect(self.ui.horizontalSliderIsoLevelMax,QtCore.SIGNAL("valueChanged(int)"),self.isoValueMaxIndicatorChanged)
        self.filterIsoValue = DelayedFilter(self.thread())
        self.filterIsoValueMax = DelayedFilter(self.thread())
        self.filterSampling = DelayedFilter(self.thread())
        self.filterDisplayRadius = DelayedFilter(self.thread())
        self.ui.labelIsoLevelMax.setVisible(False)
        self.ui.horizontalSliderIsoLevelMax.setVisible(False)
        self.ui.doubleSpinBoxDensityMax.setVisible(False)
        self.connect(self.ui.horizontalSliderIsoLevel, QtCore.SIGNAL("valueChanged(int)"),self.filterIsoValue.setValue)
        self.connect(self.ui.horizontalSliderIsoLevelMax, QtCore.SIGNAL("valueChanged(int)"),self.filterIsoValueMax.setValue)
        self.connect(self.ui.horizontalSliderIsoLevel, QtCore.SIGNAL("valueChanged(int)"),self.updateOtherIsoValue)
        self.connect(self.ui.horizontalSliderIsoLevelMax, QtCore.SIGNAL("valueChanged(int)"),self.updateOtherIsoValueMax)
        
        self.connect(self.ui.horizontalSliderSampling, QtCore.SIGNAL("valueChanged(int)"),self.filterSampling.setValue)
        self.connect(self.ui.horizontalSliderDisplayRadius, QtCore.SIGNAL("valueChanged(int)"),self.filterDisplayRadius.setValue)
        self.connect(self.filterIsoValue, QtCore.SIGNAL("valueChanged(int)"), self.isoValueChanged )
        self.connect(self.filterIsoValueMax, QtCore.SIGNAL("valueChanged(int)"), self.isoValueMaxChanged )
        self.connect(self.filterSampling, QtCore.SIGNAL("valueChanged(int)"), self.samplingChanged )        
        self.connect(self.filterDisplayRadius, QtCore.SIGNAL("valueChanged(int)"), self.displayRadiusChanged )
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

            elif self.ui.radioButtonCrossSection.isChecked():
                self.ui.labelIsoLevel.setText("Minimum Density:");
                self.viewer.renderer.setViewingType(self.ViewingTypeCrossSection)
                self.viewer.visualizationOptions.ui.radioButtonFlat.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonWireframe.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonSmooth.setChecked(True)

            elif self.ui.radioButtonSolid.isChecked():
                self.ui.labelIsoLevel.setText("Minimum Density:");
                self.viewer.renderer.setViewingType(self.ViewingTypeSolid)
                
                self.viewer.visualizationOptions.ui.radioButtonFlat.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonWireframe.setEnabled(False)
                self.viewer.visualizationOptions.ui.radioButtonSmooth.setChecked(True)
                
            print "setViewingType", QtCore.QThread.currentThreadId()
            self.viewer.emitModelChanged()
    
    def modelLoadedPreDraw(self):
        self.viewer.renderer.enableDraw(False)
        self.filterIsoValue.enabled = False
        self.filterSampling.enabled = False
        self.filterDisplayRadius.enabled = False        
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderIsoLevel.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderIsoLevel.setMaximum(int(maxDensity*100))
        self.ui.horizontalSliderIsoLevelMax.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderIsoLevelMax.setMaximum(int(maxDensity*100))
        self.ui.doubleSpinBoxDensity.setMinimum(minDensity)
        self.ui.doubleSpinBoxDensity.setMaximum(maxDensity)        
        self.ui.doubleSpinBoxDensityMax.setMinimum(minDensity)
        self.ui.doubleSpinBoxDensityMax.setMaximum(maxDensity)
        if(self.ui.radioButtonIsoSurface.isChecked()):
            defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        else:
            defaultDensity = int(minDensity*100)
        self.ui.horizontalSliderIsoLevel.setValue(defaultDensity)
        self.ui.horizontalSliderIsoLevelMax.setValue(int(maxDensity*100.0))         
        maxRadius = int(max(self.viewer.renderer.getMax(0)/2, self.viewer.renderer.getMax(1)/2, self.viewer.renderer.getMax(2)/2));        
        self.ui.horizontalSliderDisplayRadius.setMaximum(maxRadius)
        self.ui.horizontalSliderDisplayRadius.setValue(maxRadius)
        self.viewer.renderer.setSampleInterval(self.ui.horizontalSliderSampling.value())
        self.viewer.renderer.setSurfaceValue(defaultDensity/100.0)
        self.viewer.renderer.setDisplayRadius(maxRadius)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setChecked(True)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setEnabled(True)
        self.showWidget(True)
        self.filterIsoValue.enabled = True
        self.filterSampling.enabled = True
        self.filterDisplayRadius.enabled = True
        self.viewer.renderer.enableDraw(True)
        
    
    def modelUnloaded(self):
        self.viewer.renderer.enableDraw(False)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setEnabled(False)
        self.showWidget(False)            
        
    def createActions(self):               
        self.displayAct.setEnabled(False)
  
    def createMenus(self):
        pass
    
    def isoValueIndicatorChanged(self, newLevel):
        newValue = newLevel/100.0
        self.ui.doubleSpinBoxDensity.setValue(float(newValue))
        
        maxValue = float(max(newValue, self.ui.doubleSpinBoxDensityMax.value()));
        if(self.ui.doubleSpinBoxDensityMax.value() != maxValue):
            self.ui.doubleSpinBoxDensityMax.setValue(maxValue)

        
        
    def isoValueMaxIndicatorChanged(self, newLevel):
        newValue = newLevel/100.0
        self.ui.doubleSpinBoxDensityMax.setValue(float(newValue))
        
        minValue = float(min(newValue, self.ui.doubleSpinBoxDensity.value()));
        if(self.ui.doubleSpinBoxDensity.value() != minValue):
            self.ui.doubleSpinBoxDensity.setValue(minValue)        
                
    def manualValueChanged(self):
        newValue = int(self.ui.doubleSpinBoxDensity.value()*100)
        if (not(self.ui.horizontalSliderIsoLevel.value() == newValue)):
            self.ui.horizontalSliderIsoLevel.setValue(newValue)
        
    def manualValueMaxChanged(self):
        newValue = int(self.ui.doubleSpinBoxDensityMax.value()*100)
        if (not(self.ui.horizontalSliderIsoLevelMax.value() == newValue)):
            self.ui.horizontalSliderIsoLevelMax.setValue(newValue)
                    
    def isoValueChanged(self, newLevel):
        #threading.Thread(target = self.updateIsoValue, args=(newLevel,)).start()
        self.updateIsoValue(newLevel)

    def isoValueMaxChanged(self, newLevel):
        #threading.Thread(target = self.updateIsoValue, args=(newLevel,)).start()
        self.updateIsoValueMax(newLevel)        
        
    def updateIsoValue(self, newLevel):        
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.setSurfaceValue(newLevel/100.0)
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.emitModelChanged()
        
    def updateOtherIsoValue(self, newLevel):
        maxValue = max(newLevel, self.ui.horizontalSliderIsoLevelMax.value())
        if (not(self.ui.horizontalSliderIsoLevelMax.value() == maxValue)):
            self.ui.horizontalSliderIsoLevelMax.setValue(maxValue)        
        
    def updateOtherIsoValueMax(self, newLevel):
        minValue = min(newLevel, self.ui.horizontalSliderIsoLevel.value())
        if (not(self.ui.horizontalSliderIsoLevel.value() == minValue)):
            self.ui.horizontalSliderIsoLevel.setValue(minValue)              
        
    def updateIsoValueMax(self, newLevel):        
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.setMaxSurfaceValue(newLevel/100.0)
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.emitModelChanged()        
    
    def samplingChanged(self, newLevel):
        self.viewer.renderer.setSampleInterval(newLevel)
        self.viewer.emitModelChanged()
        
    def displayRadiusChanged(self, newRadius):
        self.viewer.renderer.setDisplayRadius(newRadius)
        self.viewer.emitModelChanged()
        
    def displayRadiusEnabled(self, enabled):
        self.viewer.renderer.useDisplayRadius(enabled)
        self.viewer.emitModelChanged()
        