# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to build SSEs from SSE Hunter results


from PyQt4 import QtCore, QtGui
from ui_dialog_volume_sse_builder import Ui_DialogVolumeSSEBuilder
from base_dock_widget import BaseDockWidget
from sse_hunter_engine import SSEHunterEngine
from auto_helix_builder_engine import AutoHelixBuilderEngine

class VolumeSSEBuilderForm(BaseDockWidget, Ui_DialogVolumeSSEBuilder):
        
    def __init__(self, main, viewer, parent=None):
        BaseDockWidget.__init__(self, 
                                main, 
                                "Identify &SSEs", 
                                "Identify secondary structure elements", 
                                "detectSSE_Volume", 
                                "actions-sse-detectSSE", 
                                "actions-sse", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.RightDockWidgetArea, 
                                parent)
        self.app = main
        self.viewer = viewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)

        
        self.createUI()
        self.createActions()

    def createUI(self):
        self.setupUi(self)
        
        self.connect(self.pushButtonBrowseAtomScore, QtCore.SIGNAL("clicked (bool)"), self.browseAtomScoreFile)
        self.connect(self.pushButtonSelectionToHelix, QtCore.SIGNAL("clicked (bool)"), self.selectionToHelix)
        self.connect(self.pushButtonSelectionToSheet, QtCore.SIGNAL("clicked (bool)"), self.selectionToSheet)
        self.connect(self.pushButtonRemoveSSE, QtCore.SIGNAL("clicked (bool)"), self.removeSSE)        
        self.connect(self.pushButtonSSEHunter, QtCore.SIGNAL("clicked (bool)"), self.runSSEHunter)
        self.connect(self.pushButtonLoadVolume, QtCore.SIGNAL("clicked (bool)"), self.loadVolume)
        self.connect(self.pushButtonSavePseudoatoms, QtCore.SIGNAL("clicked (bool)"), self.savePseudoatoms)
        self.connect(self.pushButtonLoadSkeleton, QtCore.SIGNAL("clicked (bool)"), self.loadSkeleton)
        self.connect(self.pushButtonAddHelices, QtCore.SIGNAL("clicked (bool)"), self.autoBuildHelices)
        self.connect(self.pushButtonRemoveHelices, QtCore.SIGNAL("clicked (bool)"), self.removeHelices)
        self.connect(self.doubleSpinBoxCorrelation, QtCore.SIGNAL("valueChanged(double)"), self.updateTotalScoreSSEHunterAtoms)
        self.connect(self.doubleSpinBoxSkeleton, QtCore.SIGNAL("valueChanged(double)"), self.updateTotalScoreSSEHunterAtoms)
        self.connect(self.doubleSpinBoxGeometry, QtCore.SIGNAL("valueChanged(double)"), self.updateTotalScoreSSEHunterAtoms)
        self.connect(self.app.viewers["volume"], QtCore.SIGNAL("modelLoaded()"), self.enableDisableSSEHunter)
        self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("modelLoaded()"), self.enableDisableSSEHunter)
        self.connect(self.app.viewers["volume"], QtCore.SIGNAL("modelUnloaded()"), self.enableDisableSSEHunter)
        self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("modelUnloaded()"), self.enableDisableSSEHunter)
        
    
    def disableSavePseudoatoms(self):
        self.pushButtonSavePseudoatoms.setEnabled(False)        
    
    def savePseudoatoms(self, temp):
        fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Pseudoatoms"), "", self.tr("Protein Data Bank (PDB) Format (*.pdb)"))
        if not fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            if not(self.app.viewers["calpha"].renderer.saveSSEHunterFile(str(fileName))):
                # TODO: Put a error message here telling the user that the save failed
                pass
            self.setCursor(QtCore.Qt.ArrowCursor)     
    
    def loadVolume(self, temp):
        self.app.actions.getAction("load_Volume").trigger()
        self.bringToFront()
        
    def loadSkeleton(self, temp):
        self.app.actions.getAction("load_Skeleton").trigger()
        self.bringToFront()

    def removeHelices(self):
        self.viewer.renderer.removeHelices()
        self.viewer.emitModelChanged()
        
    def removeSheets(self):
        self.viewer.renderer.removeSheets()
        self.viewer.emitModelChanged()
        
    def removeSSE(self, temp):
        if(QtGui.QMessageBox.question(self, "Remove Selected SSEs?", "This will remove the selected SSEs. Are you sure?", "Yes", "Cancel") == 0) :
            self.viewer.renderer.removeSelectedSSEs()
            self.viewer.emitModelLoaded() 
            self.bringToFront()
                                                                                                                                        

    def createActions(self):    
        self.detectSSEAct = self.displayAct

    def modelLoaded(self):
        #self.detectSSEAct.setEnabled(True)
        pass
       
    def modelUnloaded(self):
        #self.detectSSEAct.setEnabled(False)
        self.showWidget(False)    

    def dockVisibilityChanged(self, visible):
        BaseDockWidget.dockVisibilityChanged(self, visible)
        self.app.viewers["calpha"].centerOnRMB = not visible
        if(visible):
            self.connect(self.app.viewers["calpha"], QtCore.SIGNAL("atomSelectionUpdated(PyQt_PyObject)"), self.atomSelectionChanged)
        else:
            self.disconnect(self.app.viewers["calpha"], QtCore.SIGNAL("atomSelectionUpdated(PyQt_PyObject)"), self.atomSelectionChanged)


    def browseAtomScoreFile(self, result):
        pdbFile = QtGui.QFileDialog.getOpenFileName(self, self.tr("Load SSEHunter Results"), "", self.tr("PDB Files (*.pdb)"))
        if not pdbFile.isEmpty():
            self.calphaViewer = self.app.viewers["calpha"]
            self.calphaViewer.loadSSEHunterData(pdbFile)
            self.sseViewer = self.app.viewers["sse"]
            self.lineEditAtomScore.setText(pdbFile)
            self.connect(self.app.viewers["calpha"],  QtCore.SIGNAL("modelUnloaded()"), self.disableSavePseudoatoms)                    
            self.pushButtonSavePseudoatoms.setEnabled(True)
        self.bringToFront()
        
    def autoBuildHelices(self):
        print "VolumeSSEBuilderForm.autoBuildHelices()"
        self.calphaViewer = self.app.viewers["calpha"]
        patom_hashkeys = self.calphaViewer.renderer.getAtomHashes();
        patoms = [self.calphaViewer.renderer.getAtom(hashkey) for hashkey in patom_hashkeys]
        
        score_thresh = self.doubleSpinBoxScoreThresh.value()
        pt_line_dist_thresh = self.horizontalSliderLinearityThresh.value() / 4.0
        auto_helix_builder = AutoHelixBuilderEngine(patoms, score_thresh, pt_line_dist_thresh)
        helix_list = auto_helix_builder.get_helix_list()
        print helix_list
        for helix in helix_list:
            for atom in helix:
                atom.setSelected(True)
            self.selectionToHelix(None)
            for atom in helix:
                atom.setSelected(False)
        

    def runSSEHunter(self, result):
        self.calphaViewer = self.app.viewers["calpha"]
        self.sseViewer = self.app.viewers["sse"]
        threshold = self.doubleSpinBoxThreshold.value()
        resolution = self.doubleSpinBoxResolution.value()
        correlationWeight = self.doubleSpinBoxCorrelation.value()
        skeletonWeight = self.doubleSpinBoxSkeleton.value()
        geometryWeight = self.doubleSpinBoxGeometry.value()


        #self.calphaViewer.runSSEHunter( threshold, resolution, correlationWeight, skeletonWeight, geometryWeight )


        vol = self.app.viewers["volume"].renderer.getVolume()
        skel = self.app.viewers["skeleton"].renderer.getMesh()
        sseh = SSEHunterEngine(vol, skel, resolution, threshold)
        patoms = sseh.getScoredAtoms(correlationWeight, skeletonWeight, geometryWeight)
        
        for pseudoatom in patoms:
            self.calphaViewer.renderer.addAtom(pseudoatom)
        
        self.calphaViewer.renderer.colorSSEHunterAtoms()
        self.calphaViewer.dirty = False
        self.calphaViewer.loaded = True
        self.calphaViewer.emitModelLoadedPreDraw()
        self.calphaViewer.emitModelLoaded()
        self.calphaViewer.emitViewerSetCenter()
        self.connect(self.app.viewers["calpha"],  QtCore.SIGNAL("modelUnloaded()"), self.disableSavePseudoatoms)        
        self.pushButtonSavePseudoatoms.setEnabled(True)
        self.bringToFront()
        
		
    def updateTotalScoreSSEHunterAtoms(self):
        self.calphaViewer.updateTotalScoreSSEHunterAtoms( self.doubleSpinBoxCorrelation.value(), self.doubleSpinBoxSkeleton.value(), 
            self.doubleSpinBoxGeometry.value() )
        
    def atomSelectionChanged(self, selection):
        self.tableWidgetSelection.clearContents()
        self.calphaViewer = self.app.viewers["calpha"]
        atomCnt = self.calphaViewer.renderer.selectionAtomCount()
        self.tableWidgetSelection.setRowCount(atomCnt)
        
        for i in range(atomCnt):
            atom = self.calphaViewer.renderer.getSelectedAtom(i)
            self.tableWidgetSelection.setItem(i, 0, QtGui.QTableWidgetItem(str(atom.getResSeq())))
            self.tableWidgetSelection.setItem(i, 1, QtGui.QTableWidgetItem(str(atom.getTempFactor())))


    def getHelixEnds(self, atoms):
        distance = 0
        ends = []
        for pt1 in atoms:
            pos1 = pt1.getPosition()
            for pt2 in atoms:
                pos2 = pt2.getPosition()
                atomDist = (pos2 - pos1).length()
                if(atomDist > distance):
                    distance = atomDist
                    ends = [pos1, pos2]
        return ends
                
    
    def selectionToHelix(self, result):
        self.pushAtomsToEngine()
            
        self.sseViewer.renderer.finalizeHelix()
        
        if(self.sseViewer.loaded):
            self.sseViewer.helixLoaded = True
            self.sseViewer.dirty = True           
            self.sseViewer.emitModelChanged()
        else :
            self.sseViewer.loaded = True
            self.sseViewer.helixLoaded = True
            self.sseViewer.dirty = True
            self.sseViewer.emitModelLoadedPreDraw()
            self.sseViewer.emitModelLoaded()     
        self.bringToFront()      
    
    def pushAtomsToEngine(self):
        atomCnt = self.calphaViewer.renderer.selectionAtomCount()
        
        self.sseViewer.renderer.startNewSSE();
        
        for i in range(atomCnt):
            atom = self.calphaViewer.renderer.getSelectedAtom(i)
            position = atom.getPosition()
            self.sseViewer.renderer.addSSEPoint(position)
        
    def selectionToSheet(self, result):
        self.pushAtomsToEngine()
            
        self.sseViewer.renderer.finalizeSheet()
        
        if(self.sseViewer.loaded):
            self.sseViewer.sheetLoaded = True
            self.sseViewer.dirty = True           
            self.sseViewer.emitModelChanged()
        else :
            self.sseViewer.loaded = True
            self.sseViewer.sheetLoaded = True
            self.sseViewer.dirty = True
            self.sseViewer.emitModelLoadedPreDraw()
            self.sseViewer.emitModelLoaded()    
        self.bringToFront()
        
    def enableDisableSSEHunter(self):
        volumeViewer =  self.app.viewers["volume"]
        skeletonViewer = self.app.viewers["skeleton"]
        enabled = (volumeViewer.loaded and skeletonViewer.loaded)
        self.labelThreshold.setEnabled(enabled)
        self.labelVolumeResolution.setEnabled(enabled)
        self.labelSkeletonScore.setEnabled(enabled)
        self.labelCrossCorrelationScore.setEnabled(enabled)
        self.labelGeometricScore.setEnabled(enabled)
        self.doubleSpinBoxThreshold.setEnabled(enabled)
        self.doubleSpinBoxResolution.setEnabled(enabled)
        self.doubleSpinBoxSkeleton.setEnabled(enabled)
        self.doubleSpinBoxCorrelation.setEnabled(enabled)
        self.doubleSpinBoxGeometry.setEnabled(enabled)
        self.pushButtonSSEHunter.setEnabled(enabled)
        self.pushButtonLoadVolume.setVisible(not volumeViewer.loaded)
        self.pushButtonLoadSkeleton.setVisible(not skeletonViewer.loaded)
