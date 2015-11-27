# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   This class creates the QUndoCommand objects for the undo/redo stack.


from PyQt4 import QtGui, QtCore
from libpyGORGON import PDBAtom, PDBBond, Vector3DFloat
from calpha_interactive_loop_builder import CAlphaInteractiveLoopBuilder
from seq_model.findHelixCalphas import helixEndpointsToCAlphaPositions
from seq_model.Helix import Helix
import math
from vector_lib import *

class CAlphaStructureEditorCommandAtomPlacement(QtGui.QUndoCommand):
    def __init__(self, currentChainModel, structureEditor, resSeqNum, chosenCoordinates, viewer, description=None):
        super(CAlphaStructureEditorCommandAtomPlacement, self).__init__(description)
        self.currentChainModel = currentChainModel
        self.structureEditor = structureEditor
        self.resSeqNum = resSeqNum
        self.chosenCoordinates = chosenCoordinates
        self.viewer = viewer
        
    def redo(self):
        """
In addition to being called to redo an action, this is called the first
time the action occurs.
        """
        raw = PDBAtom(self.currentChainModel.getPdbID(), self.currentChainModel.getChainID(), self.resSeqNum, 'CA')
        raw.setPosition(self.chosenCoordinates)
        atom = self.viewer.renderer.addAtom(raw) 
        self.currentChainModel[self.resSeqNum].addAtomObject(atom)
        self.currentChainModel[self.resSeqNum].setCAlphaColorToDefault() 
        bondBefore = None
        bondAfter = None
        if self.resSeqNum - 1 in self.currentChainModel.residueRange():
            prevCAlpha = self.currentChainModel[self.resSeqNum - 1].getAtom('CA')
            if prevCAlpha:
                bondBefore=PDBBond()
                bondBefore.setAtom0Ix(prevCAlpha.getHashKey())
                bondBefore.setAtom1Ix(atom.getHashKey())
        if self.resSeqNum + 1 in self.currentChainModel.residueRange():
            nextCAlpha = self.currentChainModel[self.resSeqNum + 1].getAtom('CA')
            if nextCAlpha:
                bondAfter = PDBBond()
                bondAfter.setAtom0Ix(nextCAlpha.getHashKey())
                bondAfter.setAtom1Ix(atom.getHashKey())
        
        if bondBefore:
            self.viewer.renderer.addBond(bondBefore)
        if bondAfter:
            self.viewer.renderer.addBond(bondAfter)
        
        self.viewer.emitModelChanged()
        self.structureEditor.atomJustAdded = atom
        
        if self.structureEditor.atomicBackwardRadioButton.isChecked():
            self.structureEditor.atomPrevButtonPress()
        elif self.structureEditor.atomicForwardRadioButton.isChecked():
            self.structureEditor.atomNextButtonPress()
        
    def undo(self):
        atom = self.currentChainModel[self.resSeqNum].getAtom('CA')
        
        try:
            atomBefore = self.currentChainModel[self.resSeqNum-1].getAtom('CA')
        except (IndexError, AttributeError, KeyError):
            atomBefore = None
        try:
            atomAfter = self.currentChainModel[self.resSeqNum+1].getAtom('CA')
        except (IndexError, AttributeError, KeyError):
            atomAfter = None
        
        if atomBefore:
            bondBeforeIx = self.viewer.renderer.getBondIndex(atomBefore.getHashKey(), atom.getHashKey())
            self.viewer.renderer.deleteBond(bondBeforeIx)
            pass
        if atomAfter:
            bondAfterIx = self.viewer.renderer.getBondIndex(atomAfter.getHashKey(), atom.getHashKey())
            self.viewer.renderer.deleteBond(bondAfterIx)
            pass
            
        self.currentChainModel[self.resSeqNum].clearAtom('CA')
        self.viewer.renderer.deleteAtom(atom.getHashKey())
        
        self.viewer.emitModelChanged()
        
        if self.structureEditor.atomicBackwardRadioButton.isChecked():
            self.structureEditor.atomNextButtonPress()
        elif self.structureEditor.atomicForwardRadioButton.isChecked():
            self.structureEditor.atomPrevButtonPress()
