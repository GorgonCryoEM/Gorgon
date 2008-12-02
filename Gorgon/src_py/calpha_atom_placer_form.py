# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget can be used to place c-alpha atoms 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.14  2008/11/20 21:52:10  ssa1
#   Fixing manual placement form coordinate space bug
#
#   Revision 1.13  2008/11/14 23:06:54  colemanr
#   now makes use of CAlphaViewer.strucPred in addition to CAlphaViewer.main_chain
#
#   Revision 1.12  2008/11/13 01:02:08  colemanr
#   removed a completed "#TODO: ..." comment
#
#   Revision 1.11  2008/11/10 19:35:21  colemanr
#   Modified to work with the updated CAlphaRenderer.h--no longer need to
#   delete and re-add atoms to the renderer to display changes
#
#   Revision 1.10  2008/11/04 16:09:45  colemanr
#   no longer import ui_dialog_calpha_atom_placer - coded by hand now
#
#   Revision 1.9  2008/10/30 21:13:26  colemanr
#   no longer relies on deprecated GAtom objects, displays the correct
#   residue symbol for the selected index, no longer depends on the output
#   of the ui file & now has tabs to save screen space and show the most
#   common options by default
#
#   Revision 1.8  2008/08/27 15:26:52  marshm
#   Updates to SequenceView.  Updated coloring scheme for correspondence matches.
#
#   Revision 1.7  2008/07/28 16:19:22  ssa1
#   Adding in correspondance data repository
#
#   Revision 1.6  2008/07/01 13:41:39  marshm
#   made changes to accomodate seq_model.py:Chain object for calpha-backbone models
#
#   Revision 1.5  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from libpyGORGON import PDBAtom, Vector3DFloat
from seq_model.Residue import Residue

class CAlphaAtomPlacerForm(QtGui.QWidget):
    def __init__(self, main, viewer, main_chain, structPred, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.connect(self.skeletonViewer, QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.skeletonSelected)
        self.main_chain=main_chain
        self.structPred = structPred
        self.createUI()
        self.createActions()
        self.createMenus()
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.addAtomPushButton, QtCore.SIGNAL("pressed()"), self.addAtom)
        self.connect(self.resSeqNumSpinBox,  QtCore.SIGNAL('valueChanged(int)'), self.updateResName)

    def createUI(self):   
        self.dock = QtGui.QDockWidget(self.tr("C-Alpha - Atom Placer"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        
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
            if self.structPred.chain:
                self.resSeqNumSpinBox.setRange( min(self.structPred.chain.residueRange()), max(self.structPred.chain.residueRange()) )
            else:
                return            
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.skeletonViewer.setSelectEnabled(True)
            if not self.viewer.main_chain:
                self.viewer.main_chain = self.viewer.structPred.chain
            self.main_chain = self.viewer.main_chain
            residue = self.structPred.chain[self.resSeqNumSpinBox.value()]
            self.resNameLabel.setText(residue.symbol3)
            self.identifierLabel.setText( self.main_chain.getPdbID() + ' chain ' + self.main_chain.getChainID() )
            self.dock.show()
        else:
            self.skeletonViewer.setSelectEnabled(False)
            self.app.removeDockWidget(self.dock)
            
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("perform_CAlphaManualAtomPlacement").setChecked(visible)
        
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
    
    def createActions(self):
        placeAct = QtGui.QAction(self.tr("&Manual C-Alpha Atom Placement"), self)
        placeAct.setStatusTip(self.tr("Perform Manual C-Alpha atom placement"))
        placeAct.setCheckable(True)
        placeAct.setChecked(False)
        self.connect(placeAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("perform_CAlphaManualAtomPlacement", placeAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-calpha-manualAtomPlacement", self.app.actions.getAction("perform_CAlphaManualAtomPlacement"), "actions-calpha")
    
    def updateResName(self, index):
        print 'in updateResName'
        self.resNameLabel.setText(self.structPred.chain[index].symbol3)
