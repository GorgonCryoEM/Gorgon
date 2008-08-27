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
from ui_dialog_calpha_atom_placer import Ui_DialogCAlphaAtomPlacer
from libpyGORGON import PDBAtom, Vector3DFloat
from seq_model.GAtom import GAtom
from seq_model.Residue import Residue

class CAlphaAtomPlacerForm(QtGui.QWidget):
    def __init__(self, main, viewer, main_chain, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.connect(self.skeletonViewer, QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.skeletonSelected)
        self.createUI()
        self.createActions()
        self.createMenus()
        self.main_chain=main_chain

    def createUI(self):
        self.ui = Ui_DialogCAlphaAtomPlacer()
        self.ui.setupUi(self)       
        self.dock = QtGui.QDockWidget(self.tr("C-Alpha - Atom Placer"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.pushButtonAddAtom, QtCore.SIGNAL("pressed()"), self.addAtom)
        
    def loadWidget(self):
        if(self.app.actions.getAction("perform_CAlphaManualAtomPlacement").isChecked()) :
            self.app.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.dock)
            self.skeletonViewer.setSelectEnabled(True)
            self.dock.show()
        else:
            self.skeletonViewer.setSelectEnabled(False)
            self.app.removeDockWidget(self.dock)
            
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("perform_CAlphaManualAtomPlacement").setChecked(visible)
        
    def skeletonSelected(self, h0, h1, h2, h3, h4, h5, event):
        position = self.skeletonViewer.renderer.get3DCoordinates(h0, h1, h2, h3, h4, h5)
        self.ui.doubleSpinBoxPositionX.setValue(position.x());
        self.ui.doubleSpinBoxPositionY.setValue(position.y());
        self.ui.doubleSpinBoxPositionZ.setValue(position.z());
    
    def addAtom(self):
				'''
        atom.setSerial(self.ui.spinBoxSerial.value())
        #TODO: Find out how to convert QStrings to Std:Strings
        atom.setName(str(self.ui.lineEditAtomName.text()))
        #atom.setAltLoc(self.ui.lineEditAlternateLocation.text()[0])
        atom.setResName(str(self.ui.lineEditResidueName.text()))
        #atom.setChainId(self.ui.lineEditChainIdentifier.text()[0])
        atom.setResSeq(self.ui.spinBoxResidueSequenceNo.value())
        #atom.setICode(self.ui.lineEditInsertionCode.text()[0])
        atom.setPosition(Vector3DFloat(self.ui.doubleSpinBoxPositionX.value(), 
                                       self.ui.doubleSpinBoxPositionY.value(),
                                       self.ui.doubleSpinBoxPositionZ.value()))
				'''
				#Atom attributes
				x,y,z = self.ui.doubleSpinBoxPositionX.value(), self.ui.doubleSpinBoxPositionY.value(), self.ui.doubleSpinBoxPositionZ.value() 
				element= str(self.ui.lineEditElement.text())
				occupancy = self.ui.doubleSpinBoxOccupancy.value()
				temp_factor = self.ui.doubleSpinBoxTemperatureFactor.value() 
				atom_name=str(self.ui.lineEditAtomName.text())

				#Residue attributes
				residue=Residue(str(self.ui.lineEditResidueName.text()))
				residue.atoms[atom_name]=GAtom(element, x,y,z, self.viewer, occupancy, temp_factor)
				index=int(self.ui.spinBoxResidueSequenceNo.value())

				#Add residue to main_chain
				self.main_chain[index] = residue
				print self.main_chain.to_pdb()
				

				#Add residue to main_chain
				self.ui.spinBoxResidueSequenceNo.setValue(self.ui.spinBoxResidueSequenceNo.value()+1)
    def createActions(self):               
        placeAct = QtGui.QAction(self.tr("&Manual C-Alpha Atom Placement"), self)
        placeAct.setStatusTip(self.tr("Perform Manual C-Alpha atom placement"))
        placeAct.setCheckable(True)
        placeAct.setChecked(False)
        self.connect(placeAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("perform_CAlphaManualAtomPlacement", placeAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-calpha-manualAtomPlacement", self.app.actions.getAction("perform_CAlphaManualAtomPlacement"), "actions-calpha")        
    
        
