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
#   Revision 1.4  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtGui, QtCore, QtOpenGL
from libpyGORGON import CAlphaRenderer
from base_viewer import BaseViewer
from calpha_atom_placer_form import CAlphaAtomPlacerForm
from seq_model import Chain

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
        self.isClosedMesh = False
        self.selectEnabled = True
        self.renderer = CAlphaRenderer()          
        self.main_chain = Chain('')
        self.createUI()      
        self.app.viewers["calpha"] = self;
        self.modelColor = QtGui.QColor.fromRgba(QtGui.qRgba(170, 170, 0, 255))
        self.model2Color = QtGui.QColor.fromRgba(QtGui.qRgba(120, 120, 170, 255))
        self.model2Visible = True
        self.initVisualizationOptions()
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
        self.manualAtomPlacer = CAlphaAtomPlacerForm(self.app, self, self.main_chain)
        
    def createActions(self):
        openAct = QtGui.QAction(self.tr("C-&Alpha Atoms..."), self)
        openAct.setShortcut(self.tr("Ctrl+A"))
        openAct.setStatusTip(self.tr("Load a C-Alpha atom file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_CAlpha", openAct)
        
        saveAct = QtGui.QAction(self.tr("C-&Alpha Atoms..."), self)
        saveAct.setStatusTip(self.tr("Save a C-Alpha atom file"))
        self.connect(saveAct, QtCore.SIGNAL("triggered()"), self.saveData)
        self.app.actions.addAction("save_CAlpha", saveAct)                
        
        closeAct = QtGui.QAction(self.tr("C-&Alpha Atoms"), self)
        closeAct.setStatusTip(self.tr("Close the loaded C-Alpha atom file"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_CAlpha", closeAct)
                                
    def createMenus(self):
        self.app.menus.addAction("file-open-calpha", self.app.actions.getAction("load_CAlpha"), "file-open")    
        self.app.menus.addAction("file-save-calpha", self.app.actions.getAction("save_CAlpha"), "file-save")
        self.app.menus.addAction("file-close-calpha", self.app.actions.getAction("unload_CAlpha"), "file-close");
        self.app.menus.addMenu("actions-calpha", self.tr("C-&Alpha Atoms"), "actions");
                   
    def updateActionsAndMenus(self):        
        self.app.actions.getAction("save_CAlpha").setEnabled(self.loaded)
        self.app.actions.getAction("unload_CAlpha").setEnabled(self.loaded)  
             
