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
        self.showBox = False;
        self.renderer = SSERenderer()
        self.skeletonViewer = skeletonViewer
        self.connect(self.skeletonViewer, QtCore.SIGNAL("modelChanged()"), self.emitModelChanged)
        self.connect(self.skeletonViewer, QtCore.SIGNAL("modelUnloaded()"), self.unloadData)
        self.renderer.setSkeletonRenderer(self.skeletonViewer.renderer)        
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
    
    def loadHelixAppend(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadHelixFile(str(fileName), False)
            self.loaded = True
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoaded()
            self.emitViewerSetCenter()        
               
    def createActions(self):
        openHelixReplaceAct = QtGui.QAction(self.tr("&Helix Annotations (Overwrite)"), self)
        openHelixReplaceAct.setShortcut(self.tr("Ctrl+H"))
        openHelixReplaceAct.setStatusTip(self.tr("Erase current annotations and load a helix file"))
        self.connect(openHelixReplaceAct, QtCore.SIGNAL("triggered()"), self.loadData)
        self.app.actions.addAction("load_SSE_Helix_Overwrite", openHelixReplaceAct)
        
        openHelixAppendAct = QtGui.QAction(self.tr("Heli&x Annotations (A&ppend)"), self)
        openHelixAppendAct.setShortcut(self.tr("Ctrl+X"))
        openHelixAppendAct.setStatusTip(self.tr("Load a helix file and append it to the current annotations"))
        self.connect(openHelixAppendAct, QtCore.SIGNAL("triggered()"), self.loadHelixAppend)
        self.app.actions.addAction("load_SSE_Helix_Append", openHelixAppendAct)
        
        closeAct = QtGui.QAction(self.tr("SSE Annotations"), self)
        closeAct.setStatusTip(self.tr("Close the loaded secondary structure element file"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_SSE", closeAct)
        
                        
    def createMenus(self):
        self.app.menus.addAction("file-open-helixOverwrite", self.app.actions.getAction("load_SSE_Helix_Overwrite"), "file-open")    
        self.app.menus.addAction("file-open-helixAppend", self.app.actions.getAction("load_SSE_Helix_Append"), "file-open")
        self.app.menus.addAction("file-close-sse", self.app.actions.getAction("unload_SSE"), "file-close");
        self.app.menus.addMenu("actions-sse", self.tr("Secondary Structure &Element"), "actions");
                   
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_SSE").setEnabled(self.loaded)
        self.app.menus.getMenu("actions-sse").setEnabled(self.loaded)       
          
    
             
