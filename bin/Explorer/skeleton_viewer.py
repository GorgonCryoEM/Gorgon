from PyQt4 import QtGui, QtCore, QtOpenGL
from libpytoolkit import MeshRenderer
from base_viewer import BaseViewer
from model_visualization_form import ModelVisualizationForm


from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class SkeletonViewer(BaseViewer):

    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Skeleton"
        self.shortTitle = "SKE"

        self.renderer = MeshRenderer()
        
        self.isClosedMesh = False
        self.twoWayLighting = True
        self.lineThickness = 3
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
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.optionsWindow = SkeletonLaplacianSmoothingForm(self.app, self, self)
        
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
