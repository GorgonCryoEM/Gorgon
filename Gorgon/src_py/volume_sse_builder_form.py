# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to build SSEs from SSE Hunter results

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.15  2010/02/11 23:19:13  ssa1
#   Allowing the ability to save pseudoatoms generated from SSEHunter
#
#   Revision 1.14  2010/01/16 22:34:29  colemanr
#   using the new SSEHunterEngine Python class
#
#   Revision 1.13  2010/01/16 20:05:16  colemanr
#   moving the total score calculation of SSEHunter to Python
#
#   Revision 1.12  2010/01/14 23:34:25  ssa1
#   Allowing the deletion of SSEs from the SSEBuilder window
#
#   Revision 1.11  2010/01/10 05:31:43  colemanr
#   PDBAtoms now store their correlation, skeleton, and geometry scores. Changing the weighting for these three scores in the GUI now changes the total score for each pseudoatom.
#
#   Revision 1.10  2009/12/31 06:09:09  ssa1
#   Fixing bug where Helix save is disabled when helix is created using SSEBuilder
#
#   Revision 1.9  2009/12/28 19:22:05  ssa1
#   Fixing SSE Builder being disabled when model is unloaded.
#
#   Revision 1.8  2009/12/24 07:25:07  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior
#
#   Revision 1.7  2009/09/21 19:03:22  ssa1
#   Linear least squares fit implementation, and using it in helix positioning of SSE Builder
#
#   Revision 1.6  2009/09/02 18:02:39  ssa1
#   Moving SSEHunter to the SSEViewer menu, and adding checks for loading up volumes and skeletons
#
#   Revision 1.5  2009/09/02 17:38:44  ssa1
#   BugFix: Exception when clicking add helix / add sheet
#
#   Revision 1.4  2009/08/10 20:03:40  ssa1
#   SSEHunter interfaced into Gorgon
#
#   Revision 1.3  2009/06/24 21:33:48  ssa1
#   SSE Builder Functionality: Sheet building and better camera functionality when loading new data.
#
#   Revision 1.2  2009/06/23 16:50:34  ssa1
#   Adding in SSEBuilder Functionality: Saving helix as WRL and SSE files
#
#   Revision 1.1  2009/06/22 20:17:27  ssa1
#   Adding in SSEBuilder Functionality: Selection to Helix functionality
#

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
        
        self.pushButtonRemoveHelices.setEnabled(False)
        
        self.connect(self.pushButtonBrowseAtomScore, QtCore.SIGNAL("clicked (bool)"), self.browseAtomScoreFile)
        self.connect(self.pushButtonSelectionToHelix, QtCore.SIGNAL("clicked (bool)"), self.selectionToHelix)
        self.connect(self.pushButtonSelectionToSheet, QtCore.SIGNAL("clicked (bool)"), self.selectionToSheet)
        self.connect(self.pushButtonRemoveSSE, QtCore.SIGNAL("clicked (bool)"), self.removeSSE)        
        self.connect(self.pushButtonSSEHunter, QtCore.SIGNAL("clicked (bool)"), self.runSSEHunter)
        self.connect(self.pushButtonLoadVolume, QtCore.SIGNAL("clicked (bool)"), self.loadVolume)
        self.connect(self.pushButtonSavePseudoatoms, QtCore.SIGNAL("clicked (bool)"), self.savePseudoatoms)
        self.connect(self.pushButtonLoadSkeleton, QtCore.SIGNAL("clicked (bool)"), self.loadSkeleton)
        self.connect(self.pushButtonAddHelices, QtCore.SIGNAL("clicked (bool)"), self.autoBuildHelices)
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
        pt_line_dist_thresh = self.horizontalSliderLinearityThresh.value()
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
