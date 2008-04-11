from PyQt4 import QtGui, QtCore, QtOpenGL
from gorgon_cpp_wrapper import MeshRenderer
from base_viewer import BaseViewer
from skeleton_laplacian_smoothing_form import SkeletonLaplacianSmoothingForm

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class SkeletonViewer(BaseViewer):
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Skeleton"
        self.isClosedMesh = False
        self.renderer = MeshRenderer()          
        self.createUI()      
        self.app.viewers["skeleton"] = self;
        self.modelColor = QtGui.QColor.fromRgba(QtGui.qRgba(180, 0, 0, 255))
        self.initVisualizationOptions()      
                 
    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.optionsWindow = SkeletonLaplacianSmoothingForm(self.app, self)
        
    def createActions(self):
        openAct = QtGui.QAction(self.tr("&S&keleton..."), self)
        openAct.setShortcut(self.tr("Ctrl+K"))
        openAct.setStatusTip(self.tr("Load a skeleton file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_Skeleton", openAct)
        
        closeAct = QtGui.QAction(self.tr("S&keleton"), self)
        closeAct.setStatusTip(self.tr("Close the loaded skeleton"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_Skeleton", closeAct)
                                
    def createMenus(self):
        self.app.menus.addAction("file-open-skeleton", self.app.actions.getAction("load_Skeleton"), "file-open")    
        self.app.menus.addAction("file-close-skeleton", self.app.actions.getAction("unload_Skeleton"), "file-close");
        self.app.menus.addMenu("actions-skeleton", self.tr("S&keleton"), "actions");
                   
    def updateActionsAndMenus(self):
        if(self.viewerAutonomous):
            self.app.actions.getAction("load_Skeleton").setEnabled(True)
            self.app.actions.getAction("unload_Skeleton").setEnabled(self.loaded)
            self.app.menus.getMenu("actions-skeleton").setEnabled(self.loaded)
        
    def updateViewerAutonomy(self, autonomous): 
        if(not autonomous):
            self.app.actions.getAction("load_Skeleton").setEnabled(autonomous)
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
             
