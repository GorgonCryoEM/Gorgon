from mesh_engine import MeshEngine

class ObjLoaderEngine(MeshEngine):
    def __init__(self):
        MeshEngine.__init__(self)
        self.vertices = []
        self.normals = []
        self.texcoords = []
        self.faces = []
        self.mtl = {}

    def produceMesh(self, args, mpl):
        filename = args[0]
        swapyz = False
        vEncountered = False
        vrange = [[0,0],[0,0],[0,0]]
        material = None
        for line in open(filename, "r"):
            if line.startswith('#'): continue
            values = line.split()
            if not values: continue
            if values[0] == 'v':
                v = map(float, values[1:4])
                if swapyz:
                    v = v[0], v[2], v[1]
                    
                if vEncountered:
                    for i,r in enumerate(vrange):
                        r[0] = min(r[0],v[i])
                        r[1] = max(r[1],v[i])
                else:
                    for i,r in enumerate(vrange):
                        r = [v[i], v[i]]
                        vEncountered = True
                        
                self.vertices.append(v)
            elif values[0] == 'vn':
                v = map(float, values[1:4])
                if swapyz:
                    v = v[0], v[2], v[1]
                self.normals.append(v)
            elif values[0] == 'vt':
                self.texcoords.append(map(float, values[1:3]))
            elif values[0] in ('usemtl', 'usemat'):
                material = values[1]
            elif values[0] == 'mtllib':
                self.mtl = MTL(values[1])
            elif values[0] == 'f':
                face = []
                texcoords = []
                norms = []
                for v in values[1:]:
                    w = v.split('/')
                    face.append(int(w[0]))
                    if len(w) >= 2 and len(w[1]) > 0:
                        texcoords.append(int(w[1]))
                    else:
                        texcoords.append(0)
                    if len(w) >= 3 and len(w[2]) > 0:
                        norms.append(int(w[2]))
                    else:
                        norms.append(0)
                self.faces.append((face, norms, texcoords, material))
 
        self.mesh.mesh = glGenLists(1)
        glNewList(self.mesh.mesh, GL_COMPILE)
        glEnable(GL_TEXTURE_2D)
        #reflectance = (0.8, 1.0, 1.0, 1.0)
        #glMaterialfv(GL_FRONT, GL_SHININESS, 120)
        #glColor(1,1,1)
        glFrontFace(GL_CCW)
        for face in self.faces:
            vertices, normals, texture_coords, material = face
            
            #mtl = self.mtl[material]
            #if 'texture_Kd' in mtl:
                # use diffuse texmap
            #    glBindTexture(GL_TEXTURE_2D, mtl['texture_Kd'])
            #else:
                # just use diffuse colour
            #    glColor(*mtl['Kd'])
 
            glBegin(GL_POLYGON)
            for i in range(0, len(vertices)):
                if normals[i] > 0:
                    glNormal3fv(self.normals[normals[i] - 1])
                if texture_coords[i] > 0:
                    glTexCoord2fv(self.texcoords[texture_coords[i] - 1])
                glVertex3fv(self.vertices[vertices[i] - 1])
            glEnd()
        glDisable(GL_TEXTURE_2D)
        glEndList()

        self.xRange = vrange[0]
        self.yRange = vrange[1]
        self.zRange = vrange[2]
      
        mpl.meshProduced()
