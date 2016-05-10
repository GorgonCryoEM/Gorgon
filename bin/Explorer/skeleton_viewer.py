from PyQt4 import QtGui, QtCore, QtOpenGL
from libpytoolkit import SkeletonRenderer
from base_viewer import BaseViewer


from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class SkeletonViewer(BaseViewer):

    def __init__(self, main, parent=None):
        self.title = "Skeleton"
        super(SkeletonViewer, self).__init__(main, parent)
        self.shortTitle = "SKE"
        
        self.color = QtGui.QColor(200, 50, 50, 150)

        self.renderer = SkeletonRenderer()
        
        self.isClosedMesh = False
        self.twoWayLighting = True
        self.lineThickness = 3
        self.renderer.setLineThickness(self.lineThickness)

    def loadVolume(self, volume):
#         if(self.loaded):
#             self.unloadData
        print "SkeletonViewer.loadVolume()"
        print volume.getSize()
        
        self.renderer.loadVolume(volume)
        print self.renderer.getMesh().getSize()
        self.setScale(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())
        self.loaded = True
        self.modelLoadedPreDraw()
        self.modelChanged()
        self.centerAllRequested.emit()
