from PyQt4 import QtOpenGL, QtCore
from vector_lib import *

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)
    
class Camera(QtOpenGL.QGLWidget):
    def __init__(self, scene, parent=None):
        self.aspectRatio = 1.0;        
        self.setEyeZoom(0.25)
        QtOpenGL.QGLWidget.__init__(self, parent)

        self.scene = scene
        
        self.setCenter(0, 0, 0)
        self.setEye(0, -4, 0) 
        self.setUp(0, 0, 1)     
        self.setEyeRotation(0, 0, 0)

        self.lastPos = QtCore.QPoint()
        
        for s in self.scene:
            self.connect(s, QtCore.SIGNAL("viewerSetCenter(float, float, float, float, float, float)"), self.sceneSetCenter)
            self.connect(s, QtCore.SIGNAL("modelChanged()"), self.updateGL)
            self.connect(s, QtCore.SIGNAL("modelLoaded()"), self.updateGL)
            self.connect(s, QtCore.SIGNAL("modelUnloaded()"), self.updateGL)
            self.connect(s, QtCore.SIGNAL("modelVisualizationChanged()"), self.updateGL)

    
    def setEye(self, x, y, z):
        self.eye = [x, y, z]
        #print("Eye :", self.eye)
        try:
            self.look = vectorNormalize([self.center[0] - self.eye[0], self.center[1] - self.eye[1], self.center[2] - self.eye[2]])
            #print("Eye: look :", self.look)
            self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))
            #print("Eye: right :", self.right)        
        except:
            self.look = [0,1,0]
            self.right = [-1,0,0]
    
    def setCenter(self, x, y, z):
        self.center = [x, y, z]
        try:
            self.look = vectorNormalize([self.center[0] - self.eye[0], self.center[1] - self.eye[1], self.center[2] - self.eye[2]])
            #print("Center: look :", self.look)
            self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))
            #print("Center: right :", self.right)
        except:
            self.look = [0,1,0]
            self.right = [-1,0,0]
        
    def setUp(self, x, y, z):
        self.up = vectorNormalize([x, y, z])
        #print("Up: up :", self.up)
        try:
            self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))
            #print("Up: right :", self.right)
            self.up = vectorNormalize(vectorCrossProduct(self.look, self.right))
            #print("Up: up :", self.up)
        except:
            self.right = [-1,0,0]
        
    def setEyeRotation(self, yaw, pitch, roll):
        self.eyeRotation = [yaw, pitch, roll]
    
    def setEyeZoom(self, value):
        self.eyeZoom = min(max(value, 0.0001), 0.9999);
        self.setGlProjection()
    
    def sceneSetCenter(self, minX, minY, minZ, maxX, maxY, maxZ):
        self.setEye(maxX+(maxX-minX), maxY+(maxY-minY), maxZ+(maxZ-minZ))
        self.setCenter((minX+maxX)/2.0, (minY+maxY)/2.0, (minZ+maxZ)/2.0)
        self.setUp(0, 0, 1)
        self.setGluLookAt()
        self.initializeSceneMatrix()      
        self.updateGL()
    
    def initializeSceneMatrix(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()        
        glPushMatrix()
        self.sceneMatrix = glGetFloatv( GL_MODELVIEW_MATRIX )
        glPopMatrix()  
    
    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)     
       
    def initializeGL(self):
        afPropertiesAmbient = [0.50, 0.50, 0.50, 1.00] 
        afPropertiesDiffuse = [0.75, 0.75, 0.75, 1.00] 
        afPropertiesSpecular = [0.0, 0.0, 0.0, 1.00]
        afSpecularWhite =  [1.0, 1.0, 1.0, 1.00]
        afDiffuseBlue = [0.00, 0.00, 0.75, 1.00]
        afAmbientBlue = [0.00, 0.00, 0.25, 1.00]
        afDiffuseGray = [0.75, 0.75, 0.75, 1.00]
        afAmbientGray = [0.25, 0.25, 0.25, 1.00]
        afAmbientGreen = [0.00, 0.25, 0.00, 1.00]
        afDiffuseGreen = [0.00, 0.75, 0.00, 1.00]
        
        #glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE )
        glClearColor( 0.0, 0.0, 0.0, 1.0 )
        glClearDepth( 5.0 )
        
        afLightPosition = [1000,1000,1000]
        
        glLightfv( GL_LIGHT0, GL_AMBIENT,  afPropertiesAmbient);
        glLightfv( GL_LIGHT0, GL_DIFFUSE,  afPropertiesDiffuse) 
        glLightfv( GL_LIGHT0, GL_SPECULAR, afPropertiesSpecular) 
        #glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0)
        glLightfv( GL_LIGHT0, GL_POSITION, afLightPosition)

        glEnable( GL_LIGHT0 ) 


        afLightPosition = [-1000,-1000,-1000]        
        glLightfv( GL_LIGHT1, GL_AMBIENT,  afPropertiesAmbient);
        glLightfv( GL_LIGHT1, GL_DIFFUSE,  afPropertiesDiffuse) 
        glLightfv( GL_LIGHT1, GL_SPECULAR, afPropertiesSpecular) 
        #glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0)
        glLightfv( GL_LIGHT1, GL_POSITION, afLightPosition)

        glEnable( GL_LIGHT1 ) 
               

        glMaterialfv(GL_BACK, GL_AMBIENT,   afAmbientGray) 
        glMaterialfv(GL_BACK, GL_DIFFUSE,   afDiffuseGray) 
        glMaterialfv(GL_BACK, GL_SPECULAR,  afSpecularWhite) 
        glMaterialf( GL_BACK, GL_SHININESS, 0.1)
        glMaterialfv(GL_FRONT, GL_AMBIENT,   afAmbientGray) 
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   afDiffuseGray) 
        glMaterialfv(GL_FRONT, GL_SPECULAR,  afSpecularWhite) 
        glMaterialf( GL_FRONT, GL_SHININESS, 0.1)
        #glMaterialfv(GL_FRONT, GL_EMISSION, afSpecularWhite)

        #fColor = [0.5, 0.5, 0.5, 1.0]
        #glEnable(GL_FOG)
        #fogMode = GL_EXP
        #glFogi(GL_FOG_MODE, fogMode)
        #glFogfv(GL_FOG_COLOR, fColor)
        #glFogf(GL_FOG_DENSITY, 0.35)
        #glHint(GL_FOG_HINT, GL_DONT_CARE)
        #glFogf(GL_FOG_START, 1.0)
        #glFogf(GL_FOG_END, -1.0)
        #glClearColor(0.5, 0.5, 0.5, 1.0)
        
        #self.engine.produceMesh(["venusm.obj"],self)
        #glDisable(GL_CULL_FACE)

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        self.sceneMatrix = glGetFloatv( GL_MODELVIEW_MATRIX )
              
    def setGluLookAt(self):
        gluLookAt(self.eye[0], self.eye[1], self.eye[2], 
                  self.center[0], self.center[1], self.center[2], 
                  self.up[0], self.up[1], self.up[2])
        
    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        
        glPushMatrix()
        glTranslated(self.center[0], self.center[1], self.center[2])
        glRotatef(self.eyeRotation[0], self.up[0], self.up[1], self.up[2])
        glRotated(self.eyeRotation[1], self.right[0], self.right[1], self.right[2])       
        glRotated(self.eyeRotation[2], self.look[0], self.look[1], self.look[2])
        glTranslated(-self.center[0], -self.center[1], -self.center[2])      
        glMultMatrixf(self.sceneMatrix)
        self.sceneMatrix = glGetFloatv( GL_MODELVIEW_MATRIX )
        glPopMatrix()
        
        self.setEyeRotation(0, 0, 0)
        self.drawScene()      
               
    def drawScene(self):
        glPushMatrix()
        
        self.setGluLookAt()
                
        glMultMatrixf(self.sceneMatrix)
            
        glPushName(0)
        for s in self.scene:
            s.draw()
        glPopName()
        glPopMatrix()
       
#    def pickObject(self, x, y):
#        SIZE = 100
#        viewport = list(glGetIntegerv(GL_VIEWPORT))
#
#        glSelectBuffer(SIZE)
#        glRenderMode(GL_SELECT)
#
#        glInitNames()
#
#        glMatrixMode(GL_PROJECTION)
#        glPushMatrix()
#        glLoadIdentity()
#        gluPickMatrix(x, viewport[3]-y, 10, 10, viewport)
#        gluPerspective(45, self.ratio, 0.1, 1000)
#
#        self.drawScene(False)
#        
#        glMatrixMode(GL_PROJECTION)
#        glPopMatrix()
#        glFlush()
#
#        bufferStack = glRenderMode(GL_RENDER)
#        for hit_record in bufferStack:
#            min_depth, max_depth, names = hit_record
#            namelist = list(names)
#            for n in namelist:
#                print n
#            print min_depth, max_depth, names 
        
    def resizeGL(self, width, height):
        self.aspectRatio = width/(1.0*height)
        glViewport(0,0, width, height)
        self.setGlProjection()
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def setGlProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, 0.1, 1000)
        
    def mousePressEvent(self, event):
        self.lastPos = QtCore.QPoint(event.pos())
        
        #self.pickObject(event.x(), event.y())

    def mouseMoveEvent(self, event):
        dx = event.x() - self.lastPos.x()
        dy = event.y() - self.lastPos.y()

        if event.buttons() & QtCore.Qt.LeftButton:
            self.setEyeRotation(dx, -dy, 0)
        elif event.buttons() & QtCore.Qt.RightButton:
            self.setEyeRotation(0, 0, dx)
        
        self.lastPos = QtCore.QPoint(event.pos())

        self.updateGL()
    
    def wheelEvent(self, event):
        direction = event.delta()/abs(event.delta())
        self.setEyeZoom(self.eyeZoom + direction * 10.0/360.0)
        #distance = vectorDistance(self.eye, self.center) * direction * 0.1
        #self.setEye(self.eye[0] + self.look[0]*distance, self.eye[1] + self.look[1]*distance, self.eye[2] + self.look[2]*distance)
        self.updateGL()                          
