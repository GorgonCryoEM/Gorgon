class MeshEngine:
    """All mesh engines must extend this class. TODO: Add threading."""
    
    def __init__(self):
        self.mesh = GLMesh()
        self.xRange = [0,0]
        self.yRange = [0,0]
        self.zRange = [0,0]

    def getMesh(self):
        return self.mesh
        
    def drawMesh(self, xRot, yRot, zRot, scale):
        glPushMatrix()
        x = -self.xRange[0]-(self.xRange[1]-self.xRange[0])/2
        y = -self.yRange[0]-(self.yRange[1]-self.yRange[0])/2
        z = -self.zRange[0]-(self.zRange[1]-self.zRange[0])/2
        objlen = min(self.xRange[1]-self.xRange[0], self.yRange[1]-self.yRange[0], self.zRange[1]-self.zRange[0])
        objscale = 2.0/objlen
        glRotated(xRot, 1.0, 0.0, 0.0)
        glRotated(yRot, 0.0, 1.0, 0.0)
        glRotated(zRot, 0.0, 0.0, 1.0)
        glScale(scale, scale, scale)
        glScale(objscale,objscale,objscale)
        glTranslated(x,y,z)
        
        self.mesh.draw()
        glPopMatrix()


