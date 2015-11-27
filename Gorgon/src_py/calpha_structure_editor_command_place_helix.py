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

class CAlphaStructureEditorCommandPlaceHelix(QtGui.QUndoCommand):
    def __init__(self, currentChainModel, predHelix, startIndex, stopIndex, coord1, coord2, structureEditor, structurePrediction, description=None):
        super(CAlphaStructureEditorCommandPlaceHelix, self).__init__(description)
        self.currentChainModel = currentChainModel
        self.predHelix = predHelix
        self.startIndex = startIndex
        self.stopIndex = stopIndex
        self.coord1 = coord1
        self.coord2 = coord2
        self.structureEditor = structureEditor
        self.structurePrediction = structurePrediction
        self.CAlphaViewer = self.structureEditor.CAlphaViewer
    
    def redo(self):
                        
        self.helix = Helix(self.currentChainModel, self.predHelix.serialNo, self.predHelix.label, self.startIndex, self.stopIndex)
        self.currentChainModel.addHelix(self.predHelix.serialNo, self.helix)
        self.helix.setAxisPoints(self.coord1, self.coord2)
        
        helixCoordList = helixEndpointsToCAlphaPositions(self.coord1, self.coord2)
        
        '''
        #To see the ends of the helical axis as green and red atoms
        startAtom = PDBAtom('AAAA', 'A', 100000, 'CA')
        startAtom.setPosition(Vector3DFloat(*coord1))
        startAtom.setColor(0, 1, 0, 1)
        startAtom = self.CAlphaViewer.renderer.addAtom(startAtom)
        stopAtom = startAtom = PDBAtom('AAAA', 'A', 100001, 'CA')
        stopAtom.setPosition(Vector3DFloat(*coord2))
        stopAtom.setColor(1, 0, 0, 1)        
        stopAtom = self.CAlphaViewer.renderer.addAtom(stopAtom)
        '''
        
        for i in range(min(len(helixCoordList), self.stopIndex - self.startIndex + 1)):
            pos = helixCoordList[i]
            residue = self.currentChainModel[self.startIndex+i]
            rawAtom = residue.addAtom('CA', pos[0], pos[1], pos[2], 'C')
            atom = self.CAlphaViewer.renderer.addAtom(rawAtom)
            residue.addAtomObject(atom)
            atom.setSelected(True)
            try:
                prevAtom = self.currentChainModel[self.startIndex+i-1].getAtom('CA')
                bond = PDBBond()
                bond.setAtom0Ix(prevAtom.getHashKey())
                bond.setAtom1Ix(atom.getHashKey())
                self.CAlphaViewer.renderer.addBond(bond)
            except (KeyError, IndexError, AttributeError):
                continue

        try:
            nextAtom = self.currentChainModel[self.startIndex+len(helixCoordList)]
            bond = PDBBond()
            bond.setAtom0Ix(atom.getHashKey())
            bond.setAtom1Ix(nextAtom.getHashKey())
            self.CAlphaViewer.renderer.addBond(bond)
        except (KeyError, IndexError, AttributeError):
            pass
          
        
        self.currentChainModel.setSelection(newSelection = range(self.startIndex, 1+self.stopIndex))
        
        if not self.CAlphaViewer.loaded:
            self.CAlphaViewer.loaded = True
            self.CAlphaViewer.emitModelLoaded()
        else:
            self.CAlphaViewer.emitModelChanged()
        
    def undo(self):
        for resNum in range(self.startIndex, 1+self.stopIndex):
            
            try:
                atom = self.currentChainModel[resNum].getAtom('CA')
            except (KeyError, IndexError, AttributeError):
                atom = None
            if atom:
                
                try:
                    atomBefore = self.currentChainModel[resNum-1].getAtom('CA')
                except (KeyError, IndexError, AttributeError):
                    atomBefore = None
                if atomBefore:
                    bondBeforeIx = self.CAlphaViewer.renderer.getBondIndex(atomBefore.getHashKey(), atom.getHashKey())
                    if bondBeforeIx != -1:
                        self.CAlphaViewer.renderer.deleteBond(bondBeforeIx)
                
                try:
                    atomAfter = self.currentChainModel[resNum+1].getAtom('CA')
                except (KeyError, IndexError, AttributeError):
                    atomAfter = None
                if atomAfter:
                    bondAfterIx = self.CAlphaViewer.renderer.getBondIndex(atomAfter.getHashKey(), atom.getHashKey())
                    if bondAfterIx != -1:
                        self.CAlphaViewer.renderer.deleteBond(bondAfterIx)
        
        for resNum in range(self.startIndex, 1+self.stopIndex):
            try:
                atom = self.currentChainModel[resNum].getAtom('CA')
            except (IndexError, AttributeError, KeyError):
                continue
            self.CAlphaViewer.renderer.deleteAtom(atom.getHashKey())                
            self.currentChainModel[resNum].clearAtom('CA')
        
        self.currentChainModel.removeSecel(self.helix)
        
        if not self.CAlphaViewer.loaded:
            self.CAlphaViewer.loaded = True
            self.CAlphaViewer.emitModelLoaded()
        else:
            self.CAlphaViewer.emitModelChanged()
