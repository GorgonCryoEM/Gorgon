# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   This manages a single camera, and the scenes which are rendered using this camera 


import sys, os, time
from PyQt4 import QtOpenGL, QtCore, QtGui
from vector_lib import *
from scene_editor_form import SceneEditorForm
from libpyGORGON import Vector3DFloat
from cmath import *   
from libpyGORGON import CAlphaRenderer
from seq_model.Chain import Chain

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

    
class Camera(QtOpenGL.QGLWidget):
    def __init__(self, scene, main, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)
        self.app = main        
        self.app.themes.addDefaultRGB("Camera:Light:0", 255, 255, 255, 255)
        self.app.themes.addDefaultRGB("Camera:Light:1", 255, 255, 255, 255)
        self.app.themes.addDefaultRGB("Camera:Fog", 0, 0, 0, 255)
        self.app.themes.addDefaultRGB("Camera:Background", 0, 0, 0, 255)        
        self.near = 0
        self.cuttingPlane = 0.0
        self.scene = scene
        self.mouseTrackingEnabled = False
        self.mouseTrackingEnabledRay = False
        self.aspectRatio = 1.0
        self.selectedScene = -1
        self.lightsEnabled = [True, False]
        self.lightsPosition = [[1000,1000,1000], [-1000,-1000,-1000]]
        self.lightsUseEyePosition = [True, False]        
        self.mouseMovePoint = QtCore.QPoint(0,0)
        self.mouseDownPoint = QtCore.QPoint(0,0)     
        self.mouseLeftPressed = False
        self.mouseMidPressed = False
        self.mouseRightPressed = False
        
        self.fogDensity = 0.01
        self.fogEnabled = False
        
        self.center = [0,0,0]
        self.eye = [0,-4.1,0]
        self.look = [0,1.1,0]
        self.right = [1.1,0,0]
        self.up = [0,0,1.1]
        self.near = 0.11
        self.far = 1000.01
        self.eyeZoom = 0.26
        
            
        self.setCenter(0, 0, 0)
        self.setEye(0, -4, 0) 
        self.setUp(0, 0, 1)     
        self.setEyeRotation(0, 0, 0)
        self.lastPos = QtCore.QPoint()
        self.sceneEditor = SceneEditorForm(self.app, self)
        self.connect(self.app.themes, QtCore.SIGNAL("themeChanged()"), self.themeChanged)
        
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
    
    def setEye(self, x, y, z):
        if(self.eye != [x,y,z]):
            self.eye = [x, y, z]
            try:
                self.look = vectorNormalize([self.center[0] - self.eye[0], self.center[1] - self.eye[1], self.center[2] - self.eye[2]])
                self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))            #print("Eye: right :", self.right)      
                self.up = vectorNormalize(vectorCrossProduct(self.right, self.look))  
            except:
                self.look = [0,1,0]
                self.right = [1,0,0]
                self.up = [0,0,1]
            self.setRendererCuttingPlanes()
            self.emitCameraChanged()
    
    def setCenter(self, x, y, z):
        if(self.center != [x,y,z]):
            self.center = [x, y, z]
            try:
                self.look = vectorNormalize([self.center[0] - self.eye[0], self.center[1] - self.eye[1], self.center[2] - self.eye[2]])
                self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))
            except:
                self.look = [0,1,0]
                self.right = [1,0,0]
            self.setRendererCuttingPlanes()
            self.setRendererCenter()            
            self.emitCameraChanged()
        
    def setUp(self, x, y, z):
        if(self.up != vectorNormalize([x, y, z])):
            self.up = vectorNormalize([x, y, z])
            try:
                self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))
                self.up = vectorNormalize(vectorCrossProduct(self.right, self.look))
            except:
                self.right = [1,0,0]
            self.setRendererCuttingPlanes()
            self.emitCameraChanged()
        
    def setEyeRotation(self, yaw, pitch, roll):
        newLook = vectorNormalize(vectorSubtract(vectorAdd(self.eye, vectorScalarMultiply(yaw, self.right)), self.center));
        d = vectorDistance(self.eye, self.center)
        newEye = vectorAdd(self.center, vectorScalarMultiply(d, newLook))            
        
        newLook = vectorNormalize(vectorSubtract(vectorAdd(newEye, vectorScalarMultiply(pitch, self.up)), self.center));
        d = vectorDistance(newEye, self.center)
        newEye = vectorAdd(self.center, vectorScalarMultiply(d, newLook))
        
        self.setEye(newEye[0], newEye[1], newEye[2])
        
        newUp = vectorNormalize(vectorAdd(vectorScalarMultiply(roll*0.01, self.right), self.up))
        self.setUp(newUp[0], newUp[1], newUp[2])       
            
    def setNearFarZoom(self, near, far, zoom):
        if((self.eyeZoom != zoom) or (self.near != near) or (self.far != far)) :
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
            if(s.renderer.setCuttingPlane(self.cuttingPlane, self.look[0], self.look[1], self.look[2])) :
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
        
        distance = vectorDistance(sceneMin, sceneMax)
        [centerX, centerY, centerZ] = vectorScalarMultiply(0.5, vectorAdd(sceneMin, sceneMax))         
                     
        
        
        self.setCenter(centerX, centerY, centerZ)
        self.setEye(self.center[0] , self.center[1], self.center[2] - distance)
        self.setUp(0, -1, 0)
        centerDistance = vectorDistance(self.eye, self.center)
        self.setCuttingPlane(0.0)
        self.modelChanged()
         
        self.updateGL()
    
    def sceneSetCenterLocal(self, centerX, centerY, centerZ, distance):
        
        self.setCenter(centerX, centerY, centerZ)
        self.setEye(self.center[0] , self.center[1], self.center[2] - distance)
        self.setUp(0, -1, 0)
        centerDistance = vectorDistance(self.eye, self.center)
        self.setCuttingPlane(0.0)
        self.modelChanged()
        #self.setNearFarZoom(centerDistance - distance/2.0, centerDistance + distance/2.0, 0.25)
        #radius = vectorDistance([minX, minY, minZ], [maxX, maxY, maxZ]) / 2.0;
        #eyeDistance = vectorDistance(self.center, self.eye)
        #self.setNearFar(max(eyeDistance-radius, 0.1), eyeDistance + 2*radius)
         
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
       
        backgroundColor = self.app.themes.getColor("Camera:Background")        
        glClearColor(backgroundColor.redF(), backgroundColor.greenF(), backgroundColor.blueF(), backgroundColor.alphaF())
        glClearDepth( 1.0 )
        
        self.setLights()
        self.setNearFarZoom(0.1, 1000, 0.25)

        if(self.fogEnabled):
            fogColor = self.app.themes.getColor("Camera:Fog")
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
        light0Color = self.app.themes.getColor("Camera:Light:0")
        light1Color = self.app.themes.getColor("Camera:Light:1")

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
       
    def pickObject(self, x, y):        
        viewport = list(glGetIntegerv(GL_VIEWPORT))        
        glSelectBuffer(10000)
        glRenderMode(GL_SELECT)

        glInitNames()
        glMatrixMode(GL_PROJECTION)        
        glPushMatrix()
        glLoadIdentity()        
        gluPickMatrix(x, viewport[3]-y, 5, 5, viewport)
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)            
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
        return vectorSubtract([p2[0], p2[1], p2[2]], self.eye)
                
    def resizeGL(self, width, height):
        if(height > 0) :
            self.aspectRatio = width/(1.0*height)
            glViewport(0,0, width, height)
            self.setGlProjection()        

    def setGlProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)
        #glOrtho(-200 * self.eyeZoom, 200 * self.eyeZoom, -200 * self.eyeZoom, 200 * self.eyeZoom, self.near, self.far)
        glMatrixMode(GL_MODELVIEW)
    
    def refreshMouseTracking(self):
        self.mouseTrackingEnabled = False
        self.mouseTrackingEnabledRay = False
        for s in self.scene:
            self.mouseTrackingEnabled = self.mouseTrackingEnabled or s.mouseMoveEnabled
            self.mouseTrackingEnabledRay = self.mouseTrackingEnabledRay or s.mouseMoveEnabledRay
        self.setMouseTracking(self.mouseTrackingEnabled or self.mouseTrackingEnabledRay)
        self.updateGL()
     
    def moveSelectedScene(self, dx, dy):
        newDx = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dx / float(self.width())
        newDy = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dy / float(self.height())
        moveDirection = vectorAdd(vectorScalarMultiply(-newDy, self.up), vectorScalarMultiply(newDx, self.right))
        dirVec = Vector3DFloat(moveDirection[0], moveDirection[1], moveDirection[2])
        for s in self.scene:
            if(s.renderer.selectionMove(dirVec)):
                s.emitModelChanged()

    
    def rotateSelectedScene(self, dx, dy):
        newDx = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dx / float(self.width())
        newDy = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dy / float(self.height())

        moveLength = vectorAdd(vectorScalarMultiply(-newDy, self.up), vectorScalarMultiply(newDx, self.right))
        moveDirection = vectorNormalize(moveLength)
        rotationAxis = vectorCrossProduct(moveDirection, self.look)
        
        
        rotationAxis3D = Vector3DFloat(rotationAxis[0], rotationAxis[1], rotationAxis[2])
        centerOfMass = Vector3DFloat(0,0,0)
        
        totalCount = 0
        for s in self.scene:
            objectCount = s.renderer.selectionObjectCount()            
            if(objectCount > 0):
                totalCount = totalCount + objectCount
                centerOfMass = centerOfMass + (s.objectToWorldCoordinatesVector(s.renderer.selectionCenterOfMass()) * float(objectCount))
        if(totalCount > 0):
            centerOfMass = centerOfMass * float(1.0 / totalCount)

        for s in self.scene:
            selectionCOM = s.worldToObjectCoordinatesVector(centerOfMass)
            selectionAxis = s.worldToObjectCoordinatesVector(rotationAxis3D)
            if(s.renderer.selectionRotate(selectionCOM, selectionAxis, vectorSize(moveLength))):
                s.emitModelChanged()
                     
   
    def mousePressEvent(self, event):      
        self.mouseDownPoint = QtCore.QPoint(event.pos())
        self.mouseMovePoint = QtCore.QPoint(event.pos())
        self.mouseLeftPressed = (event.buttons() & QtCore.Qt.LeftButton)
        self.mouseMidPressed = (event.buttons() & QtCore.Qt.MidButton)
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
            for i in range(1000):
                self.setEyeRotation(-dx/10.0, dy/10.0, 0)
                self.updateGL()
                time.sleep(0.01)
            
            
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
                        
        if (event.buttons() & QtCore.Qt.LeftButton):
            if (event.buttons() & QtCore.Qt.RightButton):           # Rolling the scene
                self.setEyeRotation(0, 0, dx)
            else:                   
                if self.getSelectionMovementEnabled() and (event.modifiers() & QtCore.Qt.CTRL) :           # Rotating the selection
                    self.rotateSelectedScene(dx, dy)
                else:                                               # Rotating the scene
                    self.setEyeRotation(-dx, dy, 0)
            
        elif (event.buttons() & QtCore.Qt.RightButton):
            if self.getSelectionMovementEnabled() and (event.modifiers() & QtCore.Qt.CTRL):                 # Translating the selection
                self.moveSelectedScene(dx, dy)
            else:                                                   # Translating the scene
                newDx = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dx / float(self.width())
                newDy = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dy / float(self.height())                       
                translation = vectorAdd(vectorScalarMultiply(newDy, self.up), vectorScalarMultiply(-newDx, self.right));
                newEye = vectorAdd(self.eye, translation);               
                newCenter = vectorAdd(self.center, translation);                
                self.setEye(newEye[0], newEye[1], newEye[2])                            
                self.setCenter(newCenter[0], newCenter[1], newCenter[2])
                
        self.mouseMovePoint = QtCore.QPoint(event.pos())        

        self.updateGL()
    
    def wheelEvent(self, event):
        if(event.delta() != 0):
            direction = event.delta()/abs(event.delta())
            self.processMouseWheel(direction, event)
            if(event.modifiers() & QtCore.Qt.ALT) :                 # Setting the cutting plane
                self.setCuttingPlane(self.cuttingPlane + direction * 0.01)                
            elif (not (event.modifiers() & QtCore.Qt.ALT) and not (event.modifiers() & QtCore.Qt.CTRL)):     # Zoom in / out
                self.setNearFarZoom(self.near, self.far, self.eyeZoom + direction * 10.0/360.0)
                #newEye = vectorAdd(self.eye, vectorScalarMultiply(-direction * 0.1 * (vectorDistance(self.eye, self.look)), self.look))
                #self.setEye(newEye[0], newEye[1], newEye[2])
            self.updateGL()  
        
    def modelChanged(self):
        minDistance = 1000000000000.0
        maxDistance = 0.0
        eyeDist = vectorDistance(self.eye, self.center)
        for s in self.scene:
            if(s.loaded):
                (center, distance) = s.getCenterAndDistance()
                modelDist = vectorDistance(self.center, center)
                minDistance = min(minDistance, eyeDist - modelDist - distance/2.0)
                maxDistance = max(maxDistance, eyeDist + modelDist + distance/2.0)
        self.setNearFarZoom(minDistance, maxDistance, self.eyeZoom)
        self.updateGL()  

    def themeChanged(self):
        self.sceneEditor.ui.pushButtonLight1Color.setColor(self.app.themes.getColor("Camera:Light:0"))
        self.sceneEditor.ui.pushButtonLight2Color.setColor(self.app.themes.getColor("Camera:Light:1"))  
        self.sceneEditor.ui.pushButtonBackgroundColor.setColor(self.app.themes.getColor("Camera:Background"))
        self.sceneEditor.ui.pushButtonFogColor.setColor(self.app.themes.getColor("Camera:Fog"))
        self.initializeGL()
        
    def getSessionInfo(self, sessionManager):
        info = []
        info.extend(sessionManager.getRemarkLines("CAM", "CUTTING_PLANE", self.cuttingPlane))
        info.extend(sessionManager.getRemarkLines("CAM", "MOUSE_TRACKING_ENABLED", self.mouseTrackingEnabled))
        info.extend(sessionManager.getRemarkLines("CAM", "MOUSE_TRACKING_ENABLED_RAY", self.mouseTrackingEnabledRay))
        info.extend(sessionManager.getRemarkLines("CAM", "ASPECT_RATIO", self.aspectRatio))
        info.extend(sessionManager.getRemarkLines("CAM", "SELECTED_SCENE", self.selectedScene))
        info.extend(sessionManager.getRemarkLines("CAM", "LIGHTS_ENABLED", self.lightsEnabled))       
        info.extend(sessionManager.getRemarkLines("CAM", "LIGHTS_POSITION", self.lightsPosition))
        info.extend(sessionManager.getRemarkLines("CAM", "LIGHTS_USE_EYE_POSITION", self.lightsUseEyePosition))
        info.extend(sessionManager.getRemarkLines("CAM", "FOG_DENSITY", self.fogDensity))
        info.extend(sessionManager.getRemarkLines("CAM", "FOG_ENABLED", self.fogEnabled))            
        info.extend(sessionManager.getRemarkLines("CAM", "CENTER", self.center))
        info.extend(sessionManager.getRemarkLines("CAM", "EYE", self.eye))
        info.extend(sessionManager.getRemarkLines("CAM", "LOOK", self.look))
        info.extend(sessionManager.getRemarkLines("CAM", "RIGHT", self.right))
        info.extend(sessionManager.getRemarkLines("CAM", "UP", self.up))
        info.extend(sessionManager.getRemarkLines("CAM", "NEAR", self.near))
        info.extend(sessionManager.getRemarkLines("CAM", "FAR", self.far))
        info.extend(sessionManager.getRemarkLines("CAM", "EYE_ZOOM", self.eyeZoom))
            
        return info

    def loadSessionInfo(self, sessionManager, sessionProperties):
        self.cuttingPlane = sessionManager.getProperty(sessionProperties, "CAM", "CUTTING_PLANE")
        self.mouseTrackingEnabled = sessionManager.getProperty(sessionProperties, "CAM", "MOUSE_TRACKING_ENABLED")
        self.mouseTrackingEnabledRay = sessionManager.getProperty(sessionProperties, "CAM", "MOUSE_TRACKING_ENABLED_RAY")
        self.aspectRatio = sessionManager.getProperty(sessionProperties, "CAM", "ASPECT_RATIO")
        self.selectedScene = sessionManager.getProperty(sessionProperties, "CAM", "SELECTED_SCENE")
        self.lightsEnabled = sessionManager.getProperty(sessionProperties, "CAM", "LIGHTS_ENABLED")
        self.lightsPosition = sessionManager.getProperty(sessionProperties, "CAM", "LIGHTS_POSITION")
        self.lightsUseEyePosition = sessionManager.getProperty(sessionProperties, "CAM", "LIGHTS_USE_EYE_POSITION")
        self.fogDensity = sessionManager.getProperty(sessionProperties, "CAM", "FOG_DENSITY")
        self.fogEnabled = sessionManager.getProperty(sessionProperties, "CAM", "FOG_ENABLED")
        self.center = sessionManager.getProperty(sessionProperties, "CAM", "CENTER")
        self.eye = sessionManager.getProperty(sessionProperties, "CAM", "EYE")
        self.look = sessionManager.getProperty(sessionProperties, "CAM", "LOOK")
        self.right = sessionManager.getProperty(sessionProperties, "CAM", "RIGHT")
        self.up = sessionManager.getProperty(sessionProperties, "CAM", "UP")
        self.near = sessionManager.getProperty(sessionProperties, "CAM", "NEAR")
        self.far = sessionManager.getProperty(sessionProperties, "CAM", "FAR")
        self.eyeZoom = sessionManager.getProperty(sessionProperties, "CAM", "EYE_ZOOM")
        
        self.setNearFarZoom(self.near, self.far, self.eyeZoom)
        self.setRendererCuttingPlanes()
        self.setRendererCenter()
        self.emitCameraChanged()            
       
        
        
        
    def emitCameraChanged(self):
        self.emit(QtCore.SIGNAL("cameraChanged()"))
            
    def emitMouseMovedRaw(self, mouseHits, event):
        self.emit(QtCore.SIGNAL("mouseMovedRAW(PyQt_PyObject, QMouseEvent)"), mouseHits, event)

    def emitMouseClickedRaw(self, mouseHits, event):
        self.emit(QtCore.SIGNAL("mouseClickedRAW(PyQt_PyObject, QMouseEvent)"), mouseHits, event)   
	
    def getSelectionMovementEnabled(self):
        return self.sceneEditor.ui.checkBoxEnableSelectionMovement.isChecked()
        
    def setSelectionMovementEnabled(self, enable = True):
        self.sceneEditor.ui.checkBoxEnableSelectionMovement.setChecked(enable)
