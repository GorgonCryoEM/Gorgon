from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpyGORGON import VolumeRenderer
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
    

    def createMenus(self):
        self.app.menus.addAction("file-open-volume", self.app.actions.getAction("load_Volume"), "file-open")
        self.app.menus.addAction("file-save-volume", self.app.actions.getAction("save_Volume"), "file-save");    
        self.app.menus.addAction("file-close-volume", self.app.actions.getAction("unload_Volume"), "file-close");
    
    def createChildWindows(self):
        self.surfaceEditor = VolumeSurfaceEditorForm(self.app, self, self)
    
    def updateActionsAndMenus(self):
        self.app.actions.getAction("save_Volume").setEnabled(self.loaded)
        self.app.actions.getAction("unload_Volume").setEnabled(self.loaded)
    
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
            range = self.surfaceEditor.ui.histogram.maximumValue() - self.surfaceEditor.ui.histogram.minimumValue()
            delta = range * amount / 100.0
            
            self.surfaceEditor.ui.histogram.setLowerValue(self.surfaceEditor.ui.histogram.lowerValue() - delta)
    
    def setCenter(self, center):
        [xx, yy, zz] = self.worldToObjectCoordinates(center)
        self.renderer.setDisplayRadiusOrigin(xx, yy, zz)
        return self.surfaceEditor.ui.checkBoxUseRadius.isChecked()
    
    def getIsoValue(self):
        return self.renderer.getSurfaceValue()
                          
