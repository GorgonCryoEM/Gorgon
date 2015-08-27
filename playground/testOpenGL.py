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
        glRotate(10,1,2,0)
    
    def paintGL(self):    
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glClearColor(0,0,0,1)
        
        glRotate(5,1,2,0)

        self.drawRef()
        
    def drawRef(self):
        self.drawCenter()
#         glTranslate(0.1,0.1,0)
        self.drawAxes(1)
        
    def drawCenter(self):
        glPointSize(10)
        glBegin(GL_POINTS)
        glColor(.5,.5,.5,1)
        glVertex(0,0,0,1)
        glEnd()
        
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
        z = L
        
        glBegin(GL_TRIANGLE_FAN)
        
        glColor(0, 1, 1, 1)
        
        glVertex(-L,-L, z)
        glVertex(+L,-L, z)
        glVertex(+L,+L, z)
        glVertex(-L,+L, z)
        
        glEnd()
        
        glBegin(GL_TRIANGLE_FAN)
        glColor(0, 0.5, 0, 1)
 
        glVertex(-L,-L, -z)
        glVertex(+L,-L, -z)
        glVertex(+L,+L, -z)
        glVertex(-L,+L, -z)
         
        glEnd()
        
        glBegin(GL_LINES)
        glColor(1, 0, 0, 1)
        
        glVertex(-L,-L, z)
        glVertex(-L,-L, -z)

        glVertex(+L,-L, z)
        glVertex(+L,-L, -z)
        
        glVertex(+L,+L, z)
        glVertex(+L,+L, -z)
        
        glVertex(-L,+L, z)
        glVertex(-L,+L, -z)
        
        glEnd()

    def drawWireCube(self, L=0.5):
        z = L
        glBegin(GL_LINE_LOOP)
        
        glColor(0.3, .4, 0.4, 1)
        
        glVertex(-L,-L, z)
        glVertex(+L,-L, z)
        glVertex(+L,+L, z)
        glVertex(-L,+L, z)
        
        glEnd()
        
        glBegin(GL_LINE_LOOP)
#         glColor(0, 0.5, 0, 1)
 
        glVertex(-L,-L, -z)
        glVertex(+L,-L, -z)
        glVertex(+L,+L, -z)
        glVertex(-L,+L, -z)
         
        glEnd()
        
        glBegin(GL_LINES)
#         glColor(1, 0, 0, 1)
        
        glVertex(-L,-L, z)
        glVertex(-L,-L, -z)

        glVertex(+L,-L, z)
        glVertex(+L,-L, -z)
        
        glVertex(+L,+L, z)
        glVertex(+L,+L, -z)
        
        glVertex(-L,+L, z)
        glVertex(-L,+L, -z)
        
        glEnd()
        
    def resizeGL(self, w, h):
        glViewport(0,0,w,h)
        
app = QtGui.QApplication(["My OpenGL"])

window = GLWidget()

window.show()
window.raise_()

app.exec_()
