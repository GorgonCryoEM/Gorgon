from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

class GLWidget(QtOpenGL.QGLWidget):
    def __init__(self):
        super(GLWidget, self).__init__()
        
        self.setMinimumSize(300, 300)
        self.setWindowTitle("Yay")

        self.L = 0.5
    
    def initializeGL(self):
        glEnable(GL_DEPTH_TEST)
    
    def paintGL(self):    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glClearColor(0,0,0,1)
        
        glPointSize(10)
        glBegin(GL_POINTS)
        glColor(.5,.5,.5,1)
        glVertex(0,0,0,1)
        glEnd()
        
        L = self.L
        z = -.3
        
        glBegin(GL_LINES)
        
        glColor(.5,0,0,1)
        
        glVertex(-L,-L, z*2)
        glVertex(+L,+L, z*2)
        glVertex(+L,-L, z*2)
        glVertex(-L,+L, z*2)
        
        glEnd()
        
        glBegin(GL_LINE_LOOP)
        glColor(.5,0,0,1)
        
        glVertex(-L,-L, z)
        glVertex(+L,-L, z)
        glVertex(+L,+L, z)
        glVertex(-L,+L, z)
        
        glEnd()
        
        glBegin(GL_LINE_LOOP)
        L /= 2.
        glColor(0,.5,0,1)
        glVertex(-L,-L, 2*z)
        glVertex(+L,-L, 2*z)
        glVertex(+L,+L, 2*z)
        glVertex(-L,+L, 2*z)
        
        glEnd()
        
        glBegin(GL_LINE_STRIP)
        L *= 2.
        glColor(0,0,.5,1)
        
        glVertex(-L,-L, z)
        glVertex(-L/2,-L/2, 2*z)
        
        glVertex(+L,-L, z)
        glVertex(+L/2,-L/2, 2*z)
 
        glVertex(+L,+L, z)
        glVertex(+L/2,+L/2, 2*z)
 
        glVertex(-L,+L, z)
        glVertex(-L/2,+L/2, 2*z)
        
        glEnd()
        
        
app = QtGui.QApplication(["My OpenGL"])

window = GLWidget()

window.show()
window.raise_()

app.exec_()
