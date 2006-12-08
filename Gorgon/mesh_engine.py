import sys
import math
from gorgon import MarchingCubes
from PyQt4 import QtCore, QtGui, QtOpenGL

try:
    from OpenGL.GL import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber",
                            "PyOpenGL must be installed to run this example.",
                            QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default,
                            QtGui.QMessageBox.NoButton)
    sys.exit(1)

class Isosurface(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        #self.engine = MarchingCubes()
        self.thread = RenderThread()
        self.connect(self.thread, QtCore.SIGNAL("meshCreated()"), self.meshChanged)
        self.gllist = 0

    def drawMesh(self, xRot, yRot, zRot, scale):
        if self.gllist != 0:
            glPushMatrix()
            glTranslated(0.0, 0.0, -1.0)
            glRotated(xRot, 1.0, 0.0, 0.0)
            glRotated(yRot, 0.0, 1.0, 0.0)
            glRotated(zRot, 0.0, 0.0, 1.0)
            #glScale(scale, scale, scale)
            glPushAttrib(GL_LIGHTING_BIT)
            glDisable(GL_LIGHTING)
            glColor3f(1.0, 1.0, 1.0)
            
            glutWireCube(1.0)
            glPopAttrib()
            
            glPushMatrix()
            glTranslated(-0.5, -0.5, -0.5)
            #self.engine.drawMesh(True)
            glCallList(self.gllist)
            glPopMatrix()
            glPopMatrix()

    def load(self, filename):
        #self.engine.loadMRC(str(filename))
        self.thread.loadMRC(filename)
        #self.meshChanged()
        
    def setSurfaceValue(self, value):
        #self.engine.setSurfaceValue(value)
        #self.meshChanged()
        self.thread.setSurfaceValue(value)

    def setSampleDensity(self, value):
        #self.engine.setSampleDensity(value)
        #self.meshChanged()
        #self.thread.setSampleDensity(value)
        pass

    def meshChanged(self):
        #print "Mesh Created..."
        if self.gllist != 0:
            glDeleteLists(self.gllist,1)
            
        self.gllist = glGenLists(1)
        glNewList(self.gllist, GL_COMPILE)
        self.thread.drawMesh()
        glEndList()
        
        self.emit(QtCore.SIGNAL("meshChanged()"))

class RenderThread(QtCore.QThread):

    def __init__(self, parent=None):
        QtCore.QThread.__init__(self, parent)

        self.mutex = QtCore.QMutex()
        self.condition = QtCore.QWaitCondition()
        self.engine = MarchingCubes()
        self.surface = 0
        self.sample = 0
        self.surfaceChanged = False
        self.isLoading = False
        self.restart = False
        self.abort = False

    def __del__(self):
        self.mutex.lock()
        self.abort = True
        self.condition.wakeOne()
        self.mutex.unlock()

        self.wait()

    def loadMRC(self, filename):
        locker = QtCore.QMutexLocker(self.mutex)
        self.filename = filename
        self.isLoading = True

        if not self.isRunning():
            self.start(QtCore.QThread.LowPriority)
        else:
            self.restart = True
            self.condition.wakeOne()


    def setSurfaceValue(self, value):
        locker = QtCore.QMutexLocker(self.mutex)

        self.surface = value
        self.surfaceChanged = True
        
        if not self.isRunning():
            self.start(QtCore.QThread.LowPriority)
        else:
            self.restart = True
            self.condition.wakeOne()

    def drawMesh(self):
        locker = QtCore.QMutexLocker(self.mutex)
        self.engine.drawMesh(True)

    def run(self):
        while True:
            #print "Running..."
            self.mutex.lock()
            surface = self.surface
            engine = self.engine
            isLoading = self.isLoading
            self.mutex.unlock()

            #print "Unlocking..."
            if self.isLoading:
                #print "Loading..."
                # load filename
                engine.loadMRC(str(self.filename))
                self.isLoading = False
                self.emit(QtCore.SIGNAL("meshCreated()"))
            elif self.surfaceChanged:
                #print "Surface Changed..."
                level = 1
                engine.setSampleDensity(level)
                engine.setSurfaceValue(surface)
                level = 5
                engine.setSampleDensity(level)
                #print "Finished Changing Surface..."
                self.mutex.lock()
                self.engine = engine
                self.mutex.unlock()
                
                self.emit(QtCore.SIGNAL("meshCreated()"))
                
                while level <= 7 and not self.restart:
                    # refine structure
                    engine.setSampleDensity(level)
                    self.mutex.lock()
                    self.engine = engine
                    self.mutex.unlock()
                    self.emit(QtCore.SIGNAL("meshCreated()"))
                    self.msleep(level*90)
                    level = level + 1
                #print "Restart: " + str(self.restart) + ", Level: " + str(level)
                if not self.restart:
                    self.surfaceChanged = False
            
            self.mutex.lock()
            self.engine = engine
            if not self.restart:
                self.condition.wait(self.mutex)
            self.restart = False
            self.mutex.unlock()
  
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
