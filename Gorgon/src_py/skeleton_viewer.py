# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A viewer for skeletons 


from PyQt4 import QtGui, QtCore, QtOpenGL
from libpyGORGON import MeshRenderer
from base_viewer import BaseViewer
from model_visualization_form import ModelVisualizationForm
from skeleton_laplacian_smoothing_form import SkeletonLaplacianSmoothingForm


from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class SkeletonViewer(BaseViewer):
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Skeleton"
        self.shortTitle = "SKE"              
        self.app.themes.addDefaultRGB("Skeleton:Model:0", 180, 0, 0, 255)
        self.app.themes.addDefaultRGB("Skeleton:Model:1", 200, 150, 0, 255)
        self.app.themes.addDefaultRGB("Skeleton:Model:2", 0, 0, 0, 255)
        self.app.themes.addDefaultRGB("Skeleton:BoundingBox", 255, 255, 255, 255)              
        self.isClosedMesh = False
        self.twoWayLighting = True
        self.lineThickness = 3
        self.renderer = MeshRenderer()          
        self.renderer.setLineThickness(self.lineThickness)
        self.createUI()      
        self.app.viewers["skeleton"] = self;
        self.volumeViewer = self.app.viewers["volume"]
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))      
        self.visualizationOptions.ui.spinBoxThickness.setValue(self.lineThickness)
        self.visualizationOptions.ui.spinBoxThickness.setVisible(True)
        self.visualizationOptions.ui.labelThickness.setVisible(True)

        self.model2Visible = True
        self.model3Visible = True
        self.visualizationOptions.ui.checkBoxModelVisible.setText("Show curves colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setText("Show surfaces colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel2Color.setVisible(True)
        self.visualizationOptions.ui.checkBoxModel3Visible.setText("Show surface borders:")
        self.visualizationOptions.ui.checkBoxModel3Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel3Color.setVisible(True)        
    
                 
    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.optionsWindow = SkeletonLaplacianSmoothingForm(self.app, self, self)
        
    def createActions(self):
        openAct = QtGui.QAction(self.tr("S&keleton..."), self)
        openAct.setShortcut(self.tr("Ctrl+K"))
        openAct.setStatusTip(self.tr("Load a skeleton file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_Skeleton", openAct)
        
        saveAct = QtGui.QAction(self.tr("S&keleton..."), self)
        saveAct.setStatusTip(self.tr("Save a skeleton file"))
        self.connect(saveAct, QtCore.SIGNAL("triggered()"), self.saveData)
        self.app.actions.addAction("save_Skeleton", saveAct)                
        
        closeAct = QtGui.QAction(self.tr("S&keleton"), self)
        closeAct.setStatusTip(self.tr("Close the loaded skeleton"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_Skeleton", closeAct)
                                
    def createMenus(self):
        self.app.menus.addAction("file-open-skeleton", self.app.actions.getAction("load_Skeleton"), "file-open")    
        self.app.menus.addAction("file-save-skeleton", self.app.actions.getAction("save_Skeleton"), "file-save")
        self.app.menus.addAction("file-close-skeleton", self.app.actions.getAction("unload_Skeleton"), "file-close");
        self.app.menus.addMenu("actions-skeleton", self.tr("S&keleton"), "actions");
                   
    def updateActionsAndMenus(self):
        if(self.viewerAutonomous):
            self.app.actions.getAction("load_Skeleton").setEnabled(True)
            self.app.actions.getAction("save_Skeleton").setEnabled(self.loaded)
            self.app.actions.getAction("unload_Skeleton").setEnabled(self.loaded)
            self.app.menus.getMenu("actions-skeleton").setEnabled(True)
        
    def updateViewerAutonomy(self, autonomous): 
        if(not autonomous):
            self.app.actions.getAction("load_Skeleton").setEnabled(autonomous)
            self.app.actions.getAction("save_Skeleton").setEnabled(autonomous)
            self.app.actions.getAction("unload_Skeleton").setEnabled(autonomous)
            self.app.menus.getMenu("actions-skeleton").setEnabled(autonomous)
        else:            
            self.updateActionsAndMenus()        
                   
    def loadVolume(self, volume):
        if(self.loaded):
            self.unloadData
        self.renderer.loadVolume(volume)
        self.loaded = True
        self.emitModelLoaded()
        self.emitViewerSetCenter()             
