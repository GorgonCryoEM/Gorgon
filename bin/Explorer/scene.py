from .libs import Vec3
from line import Line
from line0 import Line0
from dot import Dot


class Scene:

    def __init__(self):
        self.shapes = []
        
        self.linecom = Line0(Vec3(100,100,100))
        self.linecom.setColor(40, 70, 50, 150)
        self.lineaxis = Line0(Vec3(100,100,100))
        self.lineaxis.setColor(80, 40, 50, 150)
        self.lineaxis.depthEnabled = True
        self.dotcenter = Dot()
        self.dotcom = Dot()
        self.dotcom.setColor(0, 130, 0, 150)
        self.shapes.append(self.lineaxis)
        self.shapes.append(self.dotcenter)
        self.shapes.append(self.dotcom)

    def add(self, shape):
        self.shapes.append(shape)

    def getShapes(self):
        return self.shapes
