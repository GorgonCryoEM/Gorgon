from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from libpytoolkit import RendererBase
from libpytoolkit import Vec3F
from .libs import Vec3

# from volume_surface_editor_form import VolumeSurfaceEditorForm
# from model_visualization_form import ModelVisualizationForm
# from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Sphere(BaseViewer):
    
    def __init__(self, main, parent=None):
        super(Sphere, self).__init__(main, parent)
        
        self.renderer = Display()
        self.s = RendererBase()
        
    def draw(self):
        self.s.drawSphere(Vec3(30., 10., 10.), 10.)
