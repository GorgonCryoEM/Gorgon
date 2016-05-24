from PyQt4 import QtGui, QtCore, QtOpenGL
from base_viewer import BaseViewer
from libpytoolkit import RendererBase
from libpytoolkit import drawSphere, drawLine, drawCylinder
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
        self.title = "Sphere"
        super(Sphere, self).__init__(main, parent)
        
        self.center = Vec3(30., 10., 10.)
        self.R      = 10.
        
        self.renderer = RendererBase()
        
        self.loaded           = True
        self.setColor(QtGui.QColor(120, 18, 80, 150))
        
    def drawGL(self):
        drawCylinder(self.center, self.center + Vec3(20,20,0), 10., 50, 50)
        drawLine(self.center, self.center + Vec3(20,20,0))
        drawSphere(self.center, self.R)

    def draw(self):
        if(self.modelVisible):
            self.display_styles[self.bg.checkedId()]()
            self.setMaterials()
            drawSphere(self.center, self.R)
