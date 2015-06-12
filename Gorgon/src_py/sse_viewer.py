# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A viewer for secondary structure elements 


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
