from PyQt4 import QtGui, QtCore, QtOpenGL

from Explorer.volume_viewer import VolumeViewer
from Explorer.skeleton_viewer import SkeletonViewer


class Scene:
    
    visualizationUpdated = QtCore.pyqtSignal()

    def __init__(self, parent):
        
        self.volumeViewer = VolumeViewer(parent)
        self.skeletonViewer = SkeletonViewer(parent)
        
        self.shapes = [self.volumeViewer, self.skeletonViewer, self.sphere]
        
    def add(self, shape):
        self.shapes.append(shape)

    def getShapes(self):
        return self.shapes

    def load(self):
        self.volumeViewer.load("densityMap.mrc")
#         self.volumeViewer1.load("vp6-96o.mrc")
        self.skeletonViewer.load("skeleton-vp6-b0.4.mrc")
