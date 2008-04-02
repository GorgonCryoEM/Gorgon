from mesh_engine import MeshEngine

class PrimitiveEngine(MeshEngine):
    """MeshEngine that creates primitives."""

    def __init__(self):
        MeshEngine.__init__(self)

    def produceMesh(self, args, mpl):
        """produceMesh takes some sort of arguments and a MeshProductionListener"""
        points = []
        triangles = []
        normals = []
        # Make triangle
        #points = [[0.5,0,0],[0,0,0],[0,0.5,0]]
        #triangles = [[0,1,2]]
        #normals = [[0,0,1],[0,0,1],[0,0,1]]
        
        ####
        # Create cylinder
        N = 20
        R = 0.5
        # Create discs
        for y in [-0.5]:
            points.append([0,y,0])
            normals.append([0,y,0])
            for n in range(0,N-1):
                angle = 360.0*n/N
                points.append([R*math.cos(angle),y,R*math.sin(angle)])
                normals.append([0,y,0])

        self.mesh = GLMesh(points,triangles,normals)
        mpl.meshProduced()
