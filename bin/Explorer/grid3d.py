from PyQt4 import QtGui, QtCore, QtOpenGL
from . import BaseViewer
from ..toolkit.libpytoolkit import RendererBase
from .libs import Vec3
from ..toolkit.libpytoolkit import drawLine

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Grid3D(BaseViewer):

    _col = 0

    def __init__(self, main, p1, p2):
        self.title = "Grid3D"
        super(Grid3D, self).__init__(main)
        
        self.p1 = Vec3(p1)
        self.p2 = Vec3(p2)
        
        self.renderer = RendererBase()
        
        self.loaded           = True
        self.setColor(QtGui.QColor(self._col*20, 20, 50, 150))
        Grid3D._col += 1
#         self._col.setBlue(self._col.blue() + 20)
        print "self._col= ", self._col

    def drawGL(self):
        print "Grid3D.drawGL"
        print self.getColor().red(), self.getColor().green(), self.getColor().blue()
        glLineWidth(3)
        drawLine(self.p1, self.p2)
