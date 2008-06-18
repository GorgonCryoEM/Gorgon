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

from PyQt4 import QtCore, QtGui
from ui_dialog_volume_surface_editor import Ui_DialogVolumeSurfaceEditor
from delayed_filter import DelayedFilter
import threading

class VolumeSurfaceEditorForm(QtGui.QWidget):
    ViewingTypeIsoSurface, ViewingTypeCrossSection, ViewingTypeSolid = range(3)
    
    def __init__(self, main, volumeViewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = volumeViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.createUI()
        self.createActions()
        self.createMenus()

    def createUI(self):
        self.ui = Ui_DialogVolumeSurfaceEditor()
        self.ui.setupUi(self)       
        self.dock = QtGui.QDockWidget(self.tr("Volume - Surface Editor"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.horizontalSliderIsoLevel,QtCore.SIGNAL("valueChanged(int)"),self.isoValueIndicatorChanged)
        self.filterIsoValue = DelayedFilter(self.thread())
        self.filterSampling = DelayedFilter(self.thread())
        self.connect(self.ui.horizontalSliderIsoLevel, QtCore.SIGNAL("valueChanged(int)"),self.filterIsoValue.setValue)
        self.connect(self.ui.horizontalSliderSampling, QtCore.SIGNAL("valueChanged(int)"),self.filterSampling.setValue)
        self.connect(self.filterIsoValue, QtCore.SIGNAL("valueChanged(int)"), self.isoValueChanged )
        self.connect(self.filterSampling, QtCore.SIGNAL("valueChanged(int)"), self.samplingChanged )
        self.connect(self.ui.radioButtonIsoSurface, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        self.connect(self.ui.radioButtonCrossSection, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        self.connect(self.ui.radioButtonSolid, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
            
    def loadWidget(self):
        if(self.app.actions.getAction("show_VolumeSurfaceEditor").isChecked()) :
            self.showWidget(True)
        else:
            self.showWidget(False)

    def showWidget(self, show):
        if(show):
            self.app.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.dock)
            self.dock.show()
        else:
             self.app.removeDockWidget(self.dock)                       
                    
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("show_VolumeSurfaceEditor").setChecked(visible)
    
    def setViewingType(self, dummy):
        if(self.ui.radioButtonIsoSurface.isChecked()):
           self.viewer.renderer.setViewingType(self.ViewingTypeIsoSurface)
        elif self.ui.radioButtonCrossSection.isChecked():
            self.viewer.renderer.setViewingType(self.ViewingTypeCrossSection)
        elif self.ui.radioButtonSolid.isChecked():
            self.viewer.renderer.setViewingType(self.ViewingTypeSolid)
        print "setViewingType", QtCore.QThread.currentThreadId()
        self.viewer.emitModelChanged()
    
    def modelLoaded(self):
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderIsoLevel.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderIsoLevel.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        self.ui.horizontalSliderIsoLevel.setValue(defaultDensity)
        self.viewer.renderer.setSampleInterval(self.ui.horizontalSliderSampling.value())
        self.viewer.renderer.setSurfaceValue(defaultDensity/100.0)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setChecked(True)
        self.app.actions.getAction("show_VolumeSurfaceEditor").setEnabled(True)
        self.showWidget(True)
    
    def modelUnloaded(self):
        self.app.actions.getAction("show_VolumeSurfaceEditor").setEnabled(False)
        self.showWidget(False)            
        
    def createActions(self):               
        volumeEditorAct = QtGui.QAction(self.tr("&Volume Surface Editor"), self)
        volumeEditorAct.setStatusTip(self.tr("Modify the volume surface"))
        volumeEditorAct.setCheckable(True)
        volumeEditorAct.setChecked(True)
        volumeEditorAct.setEnabled(False)
        self.connect(volumeEditorAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("show_VolumeSurfaceEditor", volumeEditorAct)
  
    def createMenus(self):
        self.app.menus.addAction("window-VolumeSurfaceEditor", self.app.actions.getAction("show_VolumeSurfaceEditor"), "window")        

    def isoValueIndicatorChanged(self, newLevel):
        self.ui.labelIsoValueDisplay.setNum(newLevel/100.0)
        
    def isoValueChanged(self, newLevel):
        threading.Thread(target = self.updateIsoValue, args=(newLevel,)).start()
        
    def updateIsoValue(self, newLevel):
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.setSurfaceValue(newLevel/100.0)
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.emitModelChanged()
    
    def samplingChanged(self, newLevel):
        self.viewer.renderer.setSampleInterval(newLevel)
        self.viewer.emitModelChanged()
        