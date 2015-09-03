import sys

from PyQt4 import QtCore, QtGui
from PyQt4.QtOpenGL import *

from OpenGL.GL import *

from cube import drawWireCube

class GLWidget(QGLWidget):
    def __init__(self, parent = None):
        super(GLWidget, self).__init__(parent)
        
    def initializeGL(self):
        glEnable(GL_DEPTH_TEST)
        glClearColor(0,0,0,1)
        glRotate(10,1,2,0)
        self.cubeColor = QtCore.Qt.yellow        
    
    def resizeGL(self, w, h):
        glViewport(0, 0, w, h)
        
    def paintGL(self):        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        self.qglColor(self.cubeColor)

        drawWireCube()
        
    def mouseDoubleClickEvent(self, e):        
        self.cubeColor = QtGui.QColorDialog.getColor()
        self.updateGL()


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    
    window = GLWidget()
    window.show()
    window.raise_()
    
    app.exec_()
