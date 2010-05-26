# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A dialog box which can be used to modify the visualization options of a loaded atomic model 

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
from ui_dialog_atom_visualization import Ui_DialogAtomVisualization
from base_dock_widget import BaseDockWidget
from model_visualization_form import ModelVisualizationForm 

class AtomVisualizationForm(ModelVisualizationForm):
    def __init__(self, main, viewer, parent=None):
        ModelVisualizationForm.__init__(self, main, viewer, parent)


    def createUI(self):
        self.ui = Ui_DialogAtomVisualization()
        self.ui.setupUi(self)
        self.connect(self.ui.radioButtonBackbone, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonRibbon, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonSideChain, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.checkBoxBoundingBox, QtCore.SIGNAL("toggled (bool)"), self.viewer.setBoundingBox)
        self.connect(self.ui.checkBoxModelVisible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModelVisibility)
        self.connect(self.ui.checkBoxModel2Visible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModel2Visibility)
        self.connect(self.ui.pushButtonBoundingBoxColor, QtCore.SIGNAL("colorChanged ()"), self.setBoundingBoxColor)        
        self.connect(self.ui.pushButtonModelColor, QtCore.SIGNAL("colorChanged ()"), self.setModelColor)  
        self.connect(self.ui.pushButtonModel2Color, QtCore.SIGNAL("colorChanged ()"), self.setModel2Color)  
        self.connect(self.ui.pushButtonCenter, QtCore.SIGNAL("pressed ()"), self.viewer.emitViewerSetCenterLocal)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.viewer.unloadData)
        self.connect(self.ui.doubleSpinBoxSizeX, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeY, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeZ, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxLocationX, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationY, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationZ, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.checkBoxShowHelices, QtCore.SIGNAL("toggled (bool)"), self.viewer.setHelixVisibility)
        self.connect(self.ui.checkBoxShowStrands, QtCore.SIGNAL("toggled (bool)"), self.viewer.setStrandVisibility)
        self.connect(self.ui.checkBoxShowLoops, QtCore.SIGNAL("toggled (bool)"), self.viewer.setLoopVisibility)
                                                 
        
    def updateFromViewer(self):
        self.ui.pushButtonModelColor.setColor(self.viewer.getModelColor())
        self.ui.pushButtonModel2Color.setColor(self.viewer.getModel2Color())
        self.ui.pushButtonBoundingBoxColor.setColor(self.viewer.getBoundingBoxColor())            
        self.ui.checkBoxBoundingBox.setChecked(self.viewer.showBox)    
        self.ui.checkBoxModelVisible.setChecked(self.viewer.modelVisible)
        self.ui.checkBoxModel2Visible.setChecked(self.viewer.model2Visible)
        self.ui.checkBoxShowHelices.setChecked(self.viewer.helicesVisible)
        self.ui.checkBoxShowStrands.setChecked(self.viewer.strandsVisible)
        self.ui.checkBoxShowLoops.setChecked(self.viewer.loopsVisible)
         
        if(self.viewer.displayStyle == self.viewer.DisplayStyleBackbone):
            self.ui.radioButtonBackbone.setChecked(True) 
        elif(self.viewer.displayStyle == self.viewer.DisplayStyleRibbon):
            self.ui.radioButtonRibbon.setChecked(True)   
        else :
            self.ui.radioButtonSideChain.setChecked(True)   
            
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
            
       
    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonBackbone.isChecked()) :
            displayStyle = self.viewer.DisplayStyleBackbone      
        elif(self.ui.radioButtonRibbon.isChecked()) :
            displayStyle = self.viewer.DisplayStyleRibbon    
        elif(self.ui.radioButtonSideChain.isChecked()) :
            displayStyle = self.viewer.DisplayStyleSideChain
        self.viewer.setDisplayStyle(displayStyle)
                                                  