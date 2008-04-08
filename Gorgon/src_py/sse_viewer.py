from PyQt4 import QtGui, QtCore, QtOpenGL
from gorgon_cpp_wrapper import SSERenderer
from base_viewer import BaseViewer

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class SSEViewer(BaseViewer):
    def __init__(self, main, skeletonViewer, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Secondary Structure Element"
        self.isClosedMesh = False
        self.renderer = SSERenderer()
        self.skeletonViewer = skeletonViewer
        self.connect(self.skeletonViewer, QtCore.SIGNAL("modelLoaded()"), self.loadData)
        self.connect(self.skeletonViewer, QtCore.SIGNAL("modelChanged()"), self.emitModelChanged)                
        self.connect(self.skeletonViewer, QtCore.SIGNAL("modelUnloaded()"), self.unloadData)
        self.createUI()      
        self.app.viewers["sse"] = self;
        self.modelColor = QtGui.QColor.fromRgba(QtGui.qRgba(0, 180, 0, 255))
        self.initVisualizationOptions()        

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        pass
    
    def loadData(self):
        self.renderer.setSkeletonRenderer(self.skeletonViewer.renderer)
        self.renderer.performAutomaticAnnotation(1.0,1.0,1.0,1.0)
        self.loaded = True
        self.emitModelLoaded();
        
    def unloadData(self):
        self.renderer.setSkeletonRenderer(0)
        self.loaded = False
        self.emitModelUnloaded();
            
    def createActions(self):
        openAct = QtGui.QAction(self.tr("Secondary Structure &Element..."), self)
        openAct.setShortcut(self.tr("Ctrl+E"))
        openAct.setStatusTip(self.tr("Load a secondary structure element file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_SSE", openAct)
        
        closeAct = QtGui.QAction(self.tr("Secondary Structure &Element"), self)
        closeAct.setStatusTip(self.tr("Close the loaded secondary structure element file"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_SSE", closeAct)
        
                        
    def createMenus(self):
        self.app.menus.addAction("file-open-sse", self.app.actions.getAction("load_SSE"), "file-open")    
        self.app.menus.addAction("file-close-sse", self.app.actions.getAction("unload_SSE"), "file-close");
        self.app.menus.addMenu("actions-sse", self.tr("Secondary Structure &Element"), "actions");
                   
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_SSE").setEnabled(self.loaded)
        self.app.menus.getMenu("actions-sse").setEnabled(self.loaded)       
          
    
             
