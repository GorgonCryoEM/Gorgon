# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   This QDockWidget contains a CAlphaSequenceWidget. Its methods handle much of the interaction with the other parts of the Gorgon application.


from PyQt4 import Qt,QtGui,QtCore
from calpha_sequence_widget import CAlphaSequenceWidget
from calpha_sequence_error import CAlphaSequenceError


class CAlphaSequenceDock(QtGui.QDockWidget):
    __dock = None
    
    def __init__(self, main, viewer, structurePrediction, currentChainModel, parent=None):
        super(CAlphaSequenceDock, self).__init__("Semi-automatic atom placement", parent)
        
        self.currentChainModel = currentChainModel
        self.structurePrediction = structurePrediction
        self.app = main
        self.viewer=viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.sseViewer = self.app.viewers["sse"]
        self.seqWidget = CAlphaSequenceWidget( structurePrediction, currentChainModel, self, self)
        self.setWidget(self.seqWidget)
        self.createActions()
        CAlphaSequenceDock.__dock = self
        self.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.connect(self.seqWidget.structureEditor.mockSidechainsCheckBox,  QtCore.SIGNAL('stateChanged(int)'),  self.toggleMockSideChains)
        if main:
            self.connect(self.app.viewers["calpha"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.updateFromViewerSelection)
            self.connect(self.app.viewers["sse"], QtCore.SIGNAL("SSE selected"), self.updateFromSSESelection)
    
    @classmethod
    def changeDockVisibility(cls, main, viewer, structurePrediction, currentChainModel):
        if not structurePrediction.chain.residueRange():
            return
        if not currentChainModel.residueRange():
            currentChainModel = Chain('', main)
            for i in structurePrediction.chain.residueRange():
                #This will not copy PDBAtom objects, but it will copy (not reference) the residue objects and place them in the new chain
                #It will not copy any Secel objects (Helix, Strand, Sheet, Coil, Secel)
                #Thus, it it prepares a clean chain object to begin building a model
                resSymbol3 = structurePrediction.chain[i].symbol3
                res = Residue(resSymbol3, currentChainModel)
                currentChainModel[i] = res
                
        if cls.__dock:
            if cls.__dock.app.actions.getAction("seqDock").isChecked():
                try:
                    CAlphaSequenceDock.checkPredictionVsModel(structurePrediction, currentChainModel)
                except CAlphaSequenceError:
                    QtGui.QMessageBox.warning(main, "Chain model does not match the sequence",  
                    "The sequence of the current chain model (from PDB file) does not match the \
                    sequence of the structure prediction (from SEQ file)" )
                    cls.__dock.app.actions.getAction("seqDock").setChecked(False)
                    return
                cls.__dock.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, cls.__dock)
                cls.__dock.changeCurrentChainModel(currentChainModel)
                cls.__dock.show()
            else:
                cls.__dock.app.removeDockWidget(cls.__dock)
        else:
            if main and viewer:
                try:
                    CAlphaSequenceDock.checkPredictionVsModel(structurePrediction, currentChainModel)
                except CAlphaSequenceError:
                    QtGui.QMessageBox.warning(main, "Chain model does not match the sequence",  
                    "The sequence of the current chain model (from PDB file) does not match the \
                    sequence of the structure prediction (from SEQ file)" )
                    return
                
                dock = CAlphaSequenceDock(main, viewer, structurePrediction, currentChainModel)
                main.addDockWidget(QtCore.Qt.RightDockWidgetArea, dock)
                dock.show()
                dock.raise_()
            else:
                if not main: 
                    print 'Sequence Dock Error: no main app'
                if not viewer: 
                    print 'Sequence Dock Error: no viewer'
                if not currentChainModel: 
                    print 'Sequence Dock: no chain to load'
    
    @classmethod
    def checkPredictionVsModel(cls, structurePrediction, currentChainModel):
        """
If the chain model's sequence is a subset of the structure prediction's
    sequence, the model is simply incomplete, and this function adds 
    the missing residues to the model, without adding any atoms to 
    those residues. 
If the sequences are equal, the model will work, and this function does
    nothing. 
If the chain model's sequence is not a subset of the structure 
    prediction's sequence, a SequenceError is raised, because the model
    can not work with the structure prediction.
        """
        modelResNumSet = set(currentChainModel.residueRange())
        predResNumSet = set(structurePrediction.chain.residueRange())
        if modelResNumSet != predResNumSet: 
            #If the sequence of the model is a subset of the structure prediction sequence, we can work with it
            if modelResNumSet.issubset(predResNumSet):
                modelIsMissing = predResNumSet.difference(modelResNumSet)
                for resNum in modelResNumSet:
                    modelResName = currentChainModel[resNum].symbol3
                    predResName = structurePrediction.chain[resNum].symbol3
                    if modelResName != predResName: #If the residue names aren't the same, it isn't a subset
                        print resNum, ':', modelResName, 'vs', predResName
                        raise CAlphaSequenceError
                        break
                for resNum in modelIsMissing:
                    #If the model is missing some residues that are in the prediction, add them to the model
                    resName = structurePrediction.chain[resNum].symbol3
                    currentChainModel[resNum] = Residue(resName, currentChainModel)                        
            else: #If the model has some residue numbers that the structure prediction doesn't, it must be a model of something else
                print "Model sequence is not a subset of the structure prediction sequence!"
                raise CAlphaSequenceError
    
    def createActions(self):
        seqDockAct = QtGui.QAction(self.tr("Partly &Automated Atom Placement"), self)
        self.seqDockAct = seqDockAct
        seqDockAct.setStatusTip(self.tr("Place atoms based on predicted SSE's"))
        seqDockAct.setCheckable(True)
        seqDockAct.setChecked(False)
        self.connect(seqDockAct, QtCore.SIGNAL("triggered()"), CAlphaSequenceDock.changeDockVisibility)
        self.app.actions.addAction("perform_autoAtomPlacement", seqDockAct)
    
    def changeCurrentChainModel(self, currentChainModel):
        '''
This changes the Chain object that is displayed and is edited by the 
user.  A chain object represents a possible structure model.
        '''
        self.currentChainModel = currentChainModel
        self.seqWidget.currentChainModel = currentChainModel
        seqView = self.seqWidget.scrollable.seqView
        seqView.currentChainModel = currentChainModel
        seqView.repaint()
        structureEditor = self.seqWidget.structureEditor
        structureEditor.currentChainModel = currentChainModel        
        for i in structureEditor.atomicResNumbers.keys():
            structureEditor.atomicResNumbers[i].setText('?')
            structureEditor.atomicResNames[i].setText('?')
    
    def changeSequenceSSE(self, structurePrediction):
        '''
This changes the structure prediction which has information about the
sequence and predicted residue indices for helices and strands.
        '''
        self.structurePrediction = structurePrediction
    
    def closeEvent(self, event):
        self.app.actions.getAction("seqDock").setChecked(False)
        
    def updateFromViewerSelection(self):
        '''
This responds to a new selection of atoms in the viewer.
        '''
        self.seqWidget.scrollable.seqView.updateSequenceSelection()
    
    def updateFromSSESelection(self):
        self.currentChainModel.setSelection(newSelection=range(self.sseViewer.currentMatch.predicted.startIndex, self.sseViewer.currentMatch.predicted.stopIndex+1))
        self.seqWidget.scrollable.seqView.updateSequenceSelection()        
    
    def toggleMockSideChains(self):
        '''
This toggles whether mock side-chains are displayed.  Mock side-chains
represent residues as spheres with relative size of a residue indicated
by radius and residue type indicated by color. 
        '''
        viewer = self.viewer
        
        if self.seqWidget.structureEditor.mockSidechainsCheckBox.isChecked():
            self.seqWidget.structureEditor.renderMockSidechains(self.currentChainModel)
        else:
            self.seqWidget.structureEditor.clearMockSidechains(self.currentChainModel)         
                
        viewer.emitModelChanged()
