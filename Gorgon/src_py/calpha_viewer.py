# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   This viewer displays c-alpha atoms 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.33.2.1  2009/12/09 04:54:37  schuhs
#   Add support for rendering a third model
#
#   Revision 1.33  2009/04/03 19:44:37  ssa1
#   CAlpha bug fixes
#
#   Revision 1.32  2009/04/02 19:00:20  ssa1
#   CAlpha Viewer bug fixes and smoother uniform functionality
#
#   Revision 1.31  2009/03/31 21:40:13  ssa1
#   Refactoring: Splitting seq_model\SequenceView.py into subclasses
#
#   Revision 1.30  2009/03/31 20:08:45  ssa1
#   Refactoring: Renaming and moving ChooseChainModel to CAlphaChooseChainModel
#
#   Revision 1.29  2009/01/01 22:48:28  colemanr
#   added docstrings
#
#   Revision 1.28  2008/12/16 23:48:56  colemanr
#   Adding a dock widget to choose between different working models of a structure (different chain objects).
#
#   Revision 1.27  2008/12/02 21:08:50  colemanr
#   Added a file-export menu with an option that will create PDB files that do not have ATOM entries with blank coordinates.
#
#   Revision 1.26  2008/11/28 04:36:17  ssa1
#   Removing error message if pyopengl does not exist.  (To make executable building easier to debug)
#
#   Revision 1.25  2008/11/26 01:10:39  colemanr
#   no longer overwriting processMouseClick
#   instead, using elementClick signal in processElementClick()
#   also, centering on atoms only if the first item of the hitstack is 0 (meaning it's an atom)
#
#   Revision 1.24  2008/11/25 22:35:39  ssa1
#   Removing focus on atoms on right click
#
#   Revision 1.23  2008/11/25 22:00:17  colemanr
#   extended the inherited processMouseClick to change the selected residues in self.main_chain
#
#   Revision 1.22  2008/11/25 21:17:57  ssa1
#   Moving focus functionality into only C-Alpha atoms
#
#   Revision 1.21  2008/11/25 20:42:04  colemanr
#   added self.centerOnSelectedAtoms()
#
#   Revision 1.20  2008/11/19 18:41:14  colemanr
#   filename must be a str not unicode to work with boost::python
#
#   Revision 1.19  2008/11/17 19:41:13  colemanr
#   bug-fix: if self.main_chain is empty, set it to self.structPred.chain before
#   changing visibility of CAlphaSequenceDock
#
#   Revision 1.18  2008/11/14 22:39:53  colemanr
#   Before changing the visibility of CAlphaSequenceView.py, set self.main_chain
#   to self.structPred.chain if self.main_chain is empty -- and other
#   changes involving main_chain and structPred
#
#   Revision 1.17  2008/11/13 20:54:40  ssa1
#   Using the correct scale when loading volumes
#
#   Revision 1.16  2008/11/11 21:44:35  colemanr
#   Loads *.seq files using the reader in StructurePrediction.py rather than
#   the one in Chain.py, because CAlphaSequenceView now uses a StructurePrediction
#   object.
#
#   Revision 1.15  2008/11/10 19:36:11  colemanr
#   Modified to work with the updated CAlphaRenderer.h--no longer need to
#   delete and re-add atoms to the renderer to display changes
#
#   Revision 1.14  2008/11/06 05:29:04  ssa1
#   CGI submission milestone for Interactive Skeletonization, and theme support, and fixing (hopefully) mac-os flicker bug
#
#   Revision 1.13  2008/11/04 16:13:21  colemanr
#   Loading a sequence with SSE predictions from a *.seq file is now
#   seperate from loading a model.  Also, two chain objects are now passed
#   to the CAlphaSequenceDock - one for the sequence & SSE predictions and the
#   other for the current model.
#
#   Revision 1.12  2008/10/30 21:19:06  colemanr
#   actually making use of CAlphaViewer.main_chain
#
#   Revision 1.11  2008/10/21 21:49:55  colemanr
#   Saving PDB files is now connected to the GUI, using Chain.saveToPDB().
#
#   Revision 1.10  2008/10/15 23:55:45  colemanr
#   handles the case of getting no atom for a residue
#
#   Revision 1.9  2008/10/07 23:40:17  colemanr
#   I made the menu item for the sequence viewer dock checkable.  Hiding the
#   dock is not handled properly yet.
#
#   Revision 1.8  2008/10/03 23:47:59  colemanr
#   I overwrote loadData in BaseViewer.  Here it loads Calpha atoms using
#   Chain.load or Chain.loadAllChains.
#
#   Revision 1.7  2008/10/03 00:29:09  colemanr
#   Chain.py now has an addCAlphaBonds instance method.  calpha_viewer.py now adds the sequence viewer to the Actions:C-Alpha menu.  CAlphaSequenceView.py now has a dock widget class that contains a CAlphaSequenceWidget object.
#
#   Revision 1.6  2008/07/28 16:19:22  ssa1
#   Adding in correspondance data repository
#
#   Revision 1.5  2008/07/01 13:41:39  marshm
#   made changes to accomodate seq_model.py:Chain object for calpha-backbone models
#
#   Revision 1.4  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtGui, QtCore, QtOpenGL
from libpyGORGON import CAlphaRenderer
from base_viewer import BaseViewer
from calpha_choose_chain_to_load_form import CAlphaChooseChainToLoadForm
from calpha_atom_placer_form import CAlphaAtomPlacerForm
from calpha_sequence_dock import CAlphaSequenceDock
from seq_model.Chain import Chain
from model_visualization_form import ModelVisualizationForm
from correspondence.StructurePrediction import StructurePrediction
from calpha_choose_chain_model import CAlphaChooseChainModel

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

class CAlphaViewer(BaseViewer):
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "C-Alpha"
        self.shortTitle = "CAL"              
        self.app.themes.addDefaultRGB("C-Alpha:Model:0", 170, 170, 0, 255)
        self.app.themes.addDefaultRGB("C-Alpha:Model:1", 120, 120, 170, 255)
        self.app.themes.addDefaultRGB("C-Alpha:Model:2", 120, 120, 170, 255)
        self.app.themes.addDefaultRGB("C-Alpha:BoundingBox", 255, 255, 255, 255)         
        self.isClosedMesh = False
        self.centerOnRMB = True
        self.selectEnabled = True
        self.renderer = CAlphaRenderer()          
        self.main_chain = Chain('', self.app)
        self.structPred = None
        self.createUI()      
        self.app.viewers["calpha"] = self;
        self.model2Visible = True
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
        self.visualizationOptions.ui.checkBoxModelVisible.setText("Show atoms colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setText("Show backbone colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel2Color.setVisible(True)
        self.loadedChains = [] 
        
        #self.connect(self, QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.centerOnSelectedAtoms)
        self.connect(self, QtCore.SIGNAL("elementClicked (int, int, int, int, int, int, QMouseEvent)"), self.processElementClick)
      

    def centerOnSelectedAtoms(self, *argv):
        # This centers the CAMERA on the last selected atom.                
        if not argv:
            chain = self.main_chain
            resIndices = chain.getSelection()
            posList = []
            for resIndex in resIndices:
                try: 
                    atom = chain[resIndex].getAtom('CA')
                except KeyError:
                    continue
                if not atom:
                    continue
                posList.append(atom.getPosition())
            if not posList:
                return
            pos = posList[0]
            for position in posList[1:]:
                pos += position
            pos =  pos * (1.0/len(posList))
            
        elif argv:
            if argv[0]: #argv[0] is 0 for a click on an atom
                return
            try:
                atom = CAlphaRenderer.getAtomFromHitStack(self.renderer, argv[0], True, *argv[1:-1])
            except:
                return
            pos = atom.getPosition()
        if not pos:
            return
        #print viewer.renderer.getSpacingX(), viewer.renderer.getSpacingY(), viewer.renderer.getSpacingZ()
        x = pos.x()*self.renderer.getSpacingX() + self.renderer.getOriginX()
        y = pos.y()*self.renderer.getSpacingY() + self.renderer.getOriginY()
        z = pos.z()*self.renderer.getSpacingZ() + self.renderer.getOriginZ()
        self.app.mainCamera.setCenter( x, y, z )                
        self.emitModelChanged()
    
    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.manualAtomPlacer = CAlphaAtomPlacerForm(self.app, self, self.main_chain, self.structPred)
        self.chooseChainModel = CAlphaChooseChainModel(self.app)
        
    def createActions(self):
        openAct = QtGui.QAction(self.tr("C-&Alpha Atoms..."), self)
        openAct.setShortcut(self.tr("Ctrl+A"))
        openAct.setStatusTip(self.tr("Load a C-Alpha atom file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_CAlpha", openAct)
        
        openSeqAct = QtGui.QAction(self.tr('Se&quence and SSE prediction'), self)
        openSeqAct.setShortcut(self.tr('Ctrl+U'))
        openSeqAct.setStatusTip(self.tr('Load a sequence possibly with SSE predictions'))
        self.connect(openSeqAct, QtCore.SIGNAL('triggered()'), self.loadSeq)
        self.app.actions.addAction('load_sequence', openSeqAct)
        
        saveAct = QtGui.QAction(self.tr("C-&Alpha Atoms..."), self)
        saveAct.setStatusTip(self.tr("Save a C-Alpha atom file"))
        self.connect(saveAct, QtCore.SIGNAL("triggered()"), self.saveData)
        self.app.actions.addAction("save_CAlpha", saveAct)
        
        exportAct = QtGui.QAction(self.tr('Atoms to &PDB...'), self)
        exportAct.setStatusTip(self.tr('Export a PDB file with no placeholder atoms'))
        self.connect(exportAct, QtCore.SIGNAL('triggered()'), self.exportData)
        self.app.actions.addAction('export_CAlpha', exportAct)
        
        closeAct = QtGui.QAction(self.tr("C-&Alpha Atoms"), self)
        closeAct.setStatusTip(self.tr("Close the loaded C-Alpha atom file"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_CAlpha", closeAct)
        
        seqDockAct = QtGui.QAction(self.tr("Semi-&automatic Atom Placement"), self)
        seqDockAct.setStatusTip(self.tr("Perform partly automated atom placement"))
        seqDockAct.setCheckable(True)
        seqDockAct.setChecked(False)
        def showDock():
            loaded = True
            if not self.structPred:
                loaded = self.loadSeq()
            if self.structPred and not self.main_chain:
                self.main_chain = self.structPred.chain
            if loaded:
                CAlphaSequenceDock.changeDockVisibility(self.app, self, self.structPred, self.main_chain)
        self.connect(seqDockAct, QtCore.SIGNAL("triggered()"), showDock)
        self.app.actions.addAction("seqDock", seqDockAct)
    
    def loadSSEHunterData(self, fileName):
        if(self.loaded):
            self.unloadData()        
        self.fileName = fileName
        self.renderer.loadSSEHunterFile(str(fileName))
        volumeViewer = self.app.viewers["volume"]
        skeletonViewer = self.app.viewers["skeleton"]        
        
        self.dirty = False
        self.loaded = True
        self.emitModelLoadedPreDraw()
        self.emitModelLoaded()
        self.emitViewerSetCenter()        
        
    def runSSEHunter(self, threshold, resolution, skeletonCoefficient, correlationCoefficient, geometryCoefficient):
        if(self.loaded):
            self.unloadData()  
        self.fileName = ""      
        
        volumeViewer = self.app.viewers["volume"]
        skeletonViewer = self.app.viewers["skeleton"]        
        self.renderer.getSSEHunterAtoms(volumeViewer.renderer.getVolume(), skeletonViewer.renderer.getMesh(), resolution, threshold, skeletonCoefficient, correlationCoefficient, geometryCoefficient)

        
        self.dirty = False
        self.loaded = True
        self.emitModelLoadedPreDraw()
        self.emitModelLoaded()
        self.emitViewerSetCenter()        
       
        
        
    def loadData(self):
        #Overwriting the function in BaseViewer        
        def setupChain(mychain):            
            self.main_chain = mychain
            self.loadedChains.append(mychain)
            mychain.setViewer(self)
            #Chain.setSelectedChainKey(mychain.getIDs())
            mychain.addCalphaBonds()
            renderer = self.renderer
            for i in mychain.residueRange():
                atom = mychain[i].getAtom('CA')
                if atom:
                    atom = renderer.addAtom(atom)
                    mychain[i].addAtomObject(atom)
               
        self.fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", 
                            self.tr('Atom Positions (*.pdb)\nFASTA (*.fas *.fa *.fasta)'))
        fileNameTemp = self.fileName
        self.whichChainID = None
        filename = unicode(self.fileName)
        if filename.split('.')[-1].lower() == 'pdb':
            dlg = CAlphaChooseChainToLoadForm(unicode(self.fileName))
            if dlg.exec_():
                self.whichChainID = dlg.whichChainID
                if not self.fileName.isEmpty():
                    if(self.loaded):
                        self.unloadData()
                        
                    self.fileName = fileNameTemp
                    
                    if self.whichChainID == 'ALL':
                        mychainKeys = Chain.loadAllChains(str(self.fileName), qparent=self.app)
                        for chainKey in mychainKeys:
                            setupChain(Chain.getChain(chainKey))
                    else:
                        mychain = Chain.load(str(self.fileName), qparent=self.app, whichChainID = self.whichChainID)
                        setupChain(mychain)
        
                    if not self.loaded:
                        self.dirty = False
                        self.loaded = True
                        self.emitModelLoadedPreDraw()
                        self.emitModelLoaded()
                        self.emitViewerSetCenter()
    
    def unloadData(self):
        #overwriting the function in base viewer
        for chain in self.loadedChains:
            del chain
            chain = None
            
        BaseViewer.unloadData(self)
    
    def loadSeq(self):
        """
This function loads a SEQ file and creates a StructurePrediction object.
        """
        fileName = QtGui.QFileDialog.getOpenFileName( self, self.tr('Open Sequence'), '', 
                                            self.tr('Sequence possibly with SSE predictions (*.seq)') )
        fileName = str(fileName)
        if fileName:
            self.structPred = StructurePrediction.load(fileName, self.app)
            return True
        else : 
            return False
    
    def createMenus(self):
        self.app.menus.addAction("file-open-calpha", self.app.actions.getAction("load_CAlpha"), "file-open")
        self.app.menus.addAction('file-open-sequence', self.app.actions.getAction('load_sequence'), 'file-open')
        self.app.menus.addAction("file-save-calpha", self.app.actions.getAction("save_CAlpha"), "file-save")
        self.app.menus.addAction("file-export-calpha", self.app.actions.getAction("export_CAlpha"), "file-export")
        self.app.menus.addAction("file-close-calpha", self.app.actions.getAction("unload_CAlpha"), "file-close")
        self.app.menus.addMenu("actions-calpha", self.tr("C-&Alpha Atoms"), "actions")
        self.app.menus.addAction("showSeqDock", self.app.actions.getAction("seqDock"), "actions-calpha")           

    def clearSelection(self):
        BaseViewer.clearSelection(self)
        self.main_chain.setSelection([], None, None, None)
        self.emitAtomSelectionUpdated(self.main_chain.getSelection())      

    def processElementClick(self, *argv):
        """
In response to a click on a C-alpha element, this updates the selected
residues in the Chain object.
        """
        if argv[0]: #argv[0] is 0 for a click on an atom
            return
        hits = argv[:-1]
        event = argv[-1]
        if event.button() == QtCore.Qt.LeftButton:
            if event.modifiers() & QtCore.Qt.CTRL: #Multiple selection mode
                atom = CAlphaRenderer.getAtomFromHitStack(self.renderer, hits[0], False, *hits[1:])
                if atom.getResSeq() in self.main_chain.getSelection():
                    self.main_chain.setSelection(removeOne=atom.getResSeq())
                else:
                    self.main_chain.setSelection(addOne=atom.getResSeq())
                print self.main_chain.getSelection()
            else:
                atom = CAlphaRenderer.getAtomFromHitStack(self.renderer, hits[0], True, *hits[1:])
                self.main_chain.setSelection([atom.getResSeq()])
            self.emitAtomSelectionUpdated(self.main_chain.getSelection())
                
        if event.button() == QtCore.Qt.RightButton and self.centerOnRMB:
            self.centerOnSelectedAtoms(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6])
            
    
    def exportData(self):
        """
This saves the current chain model to a PDB file with no "ATOM" lines
for atoms that have not been placed.
        """
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Data"), "", 
                                                          self.tr('Atom Positions (*.pdb)'))
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            selectedChain = self.main_chain
            PDBstring = selectedChain.toPDB( CAlphaPlaceholders=False)
            F = open(self.fileName, 'w')
            F.write(PDBstring)
            F.close()
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
    
    def saveData(self):
        """
This saves the current chain model to a PDB file with 'ATOM' lines that
have no coordinates for atoms that have not been placed. These 
non-standard ATOM lines serve as placeholders so the entire sequence of
the chain is known including residue numbers ('SEQRES' does not give a 
starting residue number).
        """
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Data"), "", 
                                                          self.tr('Atom Positions (*.pdb)'))
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            selectedChain = self.main_chain
            selectedChain.saveToPDB(self.fileName)
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
    
    def updateActionsAndMenus(self):
        """
If a C-alpha model is loaded, this enables relevent actions.
        """
        self.app.actions.getAction("save_CAlpha").setEnabled(self.loaded)
        self.app.actions.getAction("unload_CAlpha").setEnabled(self.loaded)
        
    def emitAtomSelectionUpdated(self, selection):
        self.emit(QtCore.SIGNAL("atomSelectionUpdated(PyQt_PyObject)"), selection)
