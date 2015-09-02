from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

class GLWidget(QtOpenGL.QGLWidget):
    def __init__(self):
        super(GLWidget, self).__init__()
        
        self.L = 0.5
    
    def initializeGL(self):
        self.setMinimumSize(300, 300)
        self.setWindowTitle("Yay")
    
    def paintGL(self):    
        glClear(GL_COLOR_BUFFER_BIT)
        glClearColor(0,0,0,1)
        
        L = self.L
        glBegin(GL_LINE_LOOP)
        glColor(.5,0,0,1)
        glVertex(-L,-L,0)
        glVertex(+L,+L,0)
        glVertex(-L,+L,0)
        glVertex(+L,-L,0)
        glEnd()
        
        
app = QtGui.QApplication(["My OpenGL"])

window = GLWidget()

window.show()
window.raise_()

app.exec_()
