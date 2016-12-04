from OpenGL.GL import *
from PyQt4 import QtGui

from gorgon.libs import Vec3
from . import BaseViewer
from ...toolkit.libpytoolkit import RendererBase
from ...toolkit.libpytoolkit import drawLine


class Grid3D(BaseViewer):

    _col = 0

    def __init__(self, main, p1, p2):
        self.title = "Grid3D"
        super(Grid3D, self).__init__(RendererBase(), main)
        
        self.p1 = Vec3(p1)
        self.p2 = Vec3(p2)
        
        
        self.loaded           = True
        self.setColor(QtGui.QColor(self._col*20, 20, 50, 150))
        Grid3D._col += 1
#         self._col.setBlue(self._col.blue() + 20)
        print "self._col= ", self._col

    def paintGL(self):
        print "Grid3D.drawGL"
        print self.getColor().red(), self.getColor().green(), self.getColor().blue()
        glLineWidth(3)
        drawLine(self.p1, self.p2)
