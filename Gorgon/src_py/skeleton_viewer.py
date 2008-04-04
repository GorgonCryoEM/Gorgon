from PyQt4 import QtGui, QtCore, QtOpenGL
from gorgon_cpp_wrapper import MeshRenderer

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
        self.createUI()
        self.renderer = MeshRenderer()        
        self.gllist = 0
        self.connect(self, QtCore.SIGNAL("skeletonChanged()"), self.meshChanged)

    def createUI(self):
        self.createActions()
        self.createMenus()
               
    
    def createActions(self):
        openAct = QtGui.QAction(self.tr("S&keleton..."), self)
        openAct.setShortcut(self.tr("Ctrl+K"))
        openAct.setStatusTip(self.tr("Load a skeleton file"))
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.openDialog)
        self.app.actions.addAction("load_Skeleton", openAct)

    def createMenus(self):
        self.app.menus.addAction("file-open-skeleton", self.app.actions.getAction("load_Skeleton"), "file-open")    

    def openDialog(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open skeleton"), "", self.tr(self.renderer.getSupportedFileFormats()))
        if not fileName.isEmpty():
            self.load(fileName)
                
    def draw(self):
        self.drawMesh()
        
    def drawMesh(self):
        if self.gllist != 0:
            glPushMatrix()
            glTranslated(-0.5, -0.5, -0.5)
            glCallList(self.gllist)
            glPopMatrix()
            
    def load(self, fileName):
        self.renderer.loadFile(str(fileName))
        self.renderer.center();
        self.renderer.focus();
        self.emit(QtCore.SIGNAL("skeletonChanged()"))
        
    def meshChanged(self):
        if self.gllist != 0:
            glDeleteLists(self.gllist,1)
            
        self.gllist = glGenLists(1)
        glNewList(self.gllist, GL_COMPILE)
        self.renderer.draw()
        glEndList()            
