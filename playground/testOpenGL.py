from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

app = QtGui.QApplication(["My OpenGL"])

window = QtGui.QMainWindow()
window.show()
window.raise_()

app.exec_()
