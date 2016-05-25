from PyQt4 import QtGui, QtCore, QtOpenGL
from libpytoolkit import MeshRenderer
from base_viewer import BaseViewer
from model_visualization_form import ModelVisualizationForm


from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class SkeletonViewer(BaseViewer):

    def __init__(self, main, parent=None):
        super(SkeletonViewer, self).__init__(main, parent)
        self.title = "Skeleton"
        self.shortTitle = "SKE"

        self.renderer = MeshRenderer()
        
        self.isClosedMesh = False
        self.twoWayLighting = True
        self.lineThickness = 3
        self.renderer.setLineThickness(self.lineThickness)

        self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
        self.visualizationOptions.ui.spinBoxThickness.setValue(self.lineThickness)
        self.visualizationOptions.ui.spinBoxThickness.setVisible(True)
        self.visualizationOptions.ui.labelThickness.setVisible(True)

        self.visualizationOptions.ui.checkBoxModelVisible.setText("Show curves colored:")
