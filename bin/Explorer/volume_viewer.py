from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import VolumeRenderer
from volume_surface_editor_form import VolumeSurfaceEditorForm
from model_visualization_form import ModelVisualizationForm
from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class VolumeViewer(BaseViewer):

    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Volume"
        self.shortTitle = "VOL"

        self.menu = self.app.menubar.addMenu('Volume')

        self.renderer = VolumeRenderer()
        self.loaded = False
        self.createUI()
        self.app.viewers["volume"] = self
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
                      
    def createUI(self):
        self.createActions()
        self.createChildWindows()
        self.updateActionsAndMenus()
                          
    def createActions(self):
        self.openAct = QtGui.QAction(self.tr("&Open"), self)
        self.openAct.setShortcut(self.tr("Ctrl+V"))
        self.openAct.setStatusTip(self.tr("Load a volume file"))
        self.openAct.triggered.connect(self.loadData)
        
        self.saveAct = QtGui.QAction(self.tr("&Save"), self)
        self.saveAct.setStatusTip(self.tr("Save a volume file"))
        self.saveAct.triggered.connect(self.saveData)
        
        self.closeAct = QtGui.QAction(self.tr("&Close"), self)
        self.closeAct.setStatusTip(self.tr("Close the loaded volume"))
        self.closeAct.triggered.connect(self.unloadData)

        self.menu.addAction(self.openAct)
        self.menu.addAction(self.saveAct)
        self.menu.addAction(self.closeAct)
        
    def createChildWindows(self):
        self.surfaceEditor = VolumeSurfaceEditorForm(self.app, self, self)
    
    def updateActionsAndMenus(self):
        self.saveAct.setEnabled(self.loaded)
        self.closeAct.setEnabled(self.loaded)
    
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
        if(event.modifiers() & QtCore.Qt.CTRL):
            range = self.surfaceEditor.ui.histogram.maximumValue() - self.surfaceEditor.ui.histogram.minimumValue()
            delta = range * amount / 100.0
            
            self.surfaceEditor.ui.histogram.setLowerValue(self.surfaceEditor.ui.histogram.lowerValue() - delta)
    
    def setCenter(self, center):
        [xx, yy, zz] = self.worldToObjectCoordinates(center)
        self.renderer.setDisplayRadiusOrigin(xx, yy, zz)
        return self.surfaceEditor.ui.checkBoxUseRadius.isChecked()
    
    def getIsoValue(self):
        return self.renderer.getSurfaceValue()
                          
