from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import VolumeRenderer
from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class VolumeViewer(BaseViewer):

    def __init__(self, main, parent=None):
        self.title = "Volume"
        super(VolumeViewer, self).__init__(VolumeRenderer(), main, parent)
        self.shortTitle = "VOL"
        
        self.loaded = False
        
        self.setColor(QtGui.QColor(50, 200, 50, 150))

    def modelLoadedPreDraw(self):
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2

        maxRadius = int(max(self.renderer.getMaxPos(0), self.renderer.getMaxPos(1), self.renderer.getMaxPos(2)));
        self.renderer.setSampleInterval(1)
        self.renderer.setSurfaceValue(defaultDensity)
        self.renderer.setDisplayRadius(maxRadius)
