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
        self.renderer = MeshRenderer()          
        self.createUI()      

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        self.optionsWindow = SkeletonLaplacianSmoothingForm(self.app, self)
        
    def createActions(self):
        openAct = QtGui.QAction(self.tr("S&keleton..."), self)
        openAct.setShortcut(self.tr("Ctrl+K"))
        openAct.setStatusTip(self.tr("Load a skeleton file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_Skeleton", openAct)
        
        closeAct = QtGui.QAction(self.tr("S&keleton"), self)
        closeAct.setStatusTip(self.tr("Close the loaded skeleton"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_Skeleton", closeAct)
        
        boundingBoxAct = QtGui.QAction(self.tr("Show bounding box"), self)
        boundingBoxAct.setStatusTip(self.tr("Show a bounding box surrounding the skeleton"))
        boundingBoxAct.setCheckable(True)
        boundingBoxAct.setChecked(self.showBox)
        self.connect(boundingBoxAct, QtCore.SIGNAL("triggered()"), self.setShowBox)
        self.app.actions.addAction("boundingBox_Skeleton", boundingBoxAct)
                
        centerAct = QtGui.QAction(self.tr("&Center"), self)
        self.connect(centerAct, QtCore.SIGNAL("triggered()"), self.emitViewerSetCenter)
        self.app.actions.addAction("center_Skeleton", centerAct)
                        
    def createMenus(self):
        self.app.menus.addAction("file-open-skeleton", self.app.actions.getAction("load_Skeleton"), "file-open")    
        self.app.menus.addAction("file-close-skeleton", self.app.actions.getAction("unload_Skeleton"), "file-close");
        self.app.menus.addMenu("options-visualization-skeleton", self.tr("S&keleton"), "options-visualization");
        self.app.menus.addAction("options-visualization-skeleton-boundingBox", self.app.actions.getAction("boundingBox_Skeleton"), "options-visualization-skeleton")
        self.app.menus.addAction("options-visualization-skeleton-center", self.app.actions.getAction("center_Skeleton"), "options-visualization-skeleton");     
        self.app.menus.addMenu("actions-skeleton", self.tr("S&keleton"), "actions");
                   
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_Skeleton").setEnabled(self.loaded)
        self.app.actions.getAction("center_Skeleton").setEnabled(self.loaded)
        self.app.actions.getAction("boundingBox_Skeleton").setEnabled(self.loaded)   
        self.app.menus.getMenu("options-visualization-skeleton").setEnabled(self.loaded) 
        self.app.menus.getMenu("actions-skeleton").setEnabled(self.loaded)       
          
    def setShowBox(self):
        self.showBox = self.app.actions.getAction("boundingBox_Skeleton").isChecked()
        self.emitModelChanged()
      
