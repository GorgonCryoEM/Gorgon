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
        self.cuttingPlane = 0.0
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
            self.setRendererCuttingPlanes()
            self.emitCameraChanged()
    
    def setCenter(self, v):
        if(self.center != v):
            self.center = v
            try:
                self.look  = (self.center - self.eye).normalize()
                self.right = (self.look^self.up).normalize()
            except:
                self.look  = Vec3(0,1,0)
                self.right = Vec3(1,0,0)
            self.setRendererCuttingPlanes()
            self.setRendererCenter()
            self.emitCameraChanged()
        
    def setUp(self, v):
        if(self.up != v.normalize()):
            self.up = v.normalize()
            try:
                self.right = (self.look^self.up   ).normalize()
                self.up    = (self.right^self.look).normalize()
            except:
                self.right = Vec3(1,0,0)
            self.setRendererCuttingPlanes()
            self.emitCameraChanged()
        
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
            self.emitCameraChanged()
    
    def setCuttingPlane(self, cuttingPlane):
        newCuttingPlane = min(max(cuttingPlane, -1.0), 1.0)
        if(self.cuttingPlane != newCuttingPlane):
            self.cuttingPlane = newCuttingPlane
            self.setRendererCuttingPlanes()
    
    def setRendererCuttingPlanes(self):
        for s in self.scene:
            if(s.renderer.setCuttingPlane(self.cuttingPlane, self.look[0], self.look[1], self.look[2])):
                s.emitModelChanged()
                
    def setRendererCenter(self):
        for s in self.scene:
            if(s.setCenter(self.center)):
                s.emitModelChanged()
                 
    def sceneSetCenter(self, cX, cY, cZ, d):
        sceneMin = [cX, cY, cZ]
        sceneMax = [cX, cY, cZ]
        for s in self.scene:
            if s.loaded:
                (minPos, maxPos) = s.getBoundingBox()
                for i in range(3):
                    if minPos[i] < sceneMin[i]:
                        sceneMin[i] = minPos[i]
                    if maxPos[i] > sceneMax[i]:
                        sceneMax[i] = maxPos[i]
        
        sceneMin = Vec3(sceneMin)
        sceneMax = Vec3(sceneMax)
        
        distance = (sceneMin - sceneMax).length()
        center   = (sceneMin + sceneMax)*0.5
        [centerX, centerY, centerZ] = [center.x(), center.y(), center.z()]
                     
        self.setCenter(center)
        self.setEye(Vec3(self.center[0], self.center[1], self.center[2] - distance))
        self.setUp(Vec3(0, -1, 0))
        centerDistance = (self.eye - self.center).length()
        self.setCuttingPlane(0.0)
        self.modelChanged()
         
        self.updateGL()
    
    def sceneSetCenterLocal(self, centerX, centerY, centerZ, distance):
        
        self.setCenter(Vec3(centerX, centerY, centerZ))
        self.setEye(Vec3(self.center[0], self.center[1], self.center[2] - distance))
        self.setUp(Vec3(0, -1, 0))
        centerDistance = (self.eye - self.center).length()
        self.setCuttingPlane(0.0)
        self.modelChanged()
         
        self.updateGL()
     
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
        
    def processMouseWheel(self, direction, event):
        for s in self.scene:
            s.processMouseWheel(direction, event)
     
    def processMouseDown(self, mouseHits, event):
        globalMinDepth = self.far + 1
        minNames = list()
        sceneId = -1
        for hit_record in mouseHits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(globalMinDepth > minDepth):
                globalMinDepth = minDepth
                minNames = names
        if(minNames != list()):
            sceneId = minNames[0];
            minNames.pop(0)
        self.selectedScene = sceneId;
            
    def processMouseClick(self, mouseHits, event, leftPressed, midPressed, rightPressed):
        self.emitMouseClickedRaw(mouseHits, event)

        globalMinDepth = self.far + 1
        minNames = list()
        sceneId = -1
        for hit_record in mouseHits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(self.scene[names[0]].selectEnabled and globalMinDepth > minDepth):
                globalMinDepth = minDepth
                minNames = names
        if(minNames != list()):
            sceneId = minNames[0]
            minNames.pop(0)
            
        if (leftPressed):
            if (event.modifiers() & QtCore.Qt.CTRL):        # Multiple selection mode
                if (sceneId >= 0):
                    self.scene[sceneId].processMouseClick(minNames, event, False)
            else:                                           # Single selection mode
                for i in range(len(self.scene)):
                    self.scene[i].clearSelection()
                    self.scene[i].renderer.clearOtherHighlights()
                    self.scene[i].emitModelChanged()
                
                for i in range(len(self.scene)):
                    if (i == sceneId):
                        self.scene[sceneId].processMouseClick(minNames, event, True)
                        
        elif (rightPressed):                                # Focusing on current point
            if(sceneId >= 0):
                self.scene[sceneId].emitElementClicked(minNames, event)
            
    def processMouseMove(self, mouseHits, event):
        self.emitMouseMovedRaw(mouseHits, event)
                          
        globalMinDepth = self.far + 1
        minNames = list()
        sceneId = -1
        for hit_record in mouseHits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(self.scene[names[0]].mouseMoveEnabled and globalMinDepth > minDepth):
                globalMinDepth = minDepth
                minNames = names
        if(minNames != list()):
            sceneId = minNames[0];
            minNames.pop(0)
        if(sceneId >= 0):
            self.scene[sceneId].processMouseMove(minNames, event)
       
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

        mouseHits = glRenderMode(GL_RENDER)
        return mouseHits

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
                
    def resizeGL(self, width, height):
        if(height > 0):
            self.aspectRatio = width/(1.0*height)
            glViewport(0,0, width, height)
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
        newDx = self.moveConstant() * dx / float(self.width())
        newDy = self.moveConstant() * dy / float(self.height())
        dirVec = self.up*(-newDy) + self.right*newDx
        
        s = self.scene[self.selectedScene]
        s.selectionMove(dirVec)
        s.emitModelChanged()
#         for s in self.scene:
# #             print "  scene: ", s
#             s.selectionMove(dirVec)
#             s.emitModelChanged()

    def rotateSelectedScene(self, dx, dy):
        newDx = self.moveConstant() * dx / float(self.width())
        newDy = self.moveConstant() * dy / float(self.height())

        moveLength    = self.up*(-newDy) + self.right*newDx
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
                     
    def mousePressEvent(self, event):
        self.mouseDownPoint    = QtCore.QPoint(event.pos())
        self.mouseMovePoint    = QtCore.QPoint(event.pos())
        self.mouseLeftPressed  = (event.buttons() & QtCore.Qt.LeftButton)
        self.mouseMidPressed   = (event.buttons() & QtCore.Qt.MidButton)
        self.mouseRightPressed = (event.buttons() & QtCore.Qt.RightButton)
        self.processMouseDown(self.pickObject(self.mouseDownPoint.x(), self.mouseDownPoint.y()), event)
        
    def mouseReleaseEvent(self, event):
        self.mouseUpPoint = QtCore.QPoint(event.pos())
        #Enter selection mode only if we didnt move the mouse much.. (If the mouse was moved, then we assume a camera motion instead of a selection
        dx = self.mouseUpPoint.x() - self.mouseDownPoint.x()
        dy = self.mouseUpPoint.y() - self.mouseDownPoint.y()
             
        if (pow(self.mouseDownPoint.x() - self.mouseUpPoint.x(), 2) + pow(self.mouseDownPoint.y() - self.mouseUpPoint.y(), 2) <= 2):
            self.processMouseClick(self.pickObject(self.mouseUpPoint.x(), self.mouseUpPoint.y()), event, self.mouseLeftPressed, self.mouseMidPressed, self.mouseRightPressed)
        
        # auto rotate if ctrl + alt pressed
        if(self.mouseLeftPressed) and (event.modifiers() & QtCore.Qt.CTRL) and (event.modifiers() & QtCore.Qt.ALT):
            for i in range(100):
                self.setEyeRotation(-dx/10.0, dy/10.0, 0)
                self.updateGL()
#                 time.sleep(0.01)
            
        if(self.mouseTrackingEnabledRay):
            ray = self.getMouseRay(event.x(), event.y())
            for s in self.scene:
                if(s.mouseMoveEnabledRay):
                    s.processMouseClickRay(ray, 0.1, self.eye, event)

    def mouseMoveEvent(self, event):
        if(self.mouseTrackingEnabledRay):
            ray = self.getMouseRay(event.x(), event.y())
            for s in self.scene:
                if(s.mouseMoveEnabledRay):
                    s.processMouseMoveRay(ray, 0.1, self.eye, event)
                       
        if(self.mouseTrackingEnabled):
            self.processMouseMove(self.pickObject(event.x(), event.y()), event)

        dx = event.x() - self.mouseMovePoint.x()
        dy = event.y() - self.mouseMovePoint.y()
                        
        if (self.mouseLeftPressed):
            if (event.buttons() & QtCore.Qt.RightButton):           # Rolling the scene
                self.setEyeRotation(0, 0, dx)
            else:
                if event.modifiers() & QtCore.Qt.CTRL:           # Rotating the selection
                    self.rotateSelectedScene(dx, dy)
                else:                                               # Rotating the scene
                    self.setEyeRotation(-dx, dy, 0)
            
        elif (self.mouseRightPressed):
            if event.modifiers() & QtCore.Qt.CTRL:                 # Translating the selection
                self.moveSelectedScene(dx, dy)
            else:                                                   # Translating the scene
                newDx = self.moveConstant() * dx / float(self.width())
                newDy = self.moveConstant() * dy / float(self.height())
                translation = self.up*newDy + self.right*(-newDx);
                newEye = self.eye + translation;
                newCenter = self.center + translation;
                self.setEye(newEye)
                self.setCenter(newCenter)
                
        self.mouseMovePoint = QtCore.QPoint(event.pos())

        self.updateGL()
    
    def wheelEvent(self, event):
        if(event.delta() != 0):
            direction = event.delta()/abs(event.delta())
            self.processMouseWheel(direction, event)
            if(event.modifiers() & QtCore.Qt.ALT):                 # Setting the cutting plane
                self.setCuttingPlane(self.cuttingPlane + direction * 0.01)
            elif (not (event.modifiers() & QtCore.Qt.ALT) and not (event.modifiers() & QtCore.Qt.CTRL)):     # Zoom in / out
                self.setNearFarZoom(self.near, self.far, self.eyeZoom + direction * 10.0/360.0)
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
        
    def emitCameraChanged(self):
        self.emit(QtCore.SIGNAL("cameraChanged()"))
            
    def emitMouseMovedRaw(self, mouseHits, event):
        self.emit(QtCore.SIGNAL("mouseMovedRAW(PyQt_PyObject, QMouseEvent)"), mouseHits, event)

    def emitMouseClickedRaw(self, mouseHits, event):
        self.emit(QtCore.SIGNAL("mouseClickedRAW(PyQt_PyObject, QMouseEvent)"), mouseHits, event)
