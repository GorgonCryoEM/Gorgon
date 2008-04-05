from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from gorgon_cpp_wrapper import VolumeRenderer
from volume_surface_editor_form import VolumeSurfaceEditorForm

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class VolumeViewer(BaseViewer):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.renderer = VolumeRenderer()          
        self.loaded = False
        self.showBox = True
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.createUI()      
        self.gllist = 0

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                          
    def createActions(self):
        openAct = QtGui.QAction(self.tr("V&olume..."), self)
        openAct.setShortcut(self.tr("Ctrl+O"))
        openAct.setStatusTip(self.tr("Load a volume file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_Volume", openAct)
        
        closeAct = QtGui.QAction(self.tr("V&olume"), self)
        closeAct.setStatusTip(self.tr("Close the loaded volume"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_Volume", closeAct)
        
        boundingBoxAct = QtGui.QAction(self.tr("Show bounding box"), self)
        boundingBoxAct.setStatusTip(self.tr("Show a bounding box surrounding the volume"))
        boundingBoxAct.setCheckable(True)
        boundingBoxAct.setChecked(self.showBox)
        self.connect(boundingBoxAct, QtCore.SIGNAL("triggered()"), self.setShowBox)
        self.app.actions.addAction("boundingBox_Volume", boundingBoxAct)
                
        centerAct = QtGui.QAction(self.tr("&Center"), self)
        self.connect(centerAct, QtCore.SIGNAL("triggered()"), self.emitViewerSetCenter)
        self.app.actions.addAction("center_Volume", centerAct)
                        
    def createMenus(self):
        self.app.menus.addAction("file-open-volume", self.app.actions.getAction("load_Volume"), "file-open")    
        self.app.menus.addAction("file-close-volume", self.app.actions.getAction("unload_Volume"), "file-close");
        self.app.menus.addMenu("options-visualization-volume", self.tr("V&olume"), "options-visualization");
        self.app.menus.addAction("options-visualization-volume-boundingBox", self.app.actions.getAction("boundingBox_Volume"), "options-visualization-volume")
        self.app.menus.addAction("options-visualization-volume-center", self.app.actions.getAction("center_Volume"), "options-visualization-volume");     
        self.app.menus.addMenu("actions-volume", self.tr("V&olume"), "actions");             
    
    def createChildWindows(self):
        self.surfaceEditor = VolumeSurfaceEditorForm(self.app, self)
        
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_Volume").setEnabled(self.loaded)
        self.app.actions.getAction("center_Volume").setEnabled(self.loaded)
        self.app.actions.getAction("boundingBox_Volume").setEnabled(self.loaded)   
        self.app.menus.getMenu("options-visualization-volume").setEnabled(self.loaded) 
        self.app.menus.getMenu("actions-volume").setEnabled(self.loaded)       
          
    def setShowBox(self):
        self.showBox = self.app.actions.getAction("boundingBox_Volume").isChecked()
        self.emitModelChanged()           