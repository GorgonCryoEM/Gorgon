from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

class GLWidget(QtOpenGL.QGLWidget):
    def __init__(self):
        super(GLWidget, self).__init__()
        
        self.setMinimumSize(300, 300)
        self.setWindowTitle("Yay")
    
    def initializeGL(self):
        glEnable(GL_DEPTH_TEST)
    
    def paintGL(self):    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glClearColor(0,0,0,1)
        
        
    def drawCenter(self):
        glPointSize(10)
        glBegin(GL_POINTS)
        glColor(.5,.5,.5,1)
        glVertex(0,0,0,1)
        glEnd()
        
        glRotate(5,1,1,0)
        self.drawAxes(.7)
        
    def drawAxes(self, L=1):
        glBegin(GL_LINES)
        
        glColor(1,0,0,1)
        glVertex(0,0,0)
        glVertex(L,0,0)
        
        glColor(0,1,0,1)
        glVertex(0,0,0)
        glVertex(0,L,0)
        
        glColor(0,0,1,1)
        glVertex(0,0,0)
        glVertex(0,0,-L)
        
        glEnd()
        
    def drawCube(self, L=0.5):
        z = -.3
        
        glBegin(GL_TRIANGLES)
        
        glColor(.5,0,0,1)
        
        glVertex(-L,-L, z*2)
        glVertex(+L,+L, z*2)
        glVertex(+L,-L, 1)
        
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
        
    def resizeGL(self, w, h):
        glViewport(0,0,w,h)
        
app = QtGui.QApplication(["My OpenGL"])

window = GLWidget()

window.show()
window.raise_()

app.exec_()
