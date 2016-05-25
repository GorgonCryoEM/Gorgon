from PyQt4 import QtCore, QtGui
from ui_dialog_volume_surface_editor import Ui_DialogVolumeSurfaceEditor
from base_dock_widget import BaseDockWidget
# import threading


class VolumeSurfaceEditorForm(BaseDockWidget):
    ViewingTypeIsoSurface, ViewingTypeCrossSection, ViewingTypeSolid = range(3)
    
    def __init__(self, main, volumeViewer):
        BaseDockWidget.__init__(self,
                                main,
                                self,
                                "&Volume - Surface Editor",
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea,
                                QtCore.Qt.BottomDockWidgetArea)
        self.app = main
        self.viewer = volumeViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoadedPreDraw()"), self.modelLoadedPreDraw)
        
        self.ui = Ui_DialogVolumeSurfaceEditor()
        self.ui.setupUi(self)
 
    def modelLoadedPreDraw(self):
        self.viewer.renderer.enableDraw(False)
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2

        maxRadius = int(max(self.viewer.renderer.getMaxPos(0)/2, self.viewer.renderer.getMaxPos(1)/2, self.viewer.renderer.getMaxPos(2)/2));
        self.viewer.renderer.setSampleInterval(1)
        self.viewer.renderer.setSurfaceValue(defaultDensity)
        self.viewer.renderer.setDisplayRadius(maxRadius)
        self.show()
        self.viewer.renderer.enableDraw(True)
