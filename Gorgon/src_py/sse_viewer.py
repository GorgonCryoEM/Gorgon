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
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Secondary Structure Element"
        self.isClosedMesh = False
        self.showBox = False;
        self.renderer = SSERenderer()
        self.createUI()      
        self.app.viewers["sse"] = self;
        self.modelColor = QtGui.QColor.fromRgba(QtGui.qRgba(0, 180, 0, 255))
        self.model2Color = QtGui.QColor.fromRgba(QtGui.qRgba(120, 185, 255, 255))
        self.model2Visible = True
        self.initVisualizationOptions()
        self.visualizationOptions.ui.checkBoxModelVisible.setText("Show helices colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setText("Show sheets colored:")
        self.visualizationOptions.ui.checkBoxModel2Visible.setVisible(True)
        self.visualizationOptions.ui.pushButtonModel2Color.setVisible(True)
    

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.createChildWindows()
        self.updateActionsAndMenus()
                  
    def createChildWindows(self):
        pass
    
    def loadHelixData(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadHelixFile(str(fileName))
            self.loaded = True
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoaded()
            self.emitViewerSetCenter()        
               
    def loadSheetData(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadSheetFile(str(fileName))
            self.loaded = True
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoaded()
            self.emitViewerSetCenter()                 
               
               
    def modelChanged(self):
        self.updateActionsAndMenus()
        if self.gllist != 0:
            glDeleteLists(self.gllist,1)
            
        self.gllist = glGenLists(1)
        glNewList(self.gllist, GL_COMPILE)
        
        if(self.loaded and self.modelVisible):
            self.setMaterials(self.modelColor)
            self.renderer.draw(0)

        if(self.loaded and self.model2Visible):
            self.setMaterials(self.model2Color)
            self.renderer.draw(1)
        
        if(self.loaded and self.showBox):
            self.setMaterials(self.boxColor)
            self.renderer.drawBoundingBox()

        glEndList()                
               
               
    def createActions(self):
        openHelixAct = QtGui.QAction(self.tr("&Helix Annotations"), self)
        openHelixAct.setShortcut(self.tr("Ctrl+H"))
        openHelixAct.setStatusTip(self.tr("Load a helix file"))
        self.connect(openHelixAct, QtCore.SIGNAL("triggered()"), self.loadHelixData)
        self.app.actions.addAction("load_SSE_Helix", openHelixAct)

        openSheetAct = QtGui.QAction(self.tr("&Sheet Annotations"), self)
        openSheetAct.setShortcut(self.tr("Ctrl+S"))
        openSheetAct.setStatusTip(self.tr("Load a sheet file"))
        self.connect(openSheetAct, QtCore.SIGNAL("triggered()"), self.loadSheetData)
        self.app.actions.addAction("load_SSE_Sheet", openSheetAct)
               
        closeAct = QtGui.QAction(self.tr("SSE Annotations"), self)
        closeAct.setStatusTip(self.tr("Close the loaded secondary structure element file"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_SSE", closeAct)
        
                        
    def createMenus(self):
        self.app.menus.addAction("file-open-helix", self.app.actions.getAction("load_SSE_Helix"), "file-open")    
        self.app.menus.addAction("file-open-sheet", self.app.actions.getAction("load_SSE_Sheet"), "file-open")        
        self.app.menus.addAction("file-close-sse", self.app.actions.getAction("unload_SSE"), "file-close");
        self.app.menus.addMenu("actions-sse", self.tr("Secondary Structure &Element"), "actions");
                   
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_SSE").setEnabled(self.loaded)
        self.app.menus.getMenu("actions-sse").setEnabled(self.loaded)       
          
    
             
