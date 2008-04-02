class GLMesh:
    """Holds a mesh object of points, triangles, and normals"""
    def __init__(self, points=[], triangles=[], normals=[]):
        self.mesh = 0
        self.create(points, triangles, normals)

    def __del__(self):
        glDeleteLists(self.mesh, 1)
        
    def create(self, points, triangles, normals):
        """Create GL list to draw"""
        reflectance = (0.8, 0.1, 0.0, 1.0)
        gllist = glGenLists(1)
        glNewList(gllist, GL_COMPILE)
        #glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance)
        #glShadeModel(GL_FLAT)
        #glColor(1,1,0);

        glBegin(GL_TRIANGLES)

        for triangle in triangles:
            for p in triangle:
                glNormal3d(normals[p][0], normals[p][1], normals[p][2])
                glVertex3d(points[p][0], points[p][1], points[p][2])

        glEnd()
        
        glEndList()
        
        self.mesh = gllist
    
    def draw(self):
        glCallList(self.mesh)
