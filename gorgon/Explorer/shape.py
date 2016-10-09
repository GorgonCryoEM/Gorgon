from PyQt4 import QtGui

from base_viewer import BaseViewer
from gorgon.libs import Vec3
from ..toolkit.libpytoolkit import RendererBase


class Shape(BaseViewer):

    def __init__(self, main):
        super(Shape, self).__init__(main)
        
        self.renderer = RendererBase()
        self.s = RendererBase()

    def setColor(self, r, g, b, a):
        self.color = QtGui.QColor(r, g, b, a)

    def setLoc(self, x, y, z):
        self.loc = Vec3(x, y, z)
