# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A dialog box which can be used to modify the visualization options of a loaded atomic model 



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
        self.connect(self.ui.pushButtonBoundingBoxColor, QtCore.SIGNAL("colorChanged ()"), self.setBoundingBoxColor)        
        self.connect(self.ui.pushButtonColor1, QtCore.SIGNAL("colorChanged ()"), self.setColor1)  
        self.connect(self.ui.pushButtonColor2, QtCore.SIGNAL("colorChanged ()"), self.setColor2)  
        self.connect(self.ui.pushButtonColor3, QtCore.SIGNAL("colorChanged ()"), self.setColor3)  
        self.connect(self.ui.pushButtonColor4, QtCore.SIGNAL("colorChanged ()"), self.setColor4)  
        self.connect(self.ui.pushButtonCenter, QtCore.SIGNAL("pressed ()"), self.viewer.emitViewerSetCenterLocal)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.viewer.unloadData)
        self.connect(self.ui.doubleSpinBoxSizeX, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeY, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeZ, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxLocationX, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationY, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationZ, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.checkBoxShowAtoms, QtCore.SIGNAL("toggled (bool)"), self.viewer.setAtomVisibility)
        self.connect(self.ui.checkBoxShowBonds, QtCore.SIGNAL("toggled (bool)"), self.viewer.setBondVisibility)
        self.connect(self.ui.checkBoxShowHelices, QtCore.SIGNAL("toggled (bool)"), self.viewer.setHelixVisibility)
        self.connect(self.ui.checkBoxShowStrands, QtCore.SIGNAL("toggled (bool)"), self.viewer.setStrandVisibility)
        self.connect(self.ui.checkBoxShowLoops, QtCore.SIGNAL("toggled (bool)"), self.viewer.setLoopVisibility)
        self.connect(self.ui.hSliderSmoothness, QtCore.SIGNAL("valueChanged (int)"), self.viewer.setSegments)
        self.connect(self.ui.hSliderRoundedness, QtCore.SIGNAL("valueChanged (int)"), self.viewer.setSlices)
        # the following four methods for testing purposes only
        self.connect(self.ui.rslider, QtCore.SIGNAL("valueChanged (int)"), self.viewer.setHltR)
        self.connect(self.ui.gslider, QtCore.SIGNAL("valueChanged (int)"), self.viewer.setHltG)
        self.connect(self.ui.bslider, QtCore.SIGNAL("valueChanged (int)"), self.viewer.setHltB)
        self.connect(self.ui.aslider, QtCore.SIGNAL("valueChanged (int)"), self.viewer.setHltA)
                                                 
        
    def updateFromViewer(self):
        self.ui.pushButtonBoundingBoxColor.setColor(self.viewer.getBoundingBoxColor())            
        self.ui.checkBoxBoundingBox.setChecked(self.viewer.showBox)    
        self.ui.checkBoxShowAtoms.setChecked(self.viewer.atomsVisible)
        self.ui.checkBoxShowBonds.setChecked(self.viewer.bondsVisible)
        self.ui.checkBoxShowHelices.setChecked(self.viewer.helicesVisible)
        self.ui.checkBoxShowStrands.setChecked(self.viewer.strandsVisible)
        self.ui.checkBoxShowLoops.setChecked(self.viewer.loopsVisible)
         
        if(self.viewer.displayStyle == self.viewer.DisplayStyleBackbone):
            self.ui.radioButtonBackbone.setChecked(True) 
        elif(self.viewer.displayStyle == self.viewer.DisplayStyleRibbon):
            self.ui.radioButtonRibbon.setChecked(True)   
        else :
            self.ui.radioButtonSideChain.setChecked(True)   
            
        self.setDisplayStyle(None)
            
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
            self.ui.labelColor1.setText(self.tr('Atom color:'))
            self.ui.labelColor2.setText(self.tr('Bond color:'))
            self.ui.pushButtonColor1.setColor(self.viewer.getAtomColor())
            self.ui.pushButtonColor2.setColor(self.viewer.getBondColor())            
            self.ui.labelColor1.setVisible(True)
            self.ui.labelColor2.setVisible(True)
            self.ui.labelColor3.setVisible(False)
            self.ui.labelColor4.setVisible(False)
            self.ui.pushButtonColor1.setVisible(True)
            self.ui.pushButtonColor2.setVisible(True)
            self.ui.pushButtonColor3.setVisible(False)
            self.ui.pushButtonColor4.setVisible(False)
            self.ui.checkBoxShowAtoms.setVisible(True)
            self.ui.checkBoxShowBonds.setVisible(True)
                  
        elif(self.ui.radioButtonRibbon.isChecked()) :
            displayStyle = self.viewer.DisplayStyleRibbon
            self.ui.labelColor1.setText(self.tr('Helix color:'))
            self.ui.labelColor2.setText(self.tr('Strand color:'))
            self.ui.labelColor3.setText(self.tr('Loop color:'))
            self.ui.pushButtonColor1.setColor(self.viewer.getHelixColor())
            self.ui.pushButtonColor2.setColor(self.viewer.getStrandColor())
            self.ui.pushButtonColor3.setColor(self.viewer.getLoopColor())            
            self.ui.labelColor1.setVisible(True)
            self.ui.labelColor2.setVisible(True)
            self.ui.labelColor3.setVisible(True)
            self.ui.labelColor4.setVisible(False)
            self.ui.pushButtonColor1.setVisible(True)
            self.ui.pushButtonColor2.setVisible(True)
            self.ui.pushButtonColor3.setVisible(True)
            self.ui.pushButtonColor4.setVisible(False)
            self.ui.checkBoxShowAtoms.setVisible(False)
            self.ui.checkBoxShowBonds.setVisible(False)
                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            displayStyle = self.viewer.DisplayStyleSideChain

            self.ui.labelColor1.setText(self.tr('Carbon color:'))
            self.ui.labelColor2.setText(self.tr('Nitrogen color:'))
            self.ui.labelColor3.setText(self.tr('Oxygen color:'))
            self.ui.labelColor4.setText(self.tr('Sulphur color:'))
            self.ui.pushButtonColor1.setColor(self.viewer.getCarbonColor())
            self.ui.pushButtonColor2.setColor(self.viewer.getNitrogenColor())
            self.ui.pushButtonColor3.setColor(self.viewer.getOxygenColor())                        
            self.ui.pushButtonColor4.setColor(self.viewer.getSulphurColor())                        
            self.ui.labelColor1.setVisible(True)
            self.ui.labelColor2.setVisible(True)
            self.ui.labelColor3.setVisible(True)
            self.ui.labelColor4.setVisible(True)
            self.ui.pushButtonColor1.setVisible(True)
            self.ui.pushButtonColor2.setVisible(True)
            self.ui.pushButtonColor3.setVisible(True)
            self.ui.pushButtonColor4.setVisible(True)            
            self.ui.checkBoxShowAtoms.setVisible(True)
            self.ui.checkBoxShowBonds.setVisible(True)
           
        self.viewer.setDisplayStyle(displayStyle)        
        
        
    def setColor1(self):
        if(self.ui.radioButtonBackbone.isChecked()) :
            self.viewer.setAtomColor(self.ui.pushButtonColor1.color())                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            self.viewer.setHelixColor(self.ui.pushButtonColor1.color())                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.setCarbonColor(self.ui.pushButtonColor1.color())                                       

    def setColor2(self):
        if(self.ui.radioButtonBackbone.isChecked()) :
            self.viewer.setBondColor(self.ui.pushButtonColor2.color())                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            self.viewer.setStrandColor(self.ui.pushButtonColor2.color())                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.setNitrogenColor(self.ui.pushButtonColor2.color()) 
                   
    def setColor3(self):
        if(self.ui.radioButtonBackbone.isChecked()) :
            pass                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            self.viewer.setLoopColor(self.ui.pushButtonColor3.color())                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.setOxygenColor(self.ui.pushButtonColor3.color()) 
            
    def setColor4(self):
        if(self.ui.radioButtonBackbone.isChecked()) :
            pass                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            pass                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.setSulphurColor(self.ui.pushButtonColor4.color()) 

                                                                                                                                                      
