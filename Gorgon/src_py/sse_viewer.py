# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A viewer for secondary structure elements 

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/sse_viewer.py,v $
#   $Revision: 1.35 $
#   $Date: 2010/07/19 17:29:02 $
#   $Author: heiderp $
#   $State: Exp $
#
# History Log: 
#   $Log: sse_viewer.py,v $
#   Revision 1.35  2010/07/19 17:29:02  heiderp
#   LARGE update.  Added flexible fitting functionality, lots of logic in FlexibleFittingEngine.h
#
#   Revision 1.34  2010/01/17 05:13:36  schuhs
#   Fixing bug that created an offset between the skeleton and the SSEHunter sheets in the SSE correspondence code
#
#   Revision 1.33  2010/01/14 23:52:12  ssa1
#   Gracefully catching exceptions when loading invalid files
#
#   Revision 1.32  2009/12/24 21:53:49  ssa1
#   Giving back color control to the SSE Visualization options form when SSE Correspondence engine is not running (Bug ID: 58)
#
#   Revision 1.31  2009/12/24 07:25:07  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior
#
#   Revision 1.30  2009/12/22 01:02:24  schuhs
#   Adding support for beta sheet matching to the SSE correspondence search algorithm
#
#   Revision 1.29  2009/10/05 17:57:37  ssa1
#   Initial session saving functionality (Request ID:52)
#
#   Revision 1.28  2009/09/02 18:02:39  ssa1
#   Moving SSEHunter to the SSEViewer menu, and adding checks for loading up volumes and skeletons
#
#   Revision 1.27  2009/06/24 21:33:48  ssa1
#   SSE Builder Functionality: Sheet building and better camera functionality when loading new data.
#
#   Revision 1.26  2009/06/24 13:06:51  ssa1
#   Fixing compilation issues on MacOS
#
#   Revision 1.25  2009/06/23 16:50:34  ssa1
#   Adding in SSEBuilder Functionality: Saving helix as WRL and SSE files
#
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
from volume_sse_builder_form import VolumeSSEBuilderForm

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class SSEViewer(BaseViewer):
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Secondary Structure Element"
        self.shortTitle = "SSE"              
        self.app.themes.addDefaultRGB("Secondary Structure Element:Model:0", 0, 180, 0, 255)
        self.app.themes.addDefaultRGB("Secondary Structure Element:Model:1", 120, 185, 255, 255)
        self.app.themes.addDefaultRGB("Secondary Structure Element:Model:2", 120, 185, 255, 255)
        self.app.themes.addDefaultRGB("Secondary Structure Element:BoundingBox", 255, 255, 255, 255)          
        self.isClosedMesh = False
        self.helixFileName = ""
        self.sheetFileName = ""
        self.currentMatch = None
        self.showBox = False;
        self.helixLoaded = False
        self.sheetLoaded = False        
        self.renderer = SSERenderer()
        self.correspondenceEngine = SSECorrespondenceEngine()
        self.createUI()
        self.selectEnabled = True
        self.app.viewers["sse"] = self
        self.model2Visible = True
        self.model3Visible = False
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
        self.visualizationOptions.ui.checkBoxModelVisible.setText("Show helices colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setText("Show sheets colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel2Color.setVisible(True)
        self.visualizationOptions.ui.checkBoxModel3Visible.setText("Show skeleton sheets colored:")
        self.visualizationOptions.ui.checkBoxModel3Visible.setVisible(False)
        self.visualizationOptions.ui.pushButtonModel3Color.setVisible(False)
        
        self.connect(self, QtCore.SIGNAL('elementSelected (int, int, int, int, int, int, QMouseEvent)'), self.updateCurrentMatch)
        
        self.connect(self.app.viewers["volume"], QtCore.SIGNAL('modelLoaded()'), self.updateActionsAndMenus)
        self.connect(self.app.viewers["volume"], QtCore.SIGNAL('modelUnloaded()'), self.updateActionsAndMenus)
        self.connect(self, QtCore.SIGNAL('modelLoaded()'), self.updateActionsAndMenus)
        self.connect(self, QtCore.SIGNAL('modelUnloaded()'), self.updateActionsAndMenus)
        
        self.selectedObjects = []
        self.correspondences = []
    

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.sseBuilder = VolumeSSEBuilderForm(self.app, self, self)
        self.sequencePredictor = SSESequencePredictorForm(self.app, self, self)
        self.helixCorrespondanceFinder = SSEHelixCorrespondenceFinderForm(self.app, self, self)
        
    def loadHelixDataFromFile(self, fileName):
        self.setCursor(QtCore.Qt.WaitCursor)
        try:
            self.renderer.loadHelixFile(str(fileName))
            self.loaded = True
            self.helixLoaded = True
            self.emitModelLoaded()
            self.emitViewerSetCenter()    
        except:
            QtGui.QMessageBox.critical(self, "Unable to load data file", "The file might be corrupt, or the format may not be supported.", "Ok")

            self.loaded = False
        self.setCursor(QtCore.Qt.WaitCursor)
            
    
    def loadHelixData(self):
        self.helixFileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Helix Annotations"), "", self.tr(self.renderer.getSupportedHelixLoadFileFormats()))
        self.fileName = self.helixFileName;
        if not self.helixFileName.isEmpty():  
            self.loadHelixDataFromFile(self.helixFileName)
                    
    
    def loadSheetDataFromFile(self, fileName):
        self.setCursor(QtCore.Qt.WaitCursor)
        try:
            self.renderer.loadSheetFile(str(fileName))
            self.loaded = True
            self.sheetLoaded = True
            self.emitModelLoaded()
            self.emitViewerSetCenter()        
        except:
            QtGui.QMessageBox.critical(self, "Unable to load data file", "The file might be corrupt, or the format may not be supported.", "Ok")
            self.loaded = False
        self.setCursor(QtCore.Qt.ArrowCursor)
        
    
    def loadSheetData(self):
        self.sheetFileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Sheet Annotations"), "", self.tr(self.renderer.getSupportedSheetLoadFileFormats()))
        self.fileName = self.sheetFileName;
        if not self.sheetFileName.isEmpty():  
            self.loadSheetDataFromFile(self.sheetFileName)
            
    def saveHelixData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Helix Annotations"), "", self.tr(self.renderer.getSupportedHelixSaveFileFormats()))
        if not self.fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveHelixFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)

    def saveSheetData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Sheet Annotations"), "", self.tr(self.renderer.getSupportedSheetSaveFileFormats()))
        if not self.fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveSheetFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)

    def getSessionInfo(self, sessionManager):
        info = BaseViewer.getSessionInfo(self, sessionManager)  
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "HELIX_LOADED", self.helixLoaded))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "HELIX_FILE", self.helixFileName))        
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "SHEET_LOADED", self.sheetLoaded))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "SHEET_FILE", self.sheetFileName))
        return info
                       
    def loadSessionInfo(self, sessionManager, sessionProperties):
        BaseViewer.loadSessionInfo(self, sessionManager, sessionProperties)        
        self.helixLoaded = sessionManager.getProperty(sessionProperties, self.shortTitle, "HELIX_LOADED")
        if self.helixLoaded:
            self.helixFileName = sessionManager.getProperty(sessionProperties, self.shortTitle, "HELIX_FILE")
            self.loadHelixDataFromFile(self.helixFileName)
        self.sheetLoaded = sessionManager.getProperty(sessionProperties, self.shortTitle, "SHEET_LOADED")
        if self.sheetLoaded:
            self.sheetFileName = sessionManager.getProperty(sessionProperties, self.shortTitle, "SHEET_FILE")
            self.loadSheetDataFromFile(self.sheetFileName)

                                                    
    def unloadData(self):
        self.loaded = False
        self.helixLoaded = False
        self.sheetLoaded = False
        self.helixFileName = ""
        self.sheetFileName = ""
        BaseViewer.unloadData(self)
          
          
    def makeSheetSurfaces(self, offsetx, offsety, offsetz, scalex, scaley, scalez):
        # rebuild the set of sheets to render
        numHelicesSheets = self.correspondenceEngine.getSkeletonSSECount()
        self.renderer.unloadGraphSSEs()
        for i in range(numHelicesSheets):
            if self.correspondenceEngine.getSkeletonSSE(i).isSheet():
                self.renderer.loadGraphSSE(i, self.correspondenceEngine.getSkeletonSSE(i), offsetx, offsety, offsetz, scalex, scaley, scalez)

               
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

        saveHelixAct = QtGui.QAction(self.tr("&Helix Annotations..."), self)
        saveHelixAct.setStatusTip(self.tr("Save helix annotations"))
        self.connect(saveHelixAct, QtCore.SIGNAL("triggered()"), self.saveHelixData)
        self.app.actions.addAction("save_SSE_Helix", saveHelixAct)        

        saveSheetAct = QtGui.QAction(self.tr("&Sheet Annotations..."), self)
        saveSheetAct.setStatusTip(self.tr("Save sheet annotations"))
        self.connect(saveSheetAct, QtCore.SIGNAL("triggered()"), self.saveSheetData)
        self.app.actions.addAction("save_SSE_Sheet", saveSheetAct)    
                       
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
        self.app.menus.addAction("file-save-helix", self.app.actions.getAction("save_SSE_Helix"), "file-save");        
        self.app.menus.addAction("file-save-sheet", self.app.actions.getAction("save_SSE_Sheet"), "file-save");        
        self.app.menus.addAction("file-close-sse", self.app.actions.getAction("unload_SSE"), "file-close");
        self.app.menus.addMenu("actions-sse", self.tr("Secondary Structure &Element"), "actions");
        self.app.menus.addAction("actions-sse-fit-helix", self.app.actions.getAction("fit_SSE_Helix"), "actions-sse");
                   
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_SSE").setEnabled(self.loaded)
        self.app.actions.getAction("save_SSE_Helix").setEnabled(self.helixLoaded)
        self.app.actions.getAction("save_SSE_Sheet").setEnabled(self.sheetLoaded)
        self.app.actions.getAction("fit_SSE_Helix").setEnabled(self.loaded and self.app.viewers["volume"].loaded)
    
    def updateCurrentMatch(self, sseType, sseIndex):
        # When an element is selected in this viewer, if that item is a helix,
        # this sets self.currentMatch to the observed, predicted match for that
        # helix. It then emits an 'SSE selected' signal. 
        print "Helix #: ", sseIndex
        
        self.currentMatch = None
        
        if self.multipleSelection == True:
            self.selectedObjects.append(sseIndex)
        else:
            self.selectedObjects = []
            self.selectedObjects.append(sseIndex)
            
        self.emit(QtCore.SIGNAL("SSE selected"))
        if sseType == 0:
            try:
                self.correspondenceLibrary
            except AttributeError:
                return
            corrLib = self.correspondenceLibrary
            currCorrIndex = corrLib.getCurrentCorrespondenceIndex()
            matchList = corrLib.correspondenceList[currCorrIndex].matchList
            for match in matchList:
                if match.observed is not None and match.observed.label == sseIndex: 
                    self.currentMatch = match
                    print self.currentMatch
                    self.emit(QtCore.SIGNAL("SSE selected"))
                    break

    def fitSelectedSSEs(self):
        self.app.mainCamera.setCursor(QtCore.Qt.BusyCursor)        
        self.renderer.fitSelectedSSEs(self.app.viewers["volume"].renderer.getVolume())
        self.emitModelChanged()
        self.app.mainCamera.setCursor(QtCore.Qt.ArrowCursor)
        
        
    def updateCorrespondences(self, corrs):
        self.correspondences  = corrs
        
        
        
    # Overridden
    def emitElementClicked(self, hitStack, event):        
        if (self.app.viewers["calpha"].displayStyle == self.app.viewers["calpha"].DisplayStyleRibbon):
            if(self.app.mainCamera.mouseRightPressed and hitStack[0] == 0):
                self.emit(QtCore.SIGNAL("SSERightClicked(PyQt_PyObject, PyQt_PyObject, QMouseEvent)"), hitStack[0], hitStack[1], event)
        else:
            BaseViewer.emitElementClicked(self, hitStack, event)