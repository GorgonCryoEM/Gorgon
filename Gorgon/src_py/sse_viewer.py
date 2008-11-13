# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A viewer for secondary structure elements 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.13  2008/11/06 05:29:04  ssa1
#   CGI submission milestone for Interactive Skeletonization, and theme support, and fixing (hopefully) mac-os flicker bug
#
#   Revision 1.12  2008/08/06 06:21:37  ssa1
#   Tracing protein path, for SSE Correspondance matching
#
#   Revision 1.11  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtGui, QtCore, QtOpenGL
from libpyGORGON import SSERenderer
from base_viewer import BaseViewer
from sse_helix_correspondence_finder_form import SSEHelixCorrespondenceFinderForm
from model_visualization_form import ModelVisualizationForm
from libpyGORGON import SSECorrespondenceEngine, SSECorrespondenceResult

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "Gorgon", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class SSEViewer(BaseViewer):
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Secondary Structure Element"
        self.app.themes.addDefaultRGB("Secondary Structure Element:Model:0", 0, 180, 0, 255)
        self.app.themes.addDefaultRGB("Secondary Structure Element:Model:1", 120, 185, 255, 255)
        self.app.themes.addDefaultRGB("Secondary Structure Element:BoundingBox", 255, 255, 255, 255)          
        self.isClosedMesh = False
        self.helixFileName = ""
        self.sheetFileName = ""
        self.showBox = False;
        self.renderer = SSERenderer()
        self.correspondenceEngine = SSECorrespondenceEngine()
        self.createUI()      
        self.selectEnabled = True
        self.app.viewers["sse"] = self;
        self.model2Visible = True
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
        self.visualizationOptions.ui.checkBoxModelVisible.setText("Show helices colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setText("Show sheets colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel2Color.setVisible(True)
    

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.helixCorrespondanceFinder = SSEHelixCorrespondenceFinderForm(self.app, self)
    
    def loadHelixData(self):
        self.helixFileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not self.helixFileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadHelixFile(str(self.helixFileName))
            self.loaded = True
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoaded()
            self.emitViewerSetCenter()        
               
    def loadSheetData(self):
        self.sheetFileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not self.sheetFileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadSheetFile(str(self.sheetFileName))
            self.loaded = True
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoaded()
            self.emitViewerSetCenter()
                                          
    def unloadData(self):
        self.helixFileName = ""
        self.sheetFileName = ""
        BaseViewer.unloadData(self)
               
    def createActions(self):
        openHelixAct = QtGui.QAction(self.tr("&Helix Annotations"), self)
        openHelixAct.setShortcut(self.tr("Ctrl+H"))
        openHelixAct.setStatusTip(self.tr("Load a helix file"))
        self.connect(openHelixAct, QtCore.SIGNAL("triggered()"), self.loadHelixData)
        self.app.actions.addAction("load_SSE_Helix", openHelixAct)

        openSheetAct = QtGui.QAction(self.tr("&Sheet Annotations"), self)
        openSheetAct.setShortcut(self.tr("Ctrl+S"))
        openSheetAct.setStatusTip(self.tr("Load a sheet file"))
        self.connect(openSheetAct, QtCore.SIGNAL("triggered()"), self.loadSheetData)
        self.app.actions.addAction("load_SSE_Sheet", openSheetAct)
               
        closeAct = QtGui.QAction(self.tr("SSE Annotations"), self)
        closeAct.setStatusTip(self.tr("Close the loaded secondary structure element file"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_SSE", closeAct)
        
                        
    def createMenus(self):
        self.app.menus.addAction("file-open-helix", self.app.actions.getAction("load_SSE_Helix"), "file-open")    
        self.app.menus.addAction("file-open-sheet", self.app.actions.getAction("load_SSE_Sheet"), "file-open")        
        self.app.menus.addAction("file-close-sse", self.app.actions.getAction("unload_SSE"), "file-close");
        self.app.menus.addMenu("actions-sse", self.tr("Secondary Structure &Element"), "actions");
                   
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_SSE").setEnabled(self.loaded)

    def extraDrawingRoutines(self):
        self.helixCorrespondanceFinder.drawOverlay()          
    
             
