from PyQt4 import QtGui

from base_viewer import BaseViewer
from gorgon.libs import Vec3
from ..toolkit.libpytoolkit import RendererBase
from ..toolkit.libpytoolkit import drawSphere, drawLine, drawCylinder


# from volume_surface_editor_form import VolumeSurfaceEditorForm
# from model_visualization_form import ModelVisualizationForm
# from string import split, upper


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
