from PyQt4 import QtGui, QtCore, QtOpenGL
from gorgon_cpp_wrapper import VolumeRenderer

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class BaseViewer(QtGui.QWidget):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main      
        self.loaded = False
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.modelChanged) 
        self.connect(self, QtCore.SIGNAL("modelLoaded()"), self.modelChanged) 
        self.connect(self, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged)    
        self.gllist = 0
        self.transparency = 0
        self.showBox = True
                                          
    def draw(self):
        if self.gllist != 0:
            glPushMatrix()
            glTranslated(-0.5, -0.5, -0.5)
            glCallList(self.gllist)
            glPopMatrix()    
          
    def loadData(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not fileName.isEmpty():  
            self.renderer.loadFile(str(fileName))
            self.loaded = True
            self.emitModelLoaded()
            self.emitViewerSetCenter()
    
    def unloadData(self):
        self.renderer.unload()
        self.loaded = False
        self.emitModelUnloaded()
        
    def modelChanged(self):
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
        
    def setTransparencyValue(self, value):
        self.transparency = value;   
        self.emitModelChanged()

    def emitModelLoaded(self):
        self.emit(QtCore.SIGNAL("modelLoaded()"))

    def emitModelUnloaded(self):
        self.emit(QtCore.SIGNAL("modelUnloaded()"))
        
    def emitModelChanged(self):
        self.emit(QtCore.SIGNAL("modelChanged()"))
        
    def emitViewerSetCenter(self):
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float, float, float)"), 
                                self.renderer.getMin(0), self.renderer.getMin(1), self.renderer.getMin(2),
                                self.renderer.getMax(0), self.renderer.getMax(1), self.renderer.getMax(2))
       