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
#   Revision 1.17  2008/11/13 20:54:40  ssa1
#   Using the correct scale when loading volumes
#
#   Revision 1.16  2008/11/11 21:44:35  colemanr
#   Loads *.seq files using the reader in StructurePrediction.py rather than
#   the one in Chain.py, because SequenceView now uses a StructurePrediction
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
#   to the SequenceDock - one for the sequence & SSE predictions and the
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
#   Chain.py now has an addCAlphaBonds instance method.  calpha_viewer.py now adds the sequence viewer to the Actions:C-Alpha menu.  SequenceView.py now has a dock widget class that contains a SequenceWidget object.
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
from calpha_atom_placer_form import CAlphaAtomPlacerForm
from seq_model.SequenceView import SequenceDock
from seq_model.Chain import Chain
from model_visualization_form import ModelVisualizationForm
from correspondence.StructurePrediction import StructurePrediction

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "Gorgon", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class CAlphaViewer(BaseViewer):
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "C-Alpha"
        self.app.themes.addDefaultRGB("C-Alpha:Model:0", 170, 170, 0, 255)
        self.app.themes.addDefaultRGB("C-Alpha:Model:1", 120, 120, 170, 255)
        self.app.themes.addDefaultRGB("C-Alpha:BoundingBox", 255, 255, 255, 255)         
        self.isClosedMesh = False
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
                 
    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.manualAtomPlacer = CAlphaAtomPlacerForm(self.app, self, self.main_chain, self.structPred)
        
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
        
        closeAct = QtGui.QAction(self.tr("C-&Alpha Atoms"), self)
        closeAct.setStatusTip(self.tr("Close the loaded C-Alpha atom file"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_CAlpha", closeAct)
        
        seqDockAct = QtGui.QAction(self.tr("Partly &Automated Atom Placement"), self)
        seqDockAct.setStatusTip(self.tr("Perform partly automated atom placement"))
        seqDockAct.setCheckable(True)
        seqDockAct.setChecked(False)
        def showDock():
            if not self.structPred:
                self.loadSeq()
            if self.structPred and not self.main_chain:
                self.main_chain = self.structPred
            SequenceDock.changeDockVisibility(self.app, self, self.structPred, self.main_chain)
        self.connect(seqDockAct, QtCore.SIGNAL("triggered()"), showDock)
        self.app.actions.addAction("seqDock", seqDockAct)
        
    def loadData(self):
        #Overwriting the function in BaseViewer
        self.loaded = False #We want to load a chain to the screen each time
        self.fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", 
                            self.tr('Atom Positions (*.pdb)\nFASTA (*.fas *.fa *.fasta)'))
        self.whichChainID = None
        filename = unicode(self.fileName)
        if filename.split('.')[-1].lower() == 'pdb':
            dlg = WhichChainToLoad(unicode(self.fileName))
            if dlg.exec_():
                self.whichChainID = dlg.whichChainID
        
        def setupChain(mychain):
            self.main_chain = mychain
            mychain.setViewer(self)
            #Chain.setSelectedChainKey(mychain.getIDs())
            mychain.addCalphaBonds()
            renderer = self.renderer
            for i in mychain.residueRange():
                atom = mychain[i].getAtom('CA')
                if atom == None:
                    continue
                atom = renderer.addAtom(atom)
                mychain[i].addAtomObject(atom)
        
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            print self.whichChainID
            if self.whichChainID == 'ALL':
                mychainKeys = Chain.loadAllChains(str(self.fileName), qparent=self.app)
                print mychainKeys
                for chainKey in mychainKeys:
                    setupChain(Chain.getChain(chainKey))
            else:
                mychain = Chain.load(str(self.fileName), qparent=self.app, whichChainID = self.whichChainID)
                print Chain.getChainKeys()
                setupChain(mychain)

            if not self.loaded:
                self.dirty = False
                self.loaded = True
                self.emitModelLoadedPreDraw()
                self.emitModelLoaded()
                self.emitViewerSetCenter()
    
    def loadSeq(self):
        fileName = QtGui.QFileDialog.getOpenFileName( self, self.tr('Open Sequence'), '', 
                                            self.tr('Sequence possibly with SSE predictions (*.seq)') )
        fileName = unicode(fileName)
        if fileName:
            self.structPred = StructurePrediction.load(fileName, self.app)
    
    def createMenus(self):
        self.app.menus.addAction("file-open-calpha", self.app.actions.getAction("load_CAlpha"), "file-open")
        self.app.menus.addAction('file-open-sequence', self.app.actions.getAction('load_sequence'), 'file-open')
        self.app.menus.addAction("file-save-calpha", self.app.actions.getAction("save_CAlpha"), "file-save")
        self.app.menus.addAction("file-close-calpha", self.app.actions.getAction("unload_CAlpha"), "file-close")
        self.app.menus.addMenu("actions-calpha", self.tr("C-&Alpha Atoms"), "actions")
        self.app.menus.addAction("showSeqDock", self.app.actions.getAction("seqDock"), "actions-calpha")           
    
    def saveData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Data"), "", 
                                                          self.tr('Atom Positions (*.pdb)'))
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            selectedChain = self.main_chain
            selectedChain.saveToPDB(self.fileName)
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
    
    def updateActionsAndMenus(self):        
        self.app.actions.getAction("save_CAlpha").setEnabled(self.loaded)
        self.app.actions.getAction("unload_CAlpha").setEnabled(self.loaded)

class WhichChainToLoad(QtGui.QDialog):
    def __init__(self, fileName, parent=None):
        super(WhichChainToLoad, self).__init__(parent)
        message = QtGui.QLabel('Which chain do you want to load?')
        self.setWindowTitle('Which Chain?')
        chainIDs = Chain.getChainIDsFromPDB(fileName)
        self.chainIDList = QtGui.QListWidget()
        self.chainIDList.addItems(chainIDs)
        self.chainIDList.addItem('ALL')
        buttonBox = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Ok|QtGui.QDialogButtonBox.Cancel)
        layout = QtGui.QVBoxLayout()
        layout.addWidget(message)
        layout.addWidget(self.chainIDList)
        layout.addWidget(buttonBox)
        self.setLayout(layout)
        
        self.connect(buttonBox, QtCore.SIGNAL('accepted()'), self, QtCore.SLOT('accept()'))
        self.connect(buttonBox, QtCore.SIGNAL('rejected()'), self, QtCore.SLOT('reject()'))
    def accept(self):
        self.whichChainID = str( self.chainIDList.currentItem().text() )
        QtGui.QDialog.accept(self)
             
