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

class VolumeSSEBuilderForm(QtGui.QWidget, Ui_DialogVolumeSSEBuilder):
        
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = viewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)   
        
        self.createUI()
        self.createActions()
        self.createMenus()

    def createUI(self):
        self.setupUi(self)    
        self.dock = QtGui.QDockWidget(self.tr(str(self.windowTitle())), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()  
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.pushButtonBrowseAtomScore, QtCore.SIGNAL("clicked (bool)"), self.browseAtomScoreFile)
        self.connect(self.pushButtonSelectionToHelix, QtCore.SIGNAL("clicked (bool)"), self.selectionToHelix)
        self.connect(self.pushButtonSelectionToSheet, QtCore.SIGNAL("clicked (bool)"), self.selectionToSheet)
        self.connect(self.pushButtonSSEHunter, QtCore.SIGNAL("clicked (bool)"), self.runSSEHunter)
                                                                
                                    
    def createActions(self):    
        self.detectSSEAct = QtGui.QAction(self.tr("Identify &SSEs"), self)
        self.detectSSEAct.setStatusTip(self.tr("Identify secondary structure elements"))
        self.detectSSEAct.setCheckable(True)
        self.detectSSEAct.setChecked(False)
        self.connect(self.detectSSEAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("detectSSE_Volume", self.detectSSEAct)    
         
    def createMenus(self):
        self.app.menus.addAction("actions-volume-detectSSE", self.detectSSEAct, "actions-volume");
                
    def loadWidget(self):
        if(self.detectSSEAct.isChecked()) :
            self.showWidget(True)
        else:
            self.showWidget(False)

    def showWidget(self, show):
        if(show):
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.dock.show()
        else:
             self.app.removeDockWidget(self.dock)         
             
    def modelLoaded(self):
        self.detectSSEAct.setEnabled(True)
       
    def modelUnloaded(self):
        self.detectSSEAct.setEnabled(False)
        self.showWidget(False)    
                    
    def dockVisibilityChanged(self, visible):
        self.detectSSEAct.setChecked(visible)
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
            self.lineEditAtomScore.setText(pdbFile)
            
    def runSSEHunter(self, result):
            self.calphaViewer = self.app.viewers["calpha"]
            self.calphaViewer.runSSEHunter(self.doubleSpinBoxThreshold.value(), self.doubleSpinBoxResolution.value(), self.doubleSpinBoxSkeleton.value(), self.doubleSpinBoxCorrelation.value(), self.doubleSpinBoxGeometry.value())
            
    def atomSelectionChanged(self, selection):
        self.tableWidgetSelection.clearContents()
        
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
        atomCnt = self.calphaViewer.renderer.selectionAtomCount()
        atoms = []
        
        for i in range(atomCnt):
            atom = self.calphaViewer.renderer.getSelectedAtom(i)
            atoms.append(atom)
            
        [p1, p2] = self.getHelixEnds(atoms)
        self.sseViewer.renderer.addHelix(p1, p2)
        
        if(self.sseViewer.loaded):
            self.sseViewer.helixLoaded = True
            self.sseViewer.dirty = True           
            self.sseViewer.emitModelChanged()
        else :
            self.sseViewer.loaded = True
            self.sseViewer.sheetLoaded = True
            self.sseViewer.dirty = True
            self.sseViewer.emitModelLoadedPreDraw()
            self.sseViewer.emitModelLoaded()            
        
    def selectionToSheet(self, result):
        atomCnt = self.calphaViewer.renderer.selectionAtomCount()
        
        self.sseViewer.renderer.startNewSheet();
        
        for i in range(atomCnt):
            atom = self.calphaViewer.renderer.getSelectedAtom(i)
            position = atom.getPosition()
            self.sseViewer.renderer.addSheetPoint(position)
            
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
        
        
            
                                 