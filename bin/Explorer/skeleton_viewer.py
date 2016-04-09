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
        
        self.color = QtGui.QColor(200, 50, 50, 150)

        self.renderer = MeshRenderer()
        
        self.isClosedMesh = False
        self.twoWayLighting = True
        self.lineThickness = 3
        self.renderer.setLineThickness(self.lineThickness)

