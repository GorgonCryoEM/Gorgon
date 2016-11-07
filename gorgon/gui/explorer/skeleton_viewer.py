from PyQt4 import QtGui, QtCore, QtOpenGL
from ...toolkit.libpytoolkit import SkeletonRenderer
from base_viewer import BaseViewer


from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class SkeletonViewer(BaseViewer):

    def __init__(self, main, parent=None):
        self.title = "Skeleton"
        super(SkeletonViewer, self).__init__(SkeletonRenderer(), main)
        self.shortTitle = "SKE"
        
        self.color = QtGui.QColor(200, 50, 50, 150)

        
        self.isClosedMesh = False
        self.twoWayLighting = True
        self.lineThickness = 3
        self.renderer.setLineThickness(self.lineThickness)
        
        # self.ui.pushButtonSave.clicked.connect(self.saveData)
        
    def saveData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Data"), "")
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)

    def loadVolume(self, volume):
#         if(self.loaded):
#             self.unloadData
        print "SkeletonViewer.loadVolume()"
        print volume
        
        self.renderer.loadVolume(volume)
        print self.renderer.getMesh().getSize()
        self.setScale(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())
        self.loaded = True
        self.modelLoadedPreDraw()
        self.modelChanged()
        self.centerAllRequested.emit()
