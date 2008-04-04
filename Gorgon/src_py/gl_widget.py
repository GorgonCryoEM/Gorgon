from PyQt4 import QtOpenGL, QtCore

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)
    
class GLWidget(QtOpenGL.QGLWidget):
    def __init__(self, scene, glOptions, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)

        self.scene = scene
        glOptions.addGLWindow(self)
        
        self.xRot = 0
        self.yRot = 0
        self.zRot = 0
        self.scale = 1000

        self.z = 4
        self.x = 0
        self.y = 0

        self.showBox = True
        self.lastPos = QtCore.QPoint()
        
    def xRotation(self):
        return self.xRot

    def yRotation(self):
        return self.yRot

    def zRotation(self):
        return self.zRot

    def scale(self):
        return self.scale
    
    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)

    def setXRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.xRot:
            self.xRot = angle
            #self.emit(QtCore.SIGNAL("xRotationChanged(int)"), angle)
            #self.updateGL()

    def setYRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.yRot:
            self.yRot = angle
            #self.emit(QtCore.SIGNAL("yRotationChanged(int)"), angle)
            #self.updateGL()

    def setZRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.zRot:
            self.zRot = angle
            #self.emit(QtCore.SIGNAL("zRotationChanged(int)"), angle)
            #self.updateGL()

    def setX(self, num):
        if num != self.x:
            self.x = num
            #self.emit(QtCore.SIGNAL("zTranslationChanged(double)"), num)
            #self.updateGL()

    def setY(self, num):
        if num != self.y:
            self.y = num
            #self.emit(QtCore.SIGNAL("zTranslationChanged(double)"), num)
            #self.updateGL()

    def setZ(self, num):
        if num != self.z and num >= 0:
            self.z = num
            #self.emit(QtCore.SIGNAL("zTranslationChanged(double)"), num)
            #self.updateGL()
            
    def setScale(self, size):
        if size != self.scale and size >= 0:
            self.scale = size
            #self.emit(QtCore.SIGNAL("scaleChanged(int)"), size)
            #self.updateGL()

    def setShowBox(self, show):
        self.showBox = show
        
    def initializeGL(self):
        afPropertiesAmbient = [0.50, 0.50, 0.50, 1.00] 
        afPropertiesDiffuse = [0.75, 0.75, 0.75, 1.00] 
        afPropertiesSpecular = [1.00, 1.00, 1.00, 1.00]
        afSpecularWhite = [1.00, 1.00, 1.00, 1.00]
        afDiffuseBlue = [0.00, 0.00, 0.75, 1.00]
        afAmbientBlue = [0.00, 0.00, 0.25, 1.00]
        afDiffuseGray = [0.75, 0.75, 0.75, 1.00]
        afAmbientGray = [0.25, 0.25, 0.25, 1.00]
        afAmbientGreen = [0.00, 0.25, 0.00, 1.00]
        afDiffuseGreen = [0.00, 0.75, 0.00, 1.00]
        
        #glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE )
        glClearColor( 0.0, 0.0, 0.0, 1.0 )
        glClearDepth( 5.0 )

        self.setDisplayType(2)
        
        glLightfv( GL_LIGHT0, GL_AMBIENT,  afPropertiesAmbient);
        glLightfv( GL_LIGHT0, GL_DIFFUSE,  afPropertiesDiffuse) 
        glLightfv( GL_LIGHT0, GL_SPECULAR, afPropertiesSpecular) 
        #glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0)

        glEnable( GL_LIGHT0 ) 

        glMaterialfv(GL_BACK,  GL_AMBIENT,   afAmbientGreen)
        glMaterialfv(GL_BACK,  GL_DIFFUSE,   afDiffuseGreen) 
        glMaterialfv(GL_FRONT, GL_AMBIENT,   afAmbientGray) 
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   afDiffuseGray) 
        glMaterialfv(GL_FRONT, GL_SPECULAR,  afSpecularWhite) 
        glMaterialf( GL_FRONT, GL_SHININESS, 25.0)

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

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        self.sceneMatrix = glGetFloatv( GL_MODELVIEW_MATRIX )
        
    def setDisplayType(self, numType):
        self.drawType = numType
        if numType == 0:
            # Wireframe
            glEnable(GL_DEPTH_TEST)
            glEnable(GL_LIGHTING)
            glPolygonMode(GL_FRONT, GL_LINE)
            
        elif numType == 1:
            # Flat Shade
            glEnable(GL_DEPTH_TEST) 
            glEnable(GL_LIGHTING)
            glPolygonMode(GL_FRONT, GL_FILL)
            glShadeModel(GL_FLAT)
            
        elif numType == 2:
            glEnable(GL_DEPTH_TEST) 
            glEnable(GL_LIGHTING)
            glPolygonMode(GL_FRONT, GL_FILL)
            glShadeModel(GL_SMOOTH)
            
        else:
            self.setDisplayType(2)
            
    def setGluLookAt(self):
        gluLookAt(self.x,self.y,self.z, self.x, self.y, 0, 0,1,0)
        
    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        
        glPushMatrix()
        glRotated(self.xRot/16, 0, 1, 0)
        glRotated(self.yRot/16, 1, 0, 0)
        glMultMatrixf(self.sceneMatrix)
        self.sceneMatrix = glGetFloatv( GL_MODELVIEW_MATRIX )
        self.xRot = 0
        self.yRot = 0
        glPopMatrix()
        
        self.drawScene(self.showBox)      

    def drawScene(self, draw_box):
        glPushMatrix()
        self.setGluLookAt()
        glMultMatrixf(self.sceneMatrix)

        if(draw_box):
            glPushAttrib(GL_LIGHTING_BIT)
            glDisable(GL_LIGHTING)
            glColor3f(1.0, 1.0, 1.0)
            glutWireCube(1.0)
            glPopAttrib()
            
        glPushName(0)
        for s in self.scene:
            s.draw()
        #self.scene.draw()
        glPopName()
        glPopMatrix()
       
    def pickObject(self, x, y):
        SIZE = 100
        viewport = list(glGetIntegerv(GL_VIEWPORT))

        glSelectBuffer(SIZE)
        glRenderMode(GL_SELECT)

        glInitNames()

        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        gluPickMatrix(x, viewport[3]-y, 10, 10, viewport)
        gluPerspective(45, self.ratio, 0.1, 1000)

        self.drawScene(False)
        
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glFlush()

        bufferStack = glRenderMode(GL_RENDER)
        for hit_record in bufferStack:
            min_depth, max_depth, names = hit_record
            namelist = list(names)
            for n in namelist:
                print n
            print min_depth, max_depth, names
        
    def resizeGL(self, width, height):
        self.ratio = width/(1.0*height)
        glViewport(0,0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45, self.ratio, 0.1, 1000)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def mousePressEvent(self, event):
        self.lastPos = QtCore.QPoint(event.pos())
        
        #self.pickObject(event.x(), event.y())

    def mouseMoveEvent(self, event):
        dx = event.x() - self.lastPos.x()
        dy = event.y() - self.lastPos.y()

        if event.buttons() & QtCore.Qt.LeftButton:
            self.setXRotation(8 * dx)
            self.setYRotation(8 * dy)
            #self.setX(self.x + dx/16.0)
            #self.setY(self.y + dy/16.0)
            
        elif event.buttons() & QtCore.Qt.RightButton:
            #self.setScale(self.scale + 8 * dx)
            self.setZ(self.z + dx/8.0)
            
            #self.setXRotation(self.xRot + 8 * dy)
            #self.setZRotation(self.zRot + 8 * dx)

        self.lastPos = QtCore.QPoint(event.pos())

        self.updateGL()
        
    def normalizeAngle(self, angle):
        return angle
        
