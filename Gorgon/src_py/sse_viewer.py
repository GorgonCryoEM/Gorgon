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
#   Revision 1.24  2009/04/02 19:00:20  ssa1
#   CAlpha Viewer bug fixes and smoother uniform functionality
#
#   Revision 1.23  2009/03/17 20:00:17  ssa1
#   Removing Sheets from fiting process
#
#   Revision 1.22  2009/03/16 17:15:24  ssa1
#   setting busy state when fitting
#
#   Revision 1.21  2009/03/16 16:17:34  ssa1
#   Fitting SSEs into the Density
#
#   Revision 1.20  2009/01/01 22:09:39  colemanr
#   added docstring
#
#   Revision 1.19  2008/12/19 23:00:54  colemanr
#   In response to the signal elementSelected, SSE viewer saves the current match between an observed and predicted helix
#   if the selected element is a helix.  If it is a helix, the SSE viewer emits "SSE selected"
#
#   Revision 1.18  2008/12/18 20:15:23  ssa1
#   Adding sequence predictor
#
#   Revision 1.17  2008/12/02 21:54:53  ssa1
#   visualizing a dummy file name for sse elements
#
#   Revision 1.16  2008/11/28 04:36:17  ssa1
#   Removing error message if pyopengl does not exist.  (To make executable building easier to debug)
#
#   Revision 1.15  2008/11/20 20:56:30  ssa1
#   Properly scaling the suggested backbone
#
#   Revision 1.14  2008/11/13 20:54:40  ssa1
#   Using the correct scale when loading volumes
#
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
from sse_sequence_predictor_form import SSESequencePredictorForm
from sse_helix_correspondence_finder_form import SSEHelixCorrespondenceFinderForm
from model_visualization_form import ModelVisualizationForm
from libpyGORGON import SSECorrespondenceEngine, SSECorrespondenceResult

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


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
        self.currentMatch = None
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
        
        self.connect(self, QtCore.SIGNAL('elementSelected (int, int, int, int, int, int, QMouseEvent)'), self.updateCurrentMatch)
        
        self.connect(self.app.viewers["volume"], QtCore.SIGNAL('modelLoaded()'), self.updateActionsAndMenus)
        self.connect(self.app.viewers["volume"], QtCore.SIGNAL('modelUnloaded()'), self.updateActionsAndMenus)
        self.connect(self, QtCore.SIGNAL('modelLoaded()'), self.updateActionsAndMenus)
        self.connect(self, QtCore.SIGNAL('modelUnloaded()'), self.updateActionsAndMenus)
    

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.sequencePredictor = SSESequencePredictorForm(self.app, self)
        self.helixCorrespondanceFinder = SSEHelixCorrespondenceFinderForm(self.app, self)
    
    def loadHelixData(self):
        helixFileName = str(QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats())))
        self.loadHelixDataFile(helixFileName)
               
    def loadHelixDataFile(self, fileName):
        self.helixFileName = fileName
        self.fileName = self.helixFileName
        if not self.helixFileName == "":  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadHelixFile(str(self.helixFileName))
            self.loaded = True
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoaded()
            self.emitViewerSetCenter()        
               
    def loadSheetData(self):
        sheetFileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        self.loadSheetDataFile(sheetFileName)
                                          
    def loadSheetDataFile(self, fileName):
        self.sheetFileName = fileName
        self.fileName = self.sheetFileName
        if not self.sheetFileName == "":  
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

        fitAct = QtGui.QAction(self.tr("Fit Selected Helices"), self)
        fitAct.setShortcut(self.tr("Ctrl+F"))        
        fitAct.setStatusTip(self.tr("Fit the selected Helices into the density"))        
        self.connect(fitAct, QtCore.SIGNAL("triggered()"), self.fitSelectedSSEs)
        self.app.actions.addAction("fit_SSE_Helix", fitAct)        
                        
    def createMenus(self):
        self.app.menus.addAction("file-open-helix", self.app.actions.getAction("load_SSE_Helix"), "file-open")    
        self.app.menus.addAction("file-open-sheet", self.app.actions.getAction("load_SSE_Sheet"), "file-open")        
        self.app.menus.addAction("file-close-sse", self.app.actions.getAction("unload_SSE"), "file-close");
        self.app.menus.addMenu("actions-sse", self.tr("Secondary Structure &Element"), "actions");
        self.app.menus.addAction("actions-sse-fit-helix", self.app.actions.getAction("fit_SSE_Helix"), "actions-sse");
                   
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_SSE").setEnabled(self.loaded)
        self.app.actions.getAction("fit_SSE_Helix").setEnabled(self.loaded and self.app.viewers["volume"].loaded)
    
    def updateCurrentMatch(self, sseType, sseIndex):
        # When an element is selected in this viewer, if that item is a helix,
        # this sets self.currentMatch to the observed, predicted match for that
        # helix. It then emits an 'SSE selected' signal. 
        self.currentMatch = None
        if sseType == 0:
            try:
                self.correspondenceLibrary
            except AttributeError:
                return
            corrLib = self.correspondenceLibrary
            currCorrIndex = corrLib.getCurrentCorrespondenceIndex()
            matchList = corrLib.correspondenceList[currCorrIndex].matchList
            for match in matchList:
                if match.observed.label == sseIndex: 
                    self.currentMatch = match
                    self.emit(QtCore.SIGNAL("SSE selected"))
                    break

    def fitSelectedSSEs(self):
        self.app.mainCamera.setCursor(QtCore.Qt.BusyCursor)        
        self.renderer.fitSelectedSSEs(self.app.viewers["volume"].renderer.getVolume())
        self.emitModelChanged()
        self.app.mainCamera.setCursor(QtCore.Qt.ArrowCursor)