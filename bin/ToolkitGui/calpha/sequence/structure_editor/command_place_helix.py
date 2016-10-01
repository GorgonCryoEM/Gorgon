from PyQt4 import QtGui
# from calpha_interactive_loop_builder import CAlphaInteractiveLoopBuilder
# import math
# from vector_lib import *
from ToolkitGui.helix import place_helix


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
        place_helix(self.structureEditor, self.currentChainModel, self.predHelix, self.coord1, self.coord2)

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
