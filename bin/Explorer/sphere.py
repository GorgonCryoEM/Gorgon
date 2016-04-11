from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import Display
from libpytoolkit import drawSphere
from libpytoolkit import Vec3F
from .libs import Vec3

# from volume_surface_editor_form import VolumeSurfaceEditorForm
from model_visualization_form import ModelVisualizationForm
# from string import split, upper

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class Sphere(BaseViewer):
    
    def __init__(self, main, parent=None):
        self.title = "Sphere"
        super(Sphere, self).__init__(main, parent)
        
        self.center = Vec3(30., 10., 10.)
        self.R      = 10.
        
        self.renderer = Display()
        self.color = QtGui.QColor(120, 18, 80, 150)
        
        self.selectEnabled    = True
        self.mouseMoveEnabled = True
        self.loaded           = True
        self.modelVisible     = True
        
    def drawGL(self):
        drawSphere(self.center, self.R)

    def draw(self):
        if(self.modelVisible):
            self.display_styles[self.bg.checkedId()]()
            self.setMaterials()
            drawSphere(self.center, self.R)
