from PyQt4 import QtCore, QtGui
from ui_dialog_atom_visualization import Ui_DialogAtomVisualization
from model_visualization_form import ModelVisualizationForm 

class AtomVisualizationForm(ModelVisualizationForm):
    def __init__(self, main, viewer, parent=None):
        super(AtomVisualizationForm, self).__init__(main, viewer, parent)


    def createUI(self):
        self.ui = Ui_DialogAtomVisualization()
        self.ui.setupUi(self.viewer)
        self.ui.radioButtonBackbone.toggled.connect(self.setDisplayStyle)
        self.ui.radioButtonRibbon.toggled.connect(self.setDisplayStyle)
        self.ui.radioButtonSideChain.toggled.connect(self.setDisplayStyle)
        self.ui.pushButtonColor1.valueChanged.connect(self.setColor1)  
        self.ui.pushButtonColor2.valueChanged.connect(self.setColor2)  
        self.ui.pushButtonColor3.valueChanged.connect(self.setColor3)  
        self.ui.pushButtonColor4.valueChanged.connect(self.setColor4)  
        self.ui.pushButtonCenter.pressed.connect(self.viewer.on_center_clicked)
        self.ui.doubleSpinBoxSizeX.editingFinished.connect(self.scaleChanged)
        self.ui.doubleSpinBoxSizeY.editingFinished.connect(self.scaleChanged)
        self.ui.doubleSpinBoxSizeZ.editingFinished.connect(self.scaleChanged)
        self.ui.doubleSpinBoxLocationX.editingFinished.connect(self.locationChanged)
        self.ui.doubleSpinBoxLocationY.editingFinished.connect(self.locationChanged)
        self.ui.doubleSpinBoxLocationZ.editingFinished.connect(self.locationChanged)
        # self.ui.checkBoxShowAtoms.toggled.connect(self.viewer.setAtomVisibility)
        # self.ui.checkBoxShowBonds.toggled.connect(self.viewer.setBondVisibility)
        # self.ui.checkBoxShowHelices.toggled.connect(self.viewer.setHelixVisibility)
        # self.ui.checkBoxShowStrands.toggled.connect(self.viewer.setStrandVisibility)
        # self.ui.checkBoxShowLoops.toggled.connect(self.viewer.setLoopVisibility)
        # self.ui.hSliderSmoothness.valueChanged.connect(self.viewer.setSegments)
        # self.ui.hSliderRoundedness.valueChanged.connect(self.viewer.setSlices)
                                                 
        
    def updateFromViewer(self):
        # self.ui.checkBoxShowAtoms.setChecked(self.viewer.atomsVisible)
        # self.ui.checkBoxShowBonds.setChecked(self.viewer.bondsVisible)
        # self.ui.checkBoxShowHelices.setChecked(self.viewer.helicesVisible)
        # self.ui.checkBoxShowStrands.setChecked(self.viewer.strandsVisible)
        # self.ui.checkBoxShowLoops.setChecked(self.viewer.loopsVisible)
         
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
                                       str(round(self.viewer.renderer.getMaxPos(0) - self.viewer.renderer.getMinPos(0) ,2)) + ", " +
                                       str(round(self.viewer.renderer.getMaxPos(1) - self.viewer.renderer.getMinPos(1) ,2)) + ", " +
                                       str(round(self.viewer.renderer.getMaxPos(2) - self.viewer.renderer.getMinPos(2) ,2)) + "}")
        # if(len(self.viewer.fileName) > 0):
        #     names = self.viewer.fileName.split('\\')
        #     name = names[len(names)-1];
        #     names = name.split('/')
        #     name = names[len(names)-1];            
        #     self.ui.labelModelName.setText(name)
        # else:
        #     self.ui.labelModelName.setText("")
            
       
    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonBackbone.isChecked()) :
            displayStyle = self.viewer.DisplayStyleBackbone            
            self.ui.labelColor1.setText('Atom color:')
            self.ui.labelColor2.setText('Bond color:')
            # self.ui.pushButtonColor1.setColor(self.viewer.colors["C-Alpha:Atom"       ])
            # self.ui.pushButtonColor2.setColor(self.viewer.colors["C-Alpha:Bond"       ])            
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
            self.ui.labelColor1.setText('Helix color:')
            self.ui.labelColor2.setText('Strand color:')
            self.ui.labelColor3.setText('Loop color:')
            self.ui.pushButtonColor1.setColor(self.viewer.colors["C-Alpha:Helix"      ])
            self.ui.pushButtonColor2.setColor(self.viewer.colors["C-Alpha:Strand"     ])
            self.ui.pushButtonColor3.setColor(self.viewer.colors["C-Alpha:Loop"       ])            
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

            self.ui.labelColor1.setText('Carbon color:')
            self.ui.labelColor2.setText('Nitrogen color:')
            self.ui.labelColor3.setText('Oxygen color:')
            self.ui.labelColor4.setText('Sulphur color:')
            self.ui.pushButtonColor1.setColor(self.viewer.colors["C-Alpha:Carbon"     ])
            self.ui.pushButtonColor2.setColor(self.viewer.colors["C-Alpha:Nitrogen"   ])
            self.ui.pushButtonColor3.setColor(self.viewer.colors["C-Alpha:Oxygen"     ])                        
            self.ui.pushButtonColor4.setColor(self.viewer.colors["C-Alpha:Sulphur"    ])                        
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
            self.viewer.colors["C-Alpha:Atom"       ] = self.ui.pushButtonColor1.getColor()                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            self.viewer.colors["C-Alpha:Helix"      ] = self.ui.pushButtonColor1.getColor()                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.colors["C-Alpha:Carbon"     ] = self.ui.pushButtonColor1.getColor()                                       

    def setColor2(self):
        if(self.ui.radioButtonBackbone.isChecked()) :
            self.viewer.colors["C-Alpha:Bond"       ] = self.ui.pushButtonColor2.getColor()                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            self.viewer.colors["C-Alpha:Strand"     ] = self.ui.pushButtonColor2.getColor()                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.colors["C-Alpha:Nitrogen"   ] = self.ui.pushButtonColor2.getColor() 
                   
    def setColor3(self):
        if(self.ui.radioButtonBackbone.isChecked()) :
            pass                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            self.viewer.colors["C-Alpha:Loop"       ] = self.ui.pushButtonColor3.getColor()                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.colors["C-Alpha:Oxygen"     ] = self.ui.pushButtonColor3.getColor() 
            
    def setColor4(self):
        if(self.ui.radioButtonBackbone.isChecked()) :
            pass                              
        elif(self.ui.radioButtonRibbon.isChecked()) :
            pass                
        elif(self.ui.radioButtonSideChain.isChecked()) :
            self.viewer.colors["C-Alpha:Sulphur"    ] = self.ui.pushButtonColor4.getColor() 

                                                                                                                                                      
