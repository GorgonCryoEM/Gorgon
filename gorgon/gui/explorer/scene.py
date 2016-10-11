from PyQt4 import QtCore

from skeleton_viewer import SkeletonViewer
from sphere import Sphere
from volume_viewer import VolumeViewer


class Scene:
    
    visualizationUpdated = QtCore.pyqtSignal()

    def __init__(self, parent):
        
        self.volumeViewer = VolumeViewer(parent)
        self.skeletonViewer = SkeletonViewer(parent)
#         self.volumeViewer1 = VolumeViewer(parent)
        self.sphere = Sphere(parent)
        
#         self.shapes = [self.volumeViewer, self.skeletonViewer, self.volumeViewer1, self.sphere]
        self.shapes = [self.volumeViewer, self.skeletonViewer, self.sphere]
        
#         self.linecom = Line0(Vec3(100,100,100))
#         self.linecom.setColor(40, 70, 50, 150)
#         self.lineaxis = Line0(Vec3(100,100,100))
#         self.lineaxis.setColor(80, 40, 50, 150)
#         self.lineaxis.depthEnabled = True
#         self.dotcenter = Dot()
#         self.dotcom = Dot()
#         self.dotcom.setColor(0, 130, 0, 150)
#         self.shapes.append(self.lineaxis)
#         self.shapes.append(self.dotcenter)
#         self.shapes.append(self.dotcom)

    def add(self, shape):
        self.shapes.append(shape)

    def getShapes(self):
        return self.shapes

    def load(self):
        self.volumeViewer.load("densityMap.mrc")
#         self.volumeViewer1.load("vp6-96o.mrc")
        self.skeletonViewer.load("skeleton-vp6-b0.4.mrc")
