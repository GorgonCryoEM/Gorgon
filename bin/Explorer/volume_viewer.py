from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from volume_surface_editor_form import VolumeSurfaceEditorForm
from model_visualization_form import ModelVisualizationForm
from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class VolumeViewer(BaseViewer):

    def __init__(self, main, parent=None):
        self.title = "Volume"
        super(VolumeViewer, self).__init__(main, parent)
        self.shortTitle = "VOL"
        
        self.color = QtGui.QColor(50, 200, 50, 150)

        self.renderer = Display()
        self.loaded = False
        
        self.connect(self, QtCore.SIGNAL("modelLoadedPreDraw()"), self.modelLoadedPreDraw)

    def modelLoadedPreDraw(self):
        self.renderer.enableDraw(False)
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2

        maxRadius = int(max(self.renderer.getMax(0)/2, self.renderer.getMax(1)/2, self.renderer.getMax(2)/2));
        self.renderer.setSampleInterval(1)
        self.renderer.setSurfaceValue(defaultDensity)
        self.renderer.setDisplayRadius(maxRadius)
        self.renderer.enableDraw(True)
