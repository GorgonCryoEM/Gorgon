# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget can be used to place c-alpha atoms 


from PyQt4 import QtCore, QtGui
from libpyGORGON import PDBAtom, Vector3DFloat
from seq_model.Residue import Residue
from base_dock_widget import BaseDockWidget

class CAlphaAtomPlacerForm(BaseDockWidget):
    def __init__(self, main, viewer, main_chain, structPred, parent=None):
        BaseDockWidget.__init__(self, 
                                main, 
                                "&Manual C-Alpha Atom Placement", 
                                "Perform Manual C-Alpha atom placement", 
                                "perform_CAlphaManualAtomPlacement", 
                                "actions-calpha-manualAtomPlacement", 
                                "actions-calpha", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.RightDockWidgetArea, 
                                parent)
        self.app = main
        self.viewer = viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.connect(self.skeletonViewer, QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.skeletonSelected)
        self.main_chain=main_chain
        self.structPred = structPred
        self.createUI()
        self.connect(self.addAtomPushButton, QtCore.SIGNAL("pressed()"), self.addAtom)
        self.connect(self.resSeqNumSpinBox,  QtCore.SIGNAL('valueChanged(int)'), self.updateResName)

    def createUI(self):           
        serialLabel = QtGui.QLabel('Serial:')
        atomNameLabel = QtGui.QLabel('Atom Name:')
        altLocLabel = QtGui.QLabel('Alternate Location:')
        resSeqNumLabel = QtGui.QLabel('Reisude:')
        insertionCodeLabel = QtGui.QLabel('Insertion Code:')
        xLabel = QtGui.QLabel('x:')
        yLabel = QtGui.QLabel('y:')
        zLabel = QtGui.QLabel('z:')
        occupancyLabel = QtGui.QLabel('Occupancy:')
        tempFactorLabel = QtGui.QLabel('Temperature Factor:')
        elementLabel = QtGui.QLabel('Element:')
        chargeLabel = QtGui.QLabel('Charge:')
        noteLabel = QtGui.QLabel('Note: position can be set by clicking on the skeleton')
        
        self.identifierLabel = QtGui.QLabel('???? chain ?')
        self.resNameLabel = QtGui.QLabel('ALA')
        self.resSeqNumSpinBox = QtGui.QSpinBox()
        self.resSeqNumSpinBox.setRange(1, 100000)
        self.atomNameLineEdit = QtGui.QLineEdit('CA')
        self.atomNameLineEdit.setMaxLength(4)
        self.elementLineEdit = QtGui.QLineEdit('C')
        self.elementLineEdit.setMaxLength(2)
        self.xDoubleSpinBox = QtGui.QDoubleSpinBox()
        self.yDoubleSpinBox = QtGui.QDoubleSpinBox()
        self.zDoubleSpinBox = QtGui.QDoubleSpinBox()
        self.xDoubleSpinBox.setMaximum(99999999.99)
        self.xDoubleSpinBox.setMinimum(-99999999.99)
        self.yDoubleSpinBox.setMaximum(99999999.99)
        self.yDoubleSpinBox.setMinimum(-99999999.99)
        self.zDoubleSpinBox.setMaximum(99999999.99)
        self.zDoubleSpinBox.setMinimum(-99999999.99)
        self.serialSpinBox = QtGui.QSpinBox()
        self.altLocLineEdit = QtGui.QLineEdit()
        self.altLocLineEdit.setMaxLength(1)
        self.insertionCodeLineEdit = QtGui.QLineEdit()
        self.insertionCodeLineEdit.setMaxLength(1)
        self.occupancyDoubleSpinBox = QtGui.QDoubleSpinBox()
        self.occupancyDoubleSpinBox.setRange(0,1)
        self.occupancyDoubleSpinBox.setSingleStep(0.1)
        self.tempFactorDoubleSpinBox = QtGui.QDoubleSpinBox()
        self.chargeLineEdit = QtGui.QLineEdit()
        self.chargeLineEdit.setMaxLength(2)
        self.addAtomPushButton = QtGui.QPushButton('A&dd Atom')
        
        self.tabWidget = QtGui.QTabWidget()
        self.basicTab = QtGui.QWidget()
        self.extraTab = QtGui.QWidget()
        self.tabWidget.addTab(self.basicTab, self.tr('Basic Options'))
        self.tabWidget.addTab(self.extraTab, self.tr('Extra Options'))
        
        
        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.identifierLabel)
        layout.addWidget(self.tabWidget)
        layout.addWidget(self.addAtomPushButton)
        self.setLayout(layout)
        
        basicLayout = QtGui.QGridLayout()
        basicLayout.addWidget(self.resSeqNumSpinBox,0,1)
        basicLayout.addWidget(self.resNameLabel,0,0)
        basicLayout.addWidget(atomNameLabel,1,0)
        basicLayout.addWidget(self.atomNameLineEdit,1,1)
        basicLayout.addWidget(elementLabel,2,0)
        basicLayout.addWidget(self.elementLineEdit,2,1)
        basicLayout.addWidget(xLabel,0,2)
        basicLayout.addWidget(self.xDoubleSpinBox,0,3)
        basicLayout.addWidget(yLabel,1,2)
        basicLayout.addWidget(self.yDoubleSpinBox,1,3)
        basicLayout.addWidget(zLabel,2,2)
        basicLayout.addWidget(self.zDoubleSpinBox,2,3)
        self.basicTab.setLayout(basicLayout)
        
        extraLayout = QtGui.QGridLayout()
        extraLayout.addWidget(serialLabel,0,0)
        extraLayout.addWidget(self.serialSpinBox,0,1)
        extraLayout.addWidget(altLocLabel,1,0)
        extraLayout.addWidget(self.altLocLineEdit,1,1)
        extraLayout.addWidget(insertionCodeLabel,2,0)
        extraLayout.addWidget(self.insertionCodeLineEdit,2,1)
        extraLayout.addWidget(occupancyLabel,0,2)
        extraLayout.addWidget(self.occupancyDoubleSpinBox,0,3)
        extraLayout.addWidget(tempFactorLabel,1,2)
        extraLayout.addWidget(self.tempFactorDoubleSpinBox,1,3)
        extraLayout.addWidget(chargeLabel,2,2)
        extraLayout.addWidget(self.chargeLineEdit,2,3)
        self.extraTab.setLayout(extraLayout)
        
    def loadWidget(self):
        self.structPred = self.viewer.structPred
        if(self.app.actions.getAction("perform_CAlphaManualAtomPlacement").isChecked()) :
            if self.structPred and self.structPred.chain:
                self.resSeqNumSpinBox.setRange( min(self.structPred.chain.residueRange()), max(self.structPred.chain.residueRange()) )
            else:
                return            
            self.skeletonViewer.setSelectEnabled(True)
            if not self.viewer.main_chain:
                self.viewer.main_chain = self.viewer.structPred.chain
            self.main_chain = self.viewer.main_chain
            residue = self.structPred.chain[self.resSeqNumSpinBox.value()]
            self.resNameLabel.setText(residue.symbol3)
            self.identifierLabel.setText( self.main_chain.getPdbID() + ' chain ' + self.main_chain.getChainID() )
        else:
            self.skeletonViewer.setSelectEnabled(False)
        BaseDockWidget.loadWidget(self)
                    
    def skeletonSelected(self, h0, h1, h2, h3, h4, h5, event):
        position = self.skeletonViewer.renderer.get3DCoordinates(h0, h1, h2, h3, h4, h5)
        skeletonCoordinates = [position.x(), position.y(), position.z()]
        calphaCoordinates = self.viewer.worldToObjectCoordinates(self.skeletonViewer.objectToWorldCoordinates(skeletonCoordinates))         
        
        self.xDoubleSpinBox.setValue(calphaCoordinates[0]);
        self.yDoubleSpinBox.setValue(calphaCoordinates[1]);
        self.zDoubleSpinBox.setValue(calphaCoordinates[2]);
    
    def addAtom(self):
        #Atom attributes
        x,y,z = self.xDoubleSpinBox.value(), self.yDoubleSpinBox.value(), self.zDoubleSpinBox.value() 
        element= str(self.elementLineEdit.text())
        occupancy = self.occupancyDoubleSpinBox.value()
        temp_factor = self.tempFactorDoubleSpinBox.value() 
        atom_name=str(self.atomNameLineEdit.text())
        index=int(str(self.resSeqNumSpinBox.value()))
        residue = self.main_chain[index]
        residue.addAtom(atom_name, x, y, z, element, occupancy=occupancy, tempFactor=temp_factor)
        print self.main_chain.toPDB()
        self.resSeqNumSpinBox.setValue(self.resSeqNumSpinBox.value()+1)
        atom = residue.getAtom(atom_name)
        if atom_name == 'CA':
            viewer = self.app.viewers['calpha']
            residue.addAtomObject( viewer.renderer.addAtom(atom) )
            if not viewer.loaded:
                viewer.dirty = False
                viewer.loaded = True
                viewer.emitModelLoadedPreDraw()
                viewer.emitModelLoaded()
            else:
                viewer.emitModelChanged()
        self.bringToFront()
        
    def updateResName(self, index):
        print 'in updateResName'
        self.resNameLabel.setText(self.structPred.chain[index].symbol3)
