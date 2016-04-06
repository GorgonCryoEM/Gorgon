from PyQt4 import QtCore, QtGui
from ui_dialog_volume_surface_editor import Ui_DialogVolumeSurfaceEditor
from base_dock_widget import BaseDockWidget
# import threading


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
        self.createUI()
        self.createActions()

    def createUI(self):
        self.ui = Ui_DialogVolumeSurfaceEditor()
        self.ui.setupUi(self)
 
        self.ui.labelIsoLevelMax.setVisible(False)
        
        self.connect(self.ui.radioButtonIsoSurface, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        self.connect(self.ui.radioButtonCrossSection, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        self.connect(self.ui.radioButtonSolid, QtCore.SIGNAL("toggled(bool)"), self.setViewingType)
        
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
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        if(self.ui.radioButtonIsoSurface.isChecked()):
            defaultDensity = (minDensity + maxDensity) / 2
        else:
            defaultDensity = minDensity
        defaultDensity = (minDensity + maxDensity) / 2

        maxRadius = int(max(self.viewer.renderer.getMax(0)/2, self.viewer.renderer.getMax(1)/2, self.viewer.renderer.getMax(2)/2));
        self.viewer.renderer.setSampleInterval(1)
        self.viewer.renderer.setSurfaceValue(defaultDensity)
        self.viewer.renderer.setDisplayRadius(maxRadius)
        self.displayAct.setChecked(True)
        self.displayAct.setEnabled(True)
        self.showWidget(True)
        self.viewer.renderer.enableDraw(True)
        
    def createActions(self):
        self.displayAct.setEnabled(False)
    
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
        
