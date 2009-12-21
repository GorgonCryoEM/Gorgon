# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A viewer for volume data 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.20.2.2  2009/12/09 04:04:05  schuhs
#   Add support for rendering a third model
#
#   Revision 1.20.2.1  2009/06/09 16:35:00  schuhs
#   Allow volume filename to be passed as an argument or selected from the file dialog
#
#   Revision 1.20  2008/12/15 22:38:31  ssa1
#   Adding in support to load RAW volumes
#
#   Revision 1.19  2008/11/28 04:36:17  ssa1
#   Removing error message if pyopengl does not exist.  (To make executable building easier to debug)
#
#   Revision 1.18  2008/11/18 22:01:18  ssa1
#   Removing printfs, and adding cropping
#
#   Revision 1.17  2008/11/13 20:54:40  ssa1
#   Using the correct scale when loading volumes
#
#   Revision 1.16  2008/11/06 05:29:04  ssa1
#   CGI submission milestone for Interactive Skeletonization, and theme support, and fixing (hopefully) mac-os flicker bug
#
#   Revision 1.15  2008/10/29 19:26:26  ssa1
#   Reducing memory footprint, Increasing performance and adding volume normalization
#
#   Revision 1.14  2008/09/24 19:34:34  ssa1
#   Fixing scaling bug in CTRL + zoom, Providing function for neighboring atoms, Saving volumes as projections
#
#   Revision 1.13  2008/09/23 16:46:57  ssa1
#   CTRL + Mouse wheel for iso-value modification
#
#   Revision 1.12  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpyGORGON import VolumeRenderer
from volume_surface_editor_form import VolumeSurfaceEditorForm
from volume_binary_skeletonization_form import VolumeBinarySkeletonizationForm
from volume_grayscale_skeletonization_form import VolumeGrayscaleSkeletonizationForm
from volume_manual_skeletonization_form import VolumeManualSkeletonizationForm
from model_visualization_form import ModelVisualizationForm
from volume_crop_form import VolumeCropForm
from volume_raw_loader_form import VolumeRawLoaderForm
from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class VolumeViewer(BaseViewer):
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Volume"    
        self.shortTitle = "VOL"      
        self.app.themes.addDefaultRGB("Volume:Model:0", 180, 180, 180, 255)
        self.app.themes.addDefaultRGB("Volume:Model:1", 180, 180, 180, 255)
        self.app.themes.addDefaultRGB("Volume:Model:2", 180, 180, 180, 255)
        self.app.themes.addDefaultRGB("Volume:BoundingBox", 255, 255, 255, 255)                         
        self.renderer = VolumeRenderer()          
        self.loaded = False
        self.createUI()
        self.app.viewers["volume"] = self
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))    
                      

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                          
    def createActions(self):
        openAct = QtGui.QAction(self.tr("&Volume..."), self)
        openAct.setShortcut(self.tr("Ctrl+V"))
        openAct.setStatusTip(self.tr("Load a volume file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_Volume", openAct)
        
        saveAct = QtGui.QAction(self.tr("&Volume..."), self)
        saveAct.setStatusTip(self.tr("Save a volume file"))
        self.connect(saveAct, QtCore.SIGNAL("triggered()"), self.saveData)
        self.app.actions.addAction("save_Volume", saveAct)        
        
        closeAct = QtGui.QAction(self.tr("&Volume"), self)
        closeAct.setStatusTip(self.tr("Close the loaded volume"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_Volume", closeAct)

        normalizeAct = QtGui.QAction(self.tr("N&ormalize"), self)
        normalizeAct.setStatusTip(self.tr("Normalized the loaded volume"))
        self.connect(normalizeAct, QtCore.SIGNAL("triggered()"), self.normalizeVolume)
        self.app.actions.addAction("normalize_Volume", normalizeAct)
        
        downsampleAct = QtGui.QAction(self.tr("Do&wnsample"), self)
        downsampleAct.setStatusTip(self.tr("Downsample the loaded volume"))
        self.connect(downsampleAct, QtCore.SIGNAL("triggered()"), self.downsampleVolume)
        self.app.actions.addAction("downsample_Volume", downsampleAct)     
         
 
                                               
    def createMenus(self):
        self.app.menus.addAction("file-open-volume", self.app.actions.getAction("load_Volume"), "file-open")
        self.app.menus.addAction("file-save-volume", self.app.actions.getAction("save_Volume"), "file-save");    
        self.app.menus.addAction("file-close-volume", self.app.actions.getAction("unload_Volume"), "file-close");
        self.app.menus.addMenu("actions-volume", self.tr("V&olume"), "actions");   
        self.app.menus.addAction("actions-volume-normalize", self.app.actions.getAction("normalize_Volume"), "actions-volume");
        self.app.menus.addAction("actions-volume-downsample", self.app.actions.getAction("downsample_Volume"), "actions-volume");
        self.app.menus.addMenu("actions-volume-skeletonization", self.tr("S&keletonization"), "actions-volume");               
    
    def createChildWindows(self):
        self.surfaceEditor = VolumeSurfaceEditorForm(self.app, self)
        self.manualSkeletonizer = VolumeManualSkeletonizationForm(self.app, self)
        self.binarySkeletonizer = VolumeBinarySkeletonizationForm(self.app, self)
        self.grayscaleSkeletonizer = VolumeGrayscaleSkeletonizationForm(self.app, self)
        self.cropper = VolumeCropForm(self.app, self)
        self.rawLoader = VolumeRawLoaderForm(self.app, self)
    
    def updateActionsAndMenus(self):
        self.app.actions.getAction("save_Volume").setEnabled(self.loaded)
        self.app.actions.getAction("unload_Volume").setEnabled(self.loaded)
        self.app.actions.getAction("normalize_Volume").setEnabled(self.loaded)
        self.app.menus.getMenu("actions-volume").setEnabled(self.loaded)       
    
    def normalizeVolume(self):
        self.renderer.normalizeVolume()
        self.surfaceEditor.modelLoadedPreDraw()
        
    def downsampleVolume(self):
        self.renderer.downsampleVolume()
        self.surfaceEditor.modelLoadedPreDraw()
        self.emitModelChanged();       
        
    def cropVolume(self):
        pass 
    
    def loadData(self):
        fileName = str(QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Volume"), "", self.tr(self.renderer.getSupportedLoadFileFormats())))
        self.loadDataFromFile(fileName)

    def loadDataFromFile(self, fileName):
        self.fileName = fileName
                
        if not self.fileName=="":  
            self.setCursor(QtCore.Qt.WaitCursor)
            
            tokens = split(str(self.fileName), '.')            
            extension = upper(tokens[len(tokens)-1])
            if(extension == "RAW"):
                if(self.rawLoader.exec_() == QtGui.QDialog.Accepted):                
                    self.renderer.loadFileRAW(str(self.fileName), self.rawLoader.bitsPerCell(), self.rawLoader.sizeX(), self.rawLoader.sizeY(), self.rawLoader.sizeZ())
                else:
                    return;
                    
            else:
                self.renderer.loadFile(str(self.fileName))
            self.setScaleNoEmit(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())       
            self.loaded = True
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoadedPreDraw()
            self.emitModelLoaded()            
            self.emitViewerSetCenter()        
    
    def processMouseWheel(self, amount, event):
        if(event.modifiers() & QtCore.Qt.CTRL) :
            range = self.surfaceEditor.ui.horizontalSliderIsoLevel.maximum() - self.surfaceEditor.ui.horizontalSliderIsoLevel.minimum()
            delta = round(range * amount / 100.0)
            
            self.surfaceEditor.ui.horizontalSliderIsoLevel.setValue(self.surfaceEditor.ui.horizontalSliderIsoLevel.value() - delta)
    
    def setCenter(self, center):
        [xx, yy, zz] = self.worldToObjectCoordinates(center)
        self.renderer.setDisplayRadiusOrigin(xx, yy, zz)
        return True      
    
    def getIsoValue(self):
        return self.renderer.getSurfaceValue()
                          
                          
    def getSessionInfo(self, sessionManager):
        info = BaseViewer.getSessionInfo(self, sessionManager)  
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "ISO_LEVEL", self.surfaceEditor.ui.horizontalSliderIsoLevel.value()))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "MAX_ISO_LEVEL", self.surfaceEditor.ui.horizontalSliderIsoLevelMax.value()))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "SAMPLING_INTERVAL", self.surfaceEditor.ui.horizontalSliderSampling.value()))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "DISPLAY_RADIUS", self.surfaceEditor.ui.horizontalSliderDisplayRadius.value()))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "VIEWING_TYPE_SURFACE", self.surfaceEditor.ui.radioButtonIsoSurface.isChecked()))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "VIEWING_TYPE_SOLID", self.surfaceEditor.ui.radioButtonSolid.isChecked()))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "VIEWING_TYPE_CROSS_SECTION", self.surfaceEditor.ui.radioButtonCrossSection.isChecked()))
        info.extend(sessionManager.getRemarkLines(self.shortTitle, "USE_DISPLAY_RADIUS", self.surfaceEditor.ui.checkBoxUseRadius.checkState()))        
        return info
                       
    def loadSessionInfo(self, sessionManager, sessionProperties):
        BaseViewer.loadSessionInfo(self, sessionManager, sessionProperties)
               
        isoLevel = sessionManager.getProperty(sessionProperties, self.shortTitle, "ISO_LEVEL")
        self.surfaceEditor.ui.horizontalSliderIsoLevel.setValue(isoLevel)
        
        maxIsoLevel = sessionManager.getProperty(sessionProperties, self.shortTitle, "MAX_ISO_LEVEL")
        self.surfaceEditor.ui.horizontalSliderIsoLevelMax.setValue(maxIsoLevel)
        
        samplingInterval = sessionManager.getProperty(sessionProperties, self.shortTitle, "SAMPLING_INTERVAL")
        self.surfaceEditor.ui.horizontalSliderSampling.setValue(samplingInterval)
                
        useDisplayRadius = sessionManager.getProperty(sessionProperties, self.shortTitle, "USE_DISPLAY_RADIUS")
        self.surfaceEditor.ui.checkBoxUseRadius.setCheckState(useDisplayRadius)
                
        displayRadius = sessionManager.getProperty(sessionProperties, self.shortTitle, "DISPLAY_RADIUS")
        self.surfaceEditor.ui.horizontalSliderDisplayRadius.setValue(displayRadius)
        
        surfaceViewing = sessionManager.getProperty(sessionProperties, self.shortTitle, "VIEWING_TYPE_SURFACE")
        solidViewing = sessionManager.getProperty(sessionProperties, self.shortTitle, "VIEWING_TYPE_SOLID")
        crossSectionViewing = sessionManager.getProperty(sessionProperties, self.shortTitle, "VIEWING_TYPE_CROSS_SECTION")
        
        self.surfaceEditor.ui.radioButtonIsoSurface.setChecked(surfaceViewing)
        self.surfaceEditor.ui.radioButtonSolid.setChecked(solidViewing)
        self.surfaceEditor.ui.radioButtonCrossSection.setChecked(crossSectionViewing)
     