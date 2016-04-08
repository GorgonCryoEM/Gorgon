from PyQt4 import QtCore, QtGui
from ui_dialog_model_visualization import Ui_DialogModelVisualization
from base_dock_widget import BaseDockWidget


class ModelVisualizationForm(BaseDockWidget):

    def __init__(self, main, viewer):
        self.app = main
        self.viewer = viewer
        self.title = viewer.title + " Visualization Options"
        BaseDockWidget.__init__(self,
                                main,
                                self,
                                self.title,
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea,
                                QtCore.Qt.RightDockWidgetArea)

        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.updateFromViewer)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.updateFromViewer)
        self.setWindowTitle(self.title)
        
        self.ui = Ui_DialogModelVisualization()
        self.ui.setupUi(self)
        self.ui.spinBoxThickness.setVisible(False)
        self.ui.labelThickness.setVisible(False)
        self.setSignals()

    def setSignals(self):
        self.connect(self.ui.radioButtonWireframe,   QtCore.SIGNAL("toggled (bool)"),     self.setDisplayStyle)
        self.connect(self.ui.radioButtonFlat,        QtCore.SIGNAL("toggled (bool)"),     self.setDisplayStyle)
        self.connect(self.ui.radioButtonSmooth,      QtCore.SIGNAL("toggled (bool)"),     self.setDisplayStyle)
        self.connect(self.ui.checkBoxModelVisible,   QtCore.SIGNAL("toggled (bool)"),     self.viewer.setModelVisibility)
        self.connect(self.ui.pushButtonModelColor,   QtCore.SIGNAL("colorChanged ()"),    self.setModelColor)
        self.connect(self.ui.pushButtonCenter,       QtCore.SIGNAL("pressed ()"),         self.viewer.emitViewerSetCenterLocal)
        self.connect(self.ui.pushButtonClose,        QtCore.SIGNAL("pressed ()"),         self.viewer.unload)
        self.connect(self.ui.doubleSpinBoxSizeX,     QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeY,     QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeZ,     QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxLocationX, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationY, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationZ, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.spinBoxThickness, QtCore.SIGNAL("editingFinished ()"), self.thicknessChanged)
        self.updateFromViewer()
                                                 
    def updateFromViewer(self):
        self.ui.pushButtonModelColor.setColor(self.viewer.getColor())
        self.ui.checkBoxModelVisible.setChecked(self.viewer.modelVisible)
         
        if(self.viewer.displayStyle == self.viewer.DisplayStyleWireframe):
            self.ui.radioButtonWireframe.setChecked(True)
        elif(self.viewer.displayStyle == self.viewer.DisplayStyleFlat):
            self.ui.radioButtonFlat.setChecked(True)
        else:
            self.ui.radioButtonSmooth.setChecked(True)
            
        self.ui.doubleSpinBoxSizeX.setValue(self.viewer.renderer.getSpacingX())
        self.ui.doubleSpinBoxSizeY.setValue(self.viewer.renderer.getSpacingY())
        self.ui.doubleSpinBoxSizeZ.setValue(self.viewer.renderer.getSpacingZ())
        self.ui.doubleSpinBoxLocationX.setValue(self.viewer.renderer.getOriginX())
        self.ui.doubleSpinBoxLocationY.setValue(self.viewer.renderer.getOriginY())
        self.ui.doubleSpinBoxLocationZ.setValue(self.viewer.renderer.getOriginZ())
        self.ui.labelModelSize.setText("{" +
                                       str(round(self.viewer.renderer.getMax(0) - self.viewer.renderer.getMin(0),2)) + ", " +
                                       str(round(self.viewer.renderer.getMax(1) - self.viewer.renderer.getMin(1),2)) + ", " +
                                       str(round(self.viewer.renderer.getMax(2) - self.viewer.renderer.getMin(2),2)) + "}")
        if(len(self.viewer.fileName) > 0):
            names = self.viewer.fileName.split('\\')
            name = names[len(names)-1];
            names = name.split('/')
            name = names[len(names)-1];
            self.ui.labelModelName.setText(name)
        else:
            self.ui.labelModelName.setText("")
            
    def setModelColor(self):
        self.viewer.setColor(self.ui.pushButtonModelColor.color())
    
    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonWireframe.isChecked()):
            displayStyle = self.viewer.DisplayStyleWireframe
        elif(self.ui.radioButtonFlat.isChecked()):
            displayStyle = self.viewer.DisplayStyleFlat
        elif(self.ui.radioButtonSmooth.isChecked()):
            displayStyle = self.viewer.DisplayStyleSmooth
        self.viewer.setDisplayStyle(displayStyle)
                                                  
    def scaleChanged(self):
        self.viewer.setScale(self.ui.doubleSpinBoxSizeX.value(), self.ui.doubleSpinBoxSizeY.value(), self.ui.doubleSpinBoxSizeZ.value())
    
    def locationChanged(self):
        self.viewer.setLocation(self.ui.doubleSpinBoxLocationX.value(), self.ui.doubleSpinBoxLocationY.value(), self.ui.doubleSpinBoxLocationZ.value())
    
    def thicknessChanged(self):
        self.viewer.setThickness(self.ui.spinBoxThickness.value())
