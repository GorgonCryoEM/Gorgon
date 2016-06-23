from PyQt4 import QtGui, QtCore
from libpytoolkit import PDBAtom, PDBBond
# from calpha_interactive_loop_builder import CAlphaInteractiveLoopBuilder
from Toolkit.sse.seq_model.findHelixCalphas import helixEndpointsToCAlphaPositions
from Toolkit.sse.seq_model.Helix import Helix
# import math
# from vector_lib import *


def place_helix(currentChainModel, predHelix, startIndex, stopIndex, coord1, coord2, structureEditor, structurePrediction, description=None):
    helix = Helix(currentChainModel, predHelix.serialNo, predHelix.label, startIndex, stopIndex)
    CAlphaViewer = structureEditor.CAlphaViewer
    currentChainModel.addHelix(predHelix.serialNo, helix)
    helix.setAxisPoints(coord1, coord2)

    helixCoordList = helixEndpointsToCAlphaPositions(coord1, coord2)

    for i in range(min(len(helixCoordList), stopIndex - startIndex + 1)):
        pos = helixCoordList[i]
        residue = currentChainModel[startIndex + i]
        rawAtom = residue.addAtom('CA', pos[0], pos[1], pos[2], 'C')
        atom = CAlphaViewer.renderer.addAtom(rawAtom)
        residue.addAtomObject(atom)
        atom.setSelected(True)
        try:
            prevAtom = currentChainModel[startIndex + i - 1].getAtom('CA')
            bond = PDBBond()
            bond.setAtom0Ix(prevAtom.getHashKey())
            bond.setAtom1Ix(atom.getHashKey())
            CAlphaViewer.renderer.addBond(bond)
        except (KeyError, IndexError, AttributeError):
            continue

    try:
        nextAtom = currentChainModel[startIndex + len(helixCoordList)]
        bond = PDBBond()
        bond.setAtom0Ix(atom.getHashKey())
        bond.setAtom1Ix(nextAtom.getHashKey())
        CAlphaViewer.renderer.addBond(bond)
    except (KeyError, IndexError, AttributeError):
        pass

    currentChainModel.setSelection(newSelection=range(startIndex, 1 + stopIndex))

    if not CAlphaViewer.loaded:
        CAlphaViewer.loaded = True
    #             self.CAlphaViewer.modelChanged()
    #         else:
    #             self.CAlphaViewer.modelChanged()


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
        place_helix(self.currentChainModel, self.predHelix, self.startIndex, self.stopIndex, self.coord1, self.coord2, self.structureEditor, self.structurePrediction)

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
            self.CAlphaViewer.modelChanged()
        else:
            self.CAlphaViewer.emitModelChanged()
