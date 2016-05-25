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
        BaseViewer.__init__(self, main, parent)
        self.title = "Volume"
        self.shortTitle = "VOL"

        self.renderer = Display()
        self.loaded = False
        self.surfaceEditor = VolumeSurfaceEditorForm(self.app, self)
        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
                      
    def setCenter(self, center):
        coords = self.worldToObjectCoordinates(center)
        [x, y, z] = [coords.x(), coords.y(), coords.z()]
        self.renderer.setDisplayRadiusOrigin(x, y, z)
        return True
