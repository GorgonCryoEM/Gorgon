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
        
        self.app = main

        self.near = 0
        self.scene = scene
        self.mouseTrackingEnabled    = True
        self.mouseTrackingEnabledRay = True
        self.aspectRatio   = 1.0
        self.selectedScene = -1
        self.lightsEnabled = [True, False]
        self.lightsPosition = [Vec3(1000,1000,1000),
							   Vec3(-1000,-1000,-1000)
							   ]
        self.lightsUseEyePosition = [True, False]
        self.mouseMovePoint = QtCore.QPoint(0,0)
        self.mouseDownPoint = QtCore.QPoint(0,0)
        self.mouseLeftPressed  = False
        self.mouseMidPressed   = False
        self.mouseRightPressed = False
        
        self.fogDensity = 0.01
        self.fogEnabled = False
        
        self.center = Vec3(0.0,  0.0, 0.0)
        self.eye    = Vec3(0.0, -4.1, 0.0)
        self.look   = Vec3(0.0, 1.1, 0.0)
        self.right  = Vec3(1.1, 0.0, 0.0)
        self.up     = Vec3(0.0, 0.0, 1.1)
        self.near    = 0.11
        self.far     = 1000.01
        self.eyeZoom = 0.26
        
        self.setEyeRotation(0.0, 0.0, 0.0)
        self.setCenter     (self.center)
        self.setEye        (self.eye)
        self.setUp         (self.up)
        self.lastPos = QtCore.QPoint()
        
        for i in range(len(self.scene)):
            self.scene[i].sceneIndex = i;

        for s in self.scene:
            self.connect(s, QtCore.SIGNAL("viewerSetCenterLocal(float, float, float, float)"), self.sceneSetCenterLocal)
            self.connect(s, QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), self.sceneSetCenter)
            self.connect(s, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("modelLoaded()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("modelVisualizationChanged()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("mouseTrackingChanged()"), self.refreshMouseTracking)
    
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
            self.setRendererCenter()
        
    def setUp(self, v):
        if(self.up != v.normalize()):
            self.up = v.normalize()
            try:
                self.right = (self.look^self.up   ).normalize()
                self.up    = (self.right^self.look).normalize()
            except:
                self.right = Vec3(1,0,0)
        
    def setEyeRotation(self, yaw, pitch, roll):
        newLook = (self.eye + self.up*pitch + self.right*yaw - self.center).normalize()
        d = (self.eye - self.center).length()
        newEye = self.center + newLook*d
        
        self.setEye(newEye)
        
        newUp = (self.right*roll*0.01 + self.up).normalize()
        self.setUp(newUp)
            
    def setNearFarZoom(self, near, far, zoom):
        if((self.eyeZoom != zoom) or (self.near != near) or (self.far != far)):
            self.eyeZoom = min(max(zoom, 0.0001), 0.9999);
            nearChanged = (self.near != near)
            self.near = max(min(near, far), 0.1)
            self.far = max(self.near + 1.0, far)
            glFogf(GL_FOG_START, self.near)
            glFogf(GL_FOG_END, self.far)
            self.setGlProjection()
    
    def setRendererCenter(self):
        for s in self.scene:
            if(s.setCenter(self.center)):
                s.emitModelChanged()
                 
    def sceneSetCenter(self, cX, cY, cZ, d):
        sceneMin = Vec3(cX, cY, cZ)
        sceneMax = Vec3(cX, cY, cZ)
        for s in self.scene:
            if s.loaded:
                (minPos, maxPos) = s.getMinMax()
                for i in range(3):
                    if minPos[i] < sceneMin[i]:
                        sceneMin[i] = minPos[i]
                    if maxPos[i] > sceneMax[i]:
                        sceneMax[i] = maxPos[i]
        
        d = (sceneMin - sceneMax).length()
        center   = (sceneMin + sceneMax)*0.5
        
        self.sceneSetCenterCommon(center, d)
    
    def sceneSetCenterLocal(self, centerX, centerY, centerZ, d):
        center = Vec3(centerX, centerY, centerZ)
        self.sceneSetCenterCommon(center, d)
        
    def sceneSetCenterCommon(self, center, d):
        self.setCenter(center)
        self.setEye(Vec3(self.center[0], self.center[1], self.center[2] - d))
        self.setUp(Vec3(0, -1, 0))
        self.modelChanged()
     
    def minimumSizeHint(self):
        return QtCore.QSize(50, 50)

    def sizeHint(self):
        return QtCore.QSize(400, 400)
       
    def initializeGL(self):
        self.initializeScene()

    def initializeScene(self):
        if((sys.platform != 'darwin') and (sys.platform != 'win32')):
            glutInit(sys.argv)      #This must be here to get it to work with Freeglut.
            #otherwise you get: "freeglut  ERROR:  Function <glutWireCube> called without first calling 'glutInit'."
       
        backgroundColor = QtGui.QColor(0, 0, 0, 255)
        glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundColor.alphaF())
        glClearDepth( 1.0 )
        
        self.setLights()
        self.setNearFarZoom(0.1, 1000, 0.25)

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
        
    def paintGL(self):
        self.drawScene()
              
    def drawScene(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glEnable(GL_DEPTH_TEST)
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        self.setGluLookAt()
        self.setLights()
        for i in range(len(self.scene)):
            glPushName(i)
            self.scene[i].draw()
            glPopName()
        glPopMatrix()
        
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
                    self.scene[i].emitModelChanged()
                
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
       
    def setGluPerspective(self):
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)
        #glOrtho(-200 * self.eyeZoom, 200 * self.eyeZoom, -200 * self.eyeZoom, 200 * self.eyeZoom, self.near, self.far)

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
        self.drawScene()
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glFlush()

        hits = glRenderMode(GL_RENDER)
        return hits

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
                
    def resizeGL(self, w, h):
        if(h > 0):
            self.aspectRatio = w/(1.0*h)
            glViewport(0,0, w, h)
            self.setGlProjection()

    def setGlProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        self.setGluPerspective()
        glMatrixMode(GL_MODELVIEW)
    
    def refreshMouseTracking(self):
        self.mouseTrackingEnabled    = False
        self.mouseTrackingEnabledRay = False
        for s in self.scene:
            self.mouseTrackingEnabled    = self.mouseTrackingEnabled    or s.mouseMoveEnabled
            self.mouseTrackingEnabledRay = self.mouseTrackingEnabledRay or s.mouseMoveEnabledRay
        self.setMouseTracking(self.mouseTrackingEnabled or self.mouseTrackingEnabledRay)
        self.updateGL()
     
    def moveConstant(self):
        return (self.eye - self.center).length() #* abs(tan(pi * self.eyeZoom))

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

    def mouseVec(self, dx, dy):
        newDx = self.moveConstant() * dx / float(self.width())
        newDy = self.moveConstant() * dy / float(self.height())
        return self.up*(-newDy) + self.right*newDx;

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
                self.setNearFarZoom(self.near, self.far, self.eyeZoom + dir * 10.0/360.0)
            self.updateGL()
        
    def modelChanged(self):
        minDist = 1000000000000.0
        maxDist = 0.0
        eyeDist = (self.eye - self.center).length()
        for s in self.scene:
            if(s.loaded):
                (center, dist) = s.getCenterAndDistance()
                modelDist = (self.center - center).length()
                minDist = min(minDist, eyeDist - modelDist - dist/2.0)
                maxDist = max(maxDist, eyeDist + modelDist + dist/2.0)
        self.setNearFarZoom(minDist, maxDist, self.eyeZoom)
        self.updateGL()
        
    def emitMouseMovedRaw(self, hits, event):
        self.emit(QtCore.SIGNAL("mouseMovedRAW(PyQt_PyObject, QMouseEvent)"), hits, event)

    def emitMouseClickedRaw(self, hits, event):
        self.emit(QtCore.SIGNAL("mouseClickedRAW(PyQt_PyObject, QMouseEvent)"), hits, event)
