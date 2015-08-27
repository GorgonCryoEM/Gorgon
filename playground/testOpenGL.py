from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

app = QtGui.QApplication(["My OpenGL"])

window = QtOpenGL.QGLWidget()

window.setMinimumSize(300, 300)
window.setWindowTitle("Yay")

glClear(GL_COLOR_BUFFER_BIT)
glClearColor(0,0,0,1)

glBegin(GL_LINE_STRIP)
glVertex(-1,-1,0)
glVertex(+1,+1,0)
glEnd()

window.show()
window.raise_()

app.exec_()
