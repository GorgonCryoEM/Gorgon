# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A dialog box which can be used to modify the visualization options of a loaded model 


from PyQt4 import QtCore, QtGui
from ui_dialog_model_visualization import Ui_DialogModelVisualization
from base_dock_widget import BaseDockWidget

class ModelVisualizationForm(BaseDockWidget):
    def __init__(self, main, viewer, parent=None):
        self.app = main
        self.viewer = viewer
        self.title = viewer.title + " Visualization Options"
        BaseDockWidget.__init__(self, 
                                main, 
                                self.title, 
                                "Load the " + self.viewer.title + " visualization options", 
                                "show_" + self.viewer.title + "Visualization",
                                "window-" + self.viewer.title + "VisualizationOptions", 
                                "window", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.RightDockWidgetArea,
                                parent)

        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)   
        self.setWindowTitle(self.title)
        
        self.createUI()
        self.createActions()
        self.createMenus()
        self.updateFromViewer()

    def createUI(self):
        self.ui = Ui_DialogModelVisualization()
        self.ui.setupUi(self)    
        self.ui.checkBoxModel2Visible.setVisible(False)
        self.ui.pushButtonModel2Color.setVisible(False)
        self.ui.checkBoxModel3Visible.setVisible(False)
        self.ui.pushButtonModel3Color.setVisible(False)
        self.ui.spinBoxThickness.setVisible(False)
        self.ui.labelThickness.setVisible(False)
        self.connect(self.ui.radioButtonWireframe, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonFlat, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonSmooth, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.checkBoxBoundingBox, QtCore.SIGNAL("toggled (bool)"), self.viewer.setBoundingBox)
        self.connect(self.ui.checkBoxModelVisible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModelVisibility)
        self.connect(self.ui.checkBoxModel2Visible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModel2Visibility)
        self.connect(self.ui.checkBoxModel3Visible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModel3Visibility)
        self.connect(self.ui.pushButtonBoundingBoxColor, QtCore.SIGNAL("colorChanged ()"), self.setBoundingBoxColor)        
        self.connect(self.ui.pushButtonModelColor, QtCore.SIGNAL("colorChanged ()"), self.setModelColor)  
        self.connect(self.ui.pushButtonModel2Color, QtCore.SIGNAL("colorChanged ()"), self.setModel2Color)  
        self.connect(self.ui.pushButtonModel3Color, QtCore.SIGNAL("colorChanged ()"), self.setModel3Color)  
        self.connect(self.ui.pushButtonCenter, QtCore.SIGNAL("pressed ()"), self.viewer.emitViewerSetCenterLocal)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.viewer.unloadData)
        self.connect(self.ui.doubleSpinBoxSizeX, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeY, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeZ, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxLocationX, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationY, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationZ, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.spinBoxThickness, QtCore.SIGNAL("editingFinished ()"), self.thicknessChanged)
                                                 
        
    def updateFromViewer(self):
        self.ui.pushButtonModelColor.setColor(self.viewer.getModelColor())
        self.ui.pushButtonModel2Color.setColor(self.viewer.getModel2Color())
        self.ui.pushButtonModel3Color.setColor(self.viewer.getModel3Color())
        self.ui.pushButtonBoundingBoxColor.setColor(self.viewer.getBoundingBoxColor())            
        self.ui.checkBoxBoundingBox.setChecked(self.viewer.showBox)    
        self.ui.checkBoxModelVisible.setChecked(self.viewer.modelVisible)
        self.ui.checkBoxModel2Visible.setChecked(self.viewer.model2Visible)
        self.ui.checkBoxModel3Visible.setChecked(self.viewer.model3Visible)
         
        if(self.viewer.displayStyle == self.viewer.DisplayStyleWireframe):
            self.ui.radioButtonWireframe.setChecked(True) 
        elif(self.viewer.displayStyle == self.viewer.DisplayStyleFlat):
            self.ui.radioButtonFlat.setChecked(True)   
        else :
            self.ui.radioButtonSmooth.setChecked(True)   
            
        self.ui.doubleSpinBoxSizeX.setValue(self.viewer.renderer.getSpacingX())   
        self.ui.doubleSpinBoxSizeY.setValue(self.viewer.renderer.getSpacingY())
        self.ui.doubleSpinBoxSizeZ.setValue(self.viewer.renderer.getSpacingZ())
        self.ui.doubleSpinBoxLocationX.setValue(self.viewer.renderer.getOriginX())
        self.ui.doubleSpinBoxLocationY.setValue(self.viewer.renderer.getOriginY())
        self.ui.doubleSpinBoxLocationZ.setValue(self.viewer.renderer.getOriginZ())
        self.ui.labelModelSize.setText("{" + 
                                       str(round(self.viewer.renderer.getMax(0) - self.viewer.renderer.getMin(0) ,2)) + ", " +
                                       str(round(self.viewer.renderer.getMax(1) - self.viewer.renderer.getMin(1) ,2)) + ", " +
                                       str(round(self.viewer.renderer.getMax(2) - self.viewer.renderer.getMin(2) ,2)) + "}")
        if(len(self.viewer.fileName) > 0):
            names = self.viewer.fileName.split('\\')
            name = names[len(names)-1];
            names = name.split('/')
            name = names[len(names)-1];            
            self.ui.labelModelName.setText(name)
        else:
            self.ui.labelModelName.setText("")
            
                                    
    def createActions(self):               
        self.visualizerAct = self.displayAct
        self.visualizerAct.setEnabled(False)
  
    def createMenus(self):
        pass     
             
    def modelLoaded(self):
        self.visualizerAct.setEnabled(True)
        self.updateFromViewer()        
        self.showWidget(True)
    
    def modelChanged(self):
        self.updateFromViewer()
    
    def modelUnloaded(self):
        self.visualizerAct.setEnabled(False)
        self.showWidget(False)    

    def setBoundingBoxColor(self):
        self.viewer.setBoundingBoxColor(self.ui.pushButtonBoundingBoxColor.color())
        
    def setModelColor(self):
        self.viewer.setModelColor(self.ui.pushButtonModelColor.color())

    def setModel2Color(self):
        self.viewer.setModel2Color(self.ui.pushButtonModel2Color.color())
    
    def setModel3Color(self):
        self.viewer.setModel3Color(self.ui.pushButtonModel3Color.color())
    
    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonWireframe.isChecked()) :
            displayStyle = self.viewer.DisplayStyleWireframe      
        elif(self.ui.radioButtonFlat.isChecked()) :
            displayStyle = self.viewer.DisplayStyleFlat    
        elif(self.ui.radioButtonSmooth.isChecked()) :
            displayStyle = self.viewer.DisplayStyleSmooth
        self.viewer.setDisplayStyle(displayStyle)
                                                  
    def scaleChanged(self):
        self.viewer.setScale(self.ui.doubleSpinBoxSizeX.value(), self.ui.doubleSpinBoxSizeY.value(), self.ui.doubleSpinBoxSizeZ.value())        
    
    def locationChanged(self):
        self.viewer.setLocation(self.ui.doubleSpinBoxLocationX.value(), self.ui.doubleSpinBoxLocationY.value(), self.ui.doubleSpinBoxLocationZ.value())
    
    def thicknessChanged(self):
        self.viewer.setThickness(self.ui.spinBoxThickness.value())
