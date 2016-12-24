from PyQt4 import QtCore, QtGui
from ui_dialog_model_visualization import Ui_DialogModelVisualization


class ModelVisualizationForm(object):
    def __init__(self, main, viewer, parent=None):
        self.app = main
        self.viewer = viewer
        self.title = viewer.title + " Visualization Options"
        
        # self.viewer.visualizationUpdated.connect(self.modelLoaded)
        self.viewer.setWindowTitle(self.title)
        
        self.createUI()

    def createUI(self):
        self.ui = Ui_DialogModelVisualization()
        self.ui.setupUi(self.viewer)    
        self.ui.spinBoxThickness.setVisible(False)
        self.ui.labelThickness.setVisible(False)
        self.ui.radioButtonWireframe.toggled.connect(self.setDisplayStyle)
        self.ui.radioButtonFlat.toggled.connect(self.setDisplayStyle)
        self.ui.radioButtonSmooth.toggled.connect(self.setDisplayStyle)
        self.ui.checkBoxModelVisible.toggled.connect(self.viewer.setModelVisibility)
        self.ui.pushButtonModelColor.valueChanged.connect(self.setModelColor)  
        self.ui.pushButtonCenter.pressed.connect(self.viewer.on_center_clicked)
        self.ui.doubleSpinBoxLocationX.editingFinished.connect(self.locationChanged)
        self.ui.doubleSpinBoxLocationY.editingFinished.connect(self.locationChanged)
        self.ui.doubleSpinBoxLocationZ.editingFinished.connect(self.locationChanged)
        self.ui.spinBoxThickness.editingFinished.connect(self.thicknessChanged)
                                                 
        
    def updateFromViewer(self):
        self.ui.pushButtonModelColor.setColor(self.viewer.getColor())
         
        # if(self.viewer.displayStyle == self.viewer.DisplayStyleWireframe):
        #     self.ui.radioButtonWireframe.setChecked(True) 
        # elif(self.viewer.displayStyle == self.viewer.DisplayStyleFlat):
        #     self.ui.radioButtonFlat.setChecked(True)   
        # else :
        #     self.ui.radioButtonSmooth.setChecked(True)   
            
        self.ui.doubleSpinBoxSizeX.setText("%f" % self.viewer.renderer.getSpacingX())   
        self.ui.doubleSpinBoxSizeY.setText("%f" % self.viewer.renderer.getSpacingY())
        self.ui.doubleSpinBoxSizeZ.setText("%f" % self.viewer.renderer.getSpacingZ())
        self.ui.doubleSpinBoxSizeX.setReadOnly(True)
        self.ui.doubleSpinBoxSizeY.setReadOnly(True)
        self.ui.doubleSpinBoxSizeZ.setReadOnly(True)
        self.ui.doubleSpinBoxLocationX.setValue(self.viewer.offset[0])
        self.ui.doubleSpinBoxLocationY.setValue(self.viewer.offset[1])
        self.ui.doubleSpinBoxLocationZ.setValue(self.viewer.offset[2])
        # self.ui.labelModelSize.setText("{" +
        #                                str(round(self.viewer.renderer.getMaxPos(0) - self.viewer.renderer.getMinPos(0) ,2)) + ", " +
        #                                str(round(self.viewer.renderer.getMaxPos(1) - self.viewer.renderer.getMinPos(1) ,2)) + ", " +
        #                                str(round(self.viewer.renderer.getMaxPos(2) - self.viewer.renderer.getMinPos(2) ,2)) + "}")
        # if(len(self.viewer.fileName) > 0):
        #     names = self.viewer.fileName.split('\\')
        #     name = names[len(names)-1];
        #     names = name.split('/')
        #     name = names[len(names)-1];            
        #     self.ui.labelModelName.setText(name)
        # else:
        #     self.ui.labelModelName.setText("")
            
    def modelLoaded(self):
        self.updateFromViewer()        
    
    def modelChanged(self):
        self.updateFromViewer()
    
    def setModelColor(self):
        self.viewer.setColor(self.ui.pushButtonModelColor.getColor())

    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonWireframe.isChecked()) :
            self.viewer.runDisplayType = self.viewer.display_styles[0]
        elif(self.ui.radioButtonFlat.isChecked()) :
            self.viewer.runDisplayType = self.viewer.display_styles[1]
        elif(self.ui.radioButtonSmooth.isChecked()) :
            self.viewer.runDisplayType = self.viewer.display_styles[2]
        self.viewer.visualizationUpdated.emit()
                                                  
    def locationChanged(self):
        self.viewer.setLocation(self.ui.doubleSpinBoxLocationX.value(), self.ui.doubleSpinBoxLocationY.value(), self.ui.doubleSpinBoxLocationZ.value())
    
    def thicknessChanged(self):
        self.viewer.setThickness(self.ui.spinBoxThickness.value())
