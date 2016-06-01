from PyQt4 import QtOpenGL, QtCore, QtGui

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from libpytoolkit import *
from cmath import *
from .libs import Vec3


class Camera(QtOpenGL.QGLWidget):

    def __init__(self, scene, main, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)
        
        self.scene = scene
        self.app = main

        self.aspectRatio   = 1.0
        self.lightsEnabled = [True, False]
        self.lightsPosition = [Vec3(1000,1000,1000),
							   Vec3(-1000,-1000,-1000)
							   ]
        self.lightsUseEyePosition = [True, False]
        
        self.fogDensity = 0.01
        self.fogEnabled = False
        
        self.center = Vec3(0.0,  0.0, 0.0)
        self.eye    = Vec3(0.0, -4.1, 0.0)
        self.look   = Vec3(0.0, 1.1, 0.0)
        self.right  = Vec3(1.1, 0.0, 0.0)
        self.up     = Vec3(0.0, 0.0, 1.1)
        
        self.near    = 0.1
        self.far     = 1000
        self.eyeZoom = 0.25
        
        self.setEyeRotation(0.0, 0.0, 0.0)
        self.setCenter     (self.center)
        self.setEye        (self.eye)
        self.setUp         (self.up)
        self.setNearFarZoom()
        
        self.selectedScene = -1
        
        self.mouseTrackingEnabled    = True
        self.mouseTrackingEnabledRay = True
        self.mouseMovePoint = QtCore.QPoint(0,0)
        self.mouseDownPoint = QtCore.QPoint(0,0)
        self.mouseLeftPressed  = False
        self.mouseMidPressed   = False
        self.mouseRightPressed = False
        
        self.lastPos = QtCore.QPoint()
        
        for i in range(len(self.scene)):
            self.scene[i].sceneIndex = i;

        for s in self.scene:
            s.visualizationUpdated.connect(self.updateGL)
            s.centerRequested.connect(self.sceneSetCenterLocal)
            s.centerAllRequested.connect(self.sceneSetCenter)
            self.connect(s, QtCore.SIGNAL("mouseTrackingChanged()"), self.refreshMouseTracking)

    def initializeGL(self):
        self.initializeScene()
        for s in self.scene:
            s.initializeGL()

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glEnable(GL_DEPTH_TEST)
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        self.setGluLookAt()
        self.setLights()
        for i in range(len(self.scene)):
            glPushName(i)
            try:
                self.scene[i].paintGL()
            except:
                print "....Exception: ", self.scene[i].title, ": scene[%d].paintGL()" % i
            glPopName()
        glPopMatrix()

    def resizeGL(self, w, h):
        if(h > 0):
            self.aspectRatio = w/(1.0*h)
            glViewport(0,0, w, h)
            self.setGlProjection()

    def initializeScene(self):
        if((sys.platform != 'darwin') and (sys.platform != 'win32')):
            glutInit(sys.argv)      #This must be here to get it to work with Freeglut.
            #otherwise you get: "freeglut  ERROR:  Function <glutWireCube> called without first calling 'glutInit'."
       
        backgroundColor = QtGui.QColor(0, 0, 0, 255)
        glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundColor.alphaF())
        glClearDepth( 1.0 )
        
        self.setLights()

        if(self.fogEnabled):
            fogColor = QtGui.QColor(0, 0, 0, 255)
            glFogi(GL_FOG_MODE, GL_LINEAR)
            glFogfv(GL_FOG_COLOR, [fogColor.redF(), fogColor.greenF(), fogColor.blueF(), fogColor.alphaF()])
            glFogf(GL_FOG_DENSITY, self.fogDensity)
            glHint(GL_FOG_HINT, GL_DONT_CARE)
            glFogf(GL_FOG_START, self.near)
            glFogf(GL_FOG_END, self.far)
            glEnable(GL_FOG)
        else:
            glDisable(GL_FOG)

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)

    def setLights(self):
        glLight = [GL_LIGHT0, GL_LIGHT1]
        light0Color = QtGui.QColor(255, 255, 255, 255)
        light1Color = QtGui.QColor(255, 255, 255, 255)

        lightsColor = [[light0Color.redF(), light0Color.greenF(), light0Color.blueF(), 1.0],[light1Color.redF(), light1Color.greenF(), light1Color.blueF(), 1.0]]
        for i in range(2):
            if(self.lightsEnabled[i]):
                afPropertiesAmbient = [lightsColor[i][0]*0.3, lightsColor[i][1]*0.3, lightsColor[i][2]*0.3, 1.00]
                afPropertiesDiffuse = lightsColor[i]
                afPropertiesSpecular = [lightsColor[i][0]*0.1, lightsColor[i][0]*0.1, lightsColor[i][0]*0.1, 1.00]
                if(self.lightsUseEyePosition[i]):
                    afLightPosition = [self.eye[0], self.eye[1], self.eye[2], 1.0]
                else:
                    afLightPosition = [self.lightsPosition[i][0], self.lightsPosition[i][1], self.lightsPosition[i][2], 1.0]
                glLightfv(glLight[i], GL_AMBIENT,  afPropertiesAmbient)
                glLightfv(glLight[i], GL_DIFFUSE,  afPropertiesDiffuse)
                glLightfv(glLight[i], GL_SPECULAR, afPropertiesSpecular)
                glLightfv(glLight[i], GL_POSITION, afLightPosition)
                glEnable(glLight[i])
            else:
                glDisable(glLight[i])

    def setGluLookAt(self):
        gluLookAt(self.eye[0], self.eye[1], self.eye[2],
                  self.center[0], self.center[1], self.center[2],
                  self.up[0], self.up[1], self.up[2])

    def setGluPerspective(self):
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)
        #glOrtho(-200 * self.eyeZoom, 200 * self.eyeZoom, -200 * self.eyeZoom, 200 * self.eyeZoom, self.near, self.far)

    def setGlProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        self.setGluPerspective()
        glMatrixMode(GL_MODELVIEW)

    def modelChanged(self):
        mins = []
        maxs = []
        eyeDist = (self.eye - self.center).length()
        for s in self.scene:
            if(s.loaded):
                center = s.getCenter()
                dist   = s.getDistance()
                modelDist = (self.center - center).length()
                mins.append(eyeDist - modelDist - dist/2.0)
                maxs.append(eyeDist + modelDist + dist/2.0)
        self.near = min(mins)
        self.far  = max(maxs)
        self.setNearFarZoom()
        self.updateGL()

    def setEye(self, v):
        if(self.eye != v):
            self.eye = v
            try:
                self.look  = (self.center - self.eye).normalize()
                self.right = (self.look^self.up).normalize()            #print("Eye: right :", self.right)
                self.up    = (self.right^self.look).normalize()
            except:
                self.look  = Vec3(0,1,0)
                self.right = Vec3(1,0,0)
                self.up    = Vec3(0,0,1)

    def setCenter(self, v):
        if(self.center != v):
            self.center = v
            try:
                self.look  = (self.center - self.eye).normalize()
                self.right = (self.look^self.up).normalize()
            except:
                self.look  = Vec3(0,1,0)
                self.right = Vec3(1,0,0)

    def setUp(self, v):
        if(self.up != v.normalize()):
            self.up = v.normalize()
            try:
                self.right = (self.look^self.up   ).normalize()
                self.up    = (self.right^self.look).normalize()
            except:
                self.right = Vec3(1,0,0)

    def setEyeRotation(self, yaw, pitch, roll):
        look = (self.eye + self.up*pitch + self.right*yaw - self.center).normalize()
        d = (self.eye - self.center).length()
        eye = self.center + look*d
        
        self.setEye(eye)
        
        up = (self.right*roll*0.01 + self.up).normalize()
        self.setUp(up)

    def setNearFarZoom(self):
        self.eyeZoom = min(max(self.eyeZoom, 0.0001), 0.9999);
        self.near    = max(min(self.near, self.far), 0.1)
        self.far     = max(self.near + 1.0, self.far)
        glFogf(GL_FOG_START, self.near)
        glFogf(GL_FOG_END,   self.far)
        self.setGlProjection()

    def sceneSetCenter(self):
        minmax=[MinMax(), MinMax(), MinMax()]
        for s in self.scene:
            if s.loaded:
                minPos, maxPos = s.getMinMax()
                for i in range(3):
                    minmax[i].setMin(minPos[i])
                    minmax[i].setMax(maxPos[i])
        
        sceneMin = Vec3([minmax[i].getMin() for i in range(3)])
        sceneMax = Vec3([minmax[i].getMax() for i in range(3)])
        c   = (sceneMin + sceneMax)*0.5
        d   = (sceneMin - sceneMax).length()
        
        self.sceneSetCenterLocal(c[0], c[1], c[2], d)

    def sceneSetCenterLocal(self, cX, cY, cZ, d):
        self.setCenter(Vec3(cX, cY, cZ))
        self.setEye(Vec3(cX, cY, cZ - d))
        self.setUp(Vec3(0, -1, 0))
        self.modelChanged()

    def pickObject(self, x, y):
        viewport = list(glGetIntegerv(GL_VIEWPORT))
        glSelectBuffer(10000)
        glRenderMode(GL_SELECT)

        glInitNames()
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()
        gluPickMatrix(x, viewport[3]-y, 5, 5, viewport)
        self.setGluPerspective()
        self.paintGL()
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glFlush()

        hits = glRenderMode(GL_RENDER)
        return hits

    def moveConstant(self):
        return (self.eye - self.center).length() #* abs(tan(pi * self.eyeZoom))

    def mouseVec(self, dx, dy):
        newDx = self.moveConstant() * dx / float(self.width())
        newDy = self.moveConstant() * dy / float(self.height())
        return self.up*(-newDy) + self.right*newDx;

    def moveSelectedScene(self, dx, dy):
        dirVec = self.mouseVec(dx, dy)
        
        s = self.scene[self.selectedScene]
        s.selectionMove(dirVec)
        s.emitModelChanged()
#         for s in self.scene:
# #             print "  scene: ", s
#             s.selectionMove(dirVec)
#             s.emitModelChanged()

    def rotateSelectedScene(self, dx, dy):
        moveLength    = self.mouseVec(dx, dy)
        dirVec = moveLength.normalize()

        rotationAxis3D  = dirVec^self.look
        
        centerOfMass   = Vec3(0,0,0)
        
        totalCount = 0
        for s in self.scene:
            objectCount = s.renderer.selectionObjectCount()
            if(objectCount > 0):
                totalCount = totalCount + objectCount
                centerOfMass = centerOfMass + (s.objectToWorldCoordinates(s.renderer.selectionCenterOfMass()) * float(objectCount))
        if(totalCount > 0):
            centerOfMass = centerOfMass * float(1.0 / totalCount)

        for s in self.scene:
            selectionCOM  = s.worldToObjectCoordinates(centerOfMass)
            selectionAxis = s.worldToObjectCoordinates(rotationAxis3D)
            if(s.renderer.selectionRotate(selectionCOM, selectionAxis, moveLength.length())):
                s.emitModelChanged()

    def getMouseRay(self, x, y):
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        self.setGluLookAt()
        viewport = glGetIntegerv(GL_VIEWPORT)
        modelview = glGetDoublev(GL_MODELVIEW_MATRIX)
        projection = glGetDoublev(GL_PROJECTION_MATRIX)
        oglX = x
        oglY = viewport[3] - y
        oglZ = glReadPixels(oglX, oglY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT)[0][0]
        
        p2 = gluUnProject(oglX, oglY, oglZ, modelview, projection, viewport)
        glPopMatrix()
        return Vec3(p2) - self.eye

    def refreshMouseTracking(self):
        self.mouseTrackingEnabled    = False
        self.mouseTrackingEnabledRay = False
        for s in self.scene:
            self.mouseTrackingEnabled    = self.mouseTrackingEnabled    or s.mouseMoveEnabled
            self.mouseTrackingEnabledRay = self.mouseTrackingEnabledRay or s.mouseMoveEnabledRay
        self.setMouseTracking(self.mouseTrackingEnabled or self.mouseTrackingEnabledRay)
        self.updateGL()

    def processMouseWheel(self, dir, e):
        for s in self.scene:
            s.processMouseWheel(dir, e)

    def processMouseDown(self, hits, e):
        globalMinDepth = self.far + 1
        minNames = list()
        sceneId = -1
        for hit_record in hits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(globalMinDepth > minDepth):
                globalMinDepth = minDepth
                minNames = names
        if(minNames != list()):
            sceneId = minNames[0];
            minNames.pop(0)
        self.selectedScene = sceneId;

    def processMouseClick(self, hits, e, left, mid, right):
        self.emitMouseClickedRaw(hits, e)

        globalMinDepth = self.far + 1
        minNames = list()
        sceneId = -1
        for hit_record in hits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(self.scene[names[0]].selectEnabled and globalMinDepth > minDepth):
                globalMinDepth = minDepth
                minNames = names
        if(minNames != list()):
            sceneId = minNames[0]
            minNames.pop(0)
            
        if (left):
            if (e.modifiers() & QtCore.Qt.CTRL):        # Multiple selection mode
                if (sceneId >= 0):
                    self.scene[sceneId].processMouseClick(minNames, e, False)
            else:                                           # Single selection mode
                for i in range(len(self.scene)):
                    try:
                        self.scene[i].modelChanged()
                    except:
                        print "......exception: ", self.scene[i].title, "scene[%d].modelChanged()" % i
                
                for i in range(len(self.scene)):
                    if (i == sceneId):
                        self.scene[sceneId].processMouseClick(minNames, e, True)
                        
        elif (right):                                # Focusing on current point
            if(sceneId >= 0):
                self.scene[sceneId].emitElementClicked(minNames, e)

    def processMouseMove(self, hits, e):
        self.emitMouseMovedRaw(hits, e)
                          
        globalMinDepth = self.far + 1
        minNames = list()
        sceneId = -1
        for hit_record in hits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(self.scene[names[0]].mouseMoveEnabled and globalMinDepth > minDepth):
                globalMinDepth = minDepth
                minNames = names
        if(minNames != list()):
            sceneId = minNames[0];
            minNames.pop(0)
        if(sceneId >= 0):
            self.scene[sceneId].processMouseMove(minNames, e)

    def mousePressEvent(self, e):
        self.mouseDownPoint    = QtCore.QPoint(e.pos())
        self.mouseMovePoint    = QtCore.QPoint(e.pos())
        self.mouseLeftPressed  = (e.buttons() & QtCore.Qt.LeftButton)
        self.mouseMidPressed   = (e.buttons() & QtCore.Qt.MidButton)
        self.mouseRightPressed = (e.buttons() & QtCore.Qt.RightButton)
        self.processMouseDown(self.pickObject(self.mouseDownPoint.x(), self.mouseDownPoint.y()), e)

    def mouseReleaseEvent(self, e):
        self.mouseUpPoint = QtCore.QPoint(e.pos())
        #Enter selection mode only if we didnt move the mouse much.. (If the mouse was moved, then we assume a camera motion instead of a selection
        dx = self.mouseUpPoint.x() - self.mouseDownPoint.x()
        dy = self.mouseUpPoint.y() - self.mouseDownPoint.y()
             
        if (pow(self.mouseDownPoint.x() - self.mouseUpPoint.x(), 2) + pow(self.mouseDownPoint.y() - self.mouseUpPoint.y(), 2) <= 2):
            self.processMouseClick(self.pickObject(self.mouseUpPoint.x(), self.mouseUpPoint.y()), e, self.mouseLeftPressed, self.mouseMidPressed, self.mouseRightPressed)
        
        # auto rotate if ctrl + alt pressed
        if(self.mouseLeftPressed) and (e.modifiers() & QtCore.Qt.CTRL) and (e.modifiers() & QtCore.Qt.ALT):
            for i in range(100):
                self.setEyeRotation(-dx/10.0, dy/10.0, 0)
                self.updateGL()
#                 time.sleep(0.01)
            
        if(self.mouseTrackingEnabledRay):
            ray = self.getMouseRay(e.x(), e.y())
            for s in self.scene:
                if(s.mouseMoveEnabledRay):
                    s.processMouseClickRay(ray, 0.1, self.eye, e)

    def mouseMoveEvent(self, e):
        if(self.mouseTrackingEnabledRay):
            ray = self.getMouseRay(e.x(), e.y())
            for s in self.scene:
                if(s.mouseMoveEnabledRay):
                    s.processMouseMoveRay(ray, 0.1, self.eye, e)
                       
        if(self.mouseTrackingEnabled):
            self.processMouseMove(self.pickObject(e.x(), e.y()), e)

        dx = e.x() - self.mouseMovePoint.x()
        dy = e.y() - self.mouseMovePoint.y()
                        
        if (self.mouseLeftPressed):
            if (e.buttons() & QtCore.Qt.RightButton):           # Rolling the scene
                self.setEyeRotation(0, 0, dx)
            else:
                if e.modifiers() & QtCore.Qt.CTRL:           # Rotating the selection
                    self.rotateSelectedScene(dx, dy)
                else:                                               # Rotating the scene
                    self.setEyeRotation(-dx, dy, 0)
            
        elif (self.mouseRightPressed):
            if e.modifiers() & QtCore.Qt.CTRL:                 # Translating the selection
                self.moveSelectedScene(dx, dy)
            else:                                                   # Translating the scene
                translation = self.mouseVec(-dx, -dy)
                newEye = self.eye + translation;
                newCenter = self.center + translation;
                self.setEye(newEye)
                self.setCenter(newCenter)
                
        self.mouseMovePoint = QtCore.QPoint(e.pos())

        self.updateGL()

    def wheelEvent(self, e):
        if(e.delta() != 0):
            dir = e.delta()/abs(e.delta())
            self.processMouseWheel(dir, e)
            if(not (e.modifiers() & QtCore.Qt.ALT) and not (e.modifiers() & QtCore.Qt.CTRL)):     # Zoom in / out
                self.eyeZoom = self.eyeZoom + dir * 10.0/360.0
                self.setNearFarZoom()
            self.updateGL()

    def emitMouseMovedRaw(self, hits, event):
        self.emit(QtCore.SIGNAL("mouseMovedRAW(PyQt_PyObject, QMouseEvent)"), hits, event)

    def emitMouseClickedRaw(self, hits, event):
        self.emit(QtCore.SIGNAL("mouseClickedRAW(PyQt_PyObject, QMouseEvent)"), hits, event)
