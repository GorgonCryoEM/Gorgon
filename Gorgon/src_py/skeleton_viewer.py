from PyQt4 import QtGui, QtCore, QtOpenGL
from gorgon_cpp_wrapper import MeshRenderer
from skeleton_laplacian_smoothing_form import SkeletonLaplacianSmoothingForm

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class SkeletonViewer(QtGui.QWidget):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.renderer = MeshRenderer()          
        self.loaded = False
        self.showBox = True
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.meshChanged)
        self.createUI()      
        self.gllist = 0

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
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadSkeleton)
        self.app.actions.addAction("load_Skeleton", openAct)
        
        closeAct = QtGui.QAction(self.tr("S&keleton"), self)
        closeAct.setStatusTip(self.tr("Close the loaded skeleton"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadSkeleton)
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
                
    def draw(self):
        if self.gllist != 0:
            glPushMatrix()
            glTranslated(-0.5, -0.5, -0.5)
            glCallList(self.gllist)
            glPopMatrix()
    
    def updateActionsAndMenus(self):
        self.app.actions.getAction("unload_Skeleton").setEnabled(self.loaded)
        self.app.actions.getAction("center_Skeleton").setEnabled(self.loaded)
        self.app.actions.getAction("boundingBox_Skeleton").setEnabled(self.loaded)   
        self.app.menus.getMenu("options-visualization-skeleton").setEnabled(self.loaded) 
        self.app.menus.getMenu("actions-skeleton").setEnabled(self.loaded)       
          
    def loadSkeleton(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open skeleton"), "", self.tr(self.renderer.getSupportedFileFormats()))
        if not fileName.isEmpty():  
            self.renderer.loadFile(str(fileName))
            self.loaded = True
            self.emitModelChanged()
            self.emitViewerSetCenter()
    
    def unloadSkeleton(self):
        self.renderer.unload()
        self.loaded = False
        self.emitModelChanged()

    def setShowBox(self):
        self.showBox = self.app.actions.getAction("boundingBox_Skeleton").isChecked()
        self.emitModelChanged()
        
    def meshChanged(self):
        self.updateActionsAndMenus()
        if self.gllist != 0:
            glDeleteLists(self.gllist,1)
            
        self.gllist = glGenLists(1)
        glNewList(self.gllist, GL_COMPILE)
        self.renderer.draw()
        
        if(self.loaded and self.showBox):
            glPushAttrib(GL_LIGHTING_BIT)
            glDisable(GL_LIGHTING)
            self.renderer.drawBoundingBox()
            glPopAttrib()

        glEndList()            

    def emitModelChanged(self):
        self.emit(QtCore.SIGNAL("modelChanged()"))
        
    def emitViewerSetCenter(self):
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float, float, float)"), 
                                self.renderer.getMin(0), self.renderer.getMin(1), self.renderer.getMin(2),
                                self.renderer.getMax(0), self.renderer.getMax(1), self.renderer.getMax(2))
       