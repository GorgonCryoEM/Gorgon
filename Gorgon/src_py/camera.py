# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: This manages a single camera, and the scenes which are rendered using this camera

import sys
from PyQt4 import QtOpenGL, QtCore, QtGui
from vector_lib import *
from scene_editor_form import SceneEditorForm
from cmath import *   

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)
    
class Camera(QtOpenGL.QGLWidget):
    def __init__(self, scene, main, parent=None):                
        QtOpenGL.QGLWidget.__init__(self, parent)
        self.app = main
        self.near = 0
        self.cuttingPlane = 0.0
        self.scene = scene
        self.mouseTrackingEnabled = False
        self.aspectRatio = 1.0
        self.selectedScene = -1
        self.lightsEnabled = [True, True]
        self.lightsColor = [[1.0, 1.0, 1.0, 1.0], [1.0, 1.0, 1.0, 1.0]]
        self.lightsPosition = [[1000,1000,1000], [-1000,-1000,-1000]]        
        self.backgroundColor = [0.0, 0.0, 0.0, 1.0]
        self.mouseMovePoint = QtCore.QPoint(0,0)
        
        self.fogColor = [0.0, 0.0, 0.0, 1.0]
        self.fogDensity = 0.01
        self.fogEnabled = True
            
        self.setCenter(0, 0, 0)
        self.setEye(0, -4, 0) 
        self.setUp(0, 0, 1)     
        self.setEyeRotation(0, 0, 0)
        self.setNearFarZoom(0.1, 1000, 0.25)
        self.lastPos = QtCore.QPoint()
        self.sceneEditor = SceneEditorForm(self.app, self)
        
        for s in self.scene:
            self.connect(s, QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), self.sceneSetCenter)
            self.connect(s, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("modelLoaded()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("modelVisualizationChanged()"), self.modelChanged)
            self.connect(s, QtCore.SIGNAL("mouseTrackingChanged()"), self.refreshMouseTracking)
            
    def setEye(self, x, y, z):
        self.eye = [x, y, z]
        try:
            self.look = vectorNormalize([self.center[0] - self.eye[0], self.center[1] - self.eye[1], self.center[2] - self.eye[2]])
            self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))            #print("Eye: right :", self.right)      
            self.up = vectorNormalize(vectorCrossProduct(self.right, self.look))  
        except:
            self.look = [0,1,0]
            self.right = [-1,0,0]
            self.up = [0,0,1]
        self.setRendererCuttingPlanes()
        self.emitCameraChanged()
    
    def setCenter(self, x, y, z):
        self.center = [x, y, z]
        try:
            self.look = vectorNormalize([self.center[0] - self.eye[0], self.center[1] - self.eye[1], self.center[2] - self.eye[2]])
            self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))
        except:
            self.look = [0,1,0]
            self.right = [-1,0,0]
        self.setRendererCuttingPlanes()
        self.emitCameraChanged()
        
    def setUp(self, x, y, z):
        self.up = vectorNormalize([x, y, z])
        try:
            self.right = vectorNormalize(vectorCrossProduct(self.look, self.up))
            self.up = vectorNormalize(vectorCrossProduct(self.right, self.look))
        except:
            self.right = [-1,0,0]
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
        self.eyeZoom = min(max(zoom, 0.0001), 0.9999);
        nearChanged = (self.near != near)
        self.near = max(min(near, far), 0.1)
        self.far = max(self.near, far)
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
    
    def sceneSetCenter(self, centerX, centerY, centerZ, distance):        
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
        glClearColor(self.backgroundColor[0], self.backgroundColor[1], self.backgroundColor[2], self.backgroundColor[3])
        glClearDepth( 1.0 )
        
        glLight = [GL_LIGHT0, GL_LIGHT1]
        
        
        for i in range(2):
            if(self.lightsEnabled[i]):
                afPropertiesAmbient = [self.lightsColor[i][0]*0.3, self.lightsColor[i][1]*0.3, self.lightsColor[i][2]*0.3, 1.00] 
                afPropertiesDiffuse = self.lightsColor[i]
                afPropertiesSpecular = [self.lightsColor[i][0]*0.1, self.lightsColor[i][0]*0.1, self.lightsColor[i][0]*0.1, 1.00]
                afLightPosition = self.lightsPosition[i]         
                glLightfv(glLight[i], GL_AMBIENT,  afPropertiesAmbient)
                glLightfv(glLight[i], GL_DIFFUSE,  afPropertiesDiffuse) 
                glLightfv(glLight[i], GL_SPECULAR, afPropertiesSpecular) 
                glLightfv(glLight[i], GL_POSITION, afLightPosition)               
                glEnable(glLight[i]) 
            else:
                glDisable(glLight[i])

        if(self.fogEnabled):
            glFogi(GL_FOG_MODE, GL_LINEAR)
            glFogfv(GL_FOG_COLOR, self.fogColor)
            glFogf(GL_FOG_DENSITY, self.fogDensity)   
            glHint(GL_FOG_HINT, GL_DONT_CARE) 
            glFogf(GL_FOG_START, self.near)       
            glFogf(GL_FOG_END, self.far)
            glEnable(GL_FOG)
        else:
            glDisable(GL_FOG)    

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()        
       
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
        for i in range(len(self.scene)): 
            glPushName(i)
            self.scene[i].sceneIndex = i;     
            self.scene[i].draw()
            glPopName()
        glPopMatrix()
     
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
            
    def processMouseClick(self, mouseHits, event):   
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
            sceneId = minNames[0];
            minNames.pop(0)
        if(sceneId >= 0):
            self.scene[sceneId].processMouseClick(minNames, event)
       
    def processMouseMove(self, mouseHits, event):       
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
                
    def resizeGL(self, width, height):
        if(height > 0) :
            self.aspectRatio = width/(1.0*height)
            glViewport(0,0, width, height)
            self.setGlProjection()        

    def setGlProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(180 * self.eyeZoom, self.aspectRatio, self.near, self.far)
        glMatrixMode(GL_MODELVIEW)
    
    def refreshMouseTracking(self):
        self.mouseTrackingEnabled = False
        for s in self.scene:
            self.mouseTrackingEnabled = self.mouseTrackingEnabled or s.mouseMoveEnabled
        self.setMouseTracking(self.mouseTrackingEnabled)
        self.updateGL()
     
    def moveSelectedScene(self, dx, dy):
        if(self.selectedScene >= 0):
            newDx = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dx / float(self.width())
            newDy = vectorDistance(self.eye, self.center) * abs(tan(pi * self.eyeZoom)) * dy / float(self.height())            
            newLocation = vectorAdd(self.scene[self.selectedScene].location,  vectorScalarMultiply(newDx, self.right))   
            newLocation = vectorAdd(newLocation,  vectorScalarMultiply(-newDy, self.up))
            self.scene[self.selectedScene].setLocation(newLocation[0], newLocation[1], newLocation[2])
    
    def rotateSelectedScene(self, dx, dy):
        if(self.selectedScene >= 0):
            self.scene[self.selectedScene].setRotation(self.up, dx)
            self.scene[self.selectedScene].setRotation(self.right, dy)
            self.scene[self.selectedScene].emitModelChanged()
    
    def mousePressEvent(self, event):
        self.mouseDownPoint = QtCore.QPoint(event.pos())
        self.mouseMovePoint = QtCore.QPoint(event.pos())
        self.processMouseDown(self.pickObject(self.mouseDownPoint.x(), self.mouseDownPoint.y()), event)   
        
    def mouseReleaseEvent(self, event):
        self.mouseUpPoint = QtCore.QPoint(event.pos())
        #Enter selection mode only if we didnt move the mouse much.. (If the mouse was moved, then we assume a camera motion instead of a selection
        if (pow(self.mouseDownPoint.x() - self.mouseUpPoint.x(), 2) + pow(self.mouseDownPoint.y() - self.mouseUpPoint.y(), 2) <= 2): 
            self.processMouseClick(self.pickObject(self.mouseUpPoint.x(), self.mouseUpPoint.y()), event)

    def mouseMoveEvent(self, event):
        if(self.mouseTrackingEnabled):
            self.processMouseMove(self.pickObject(event.x(), event.y()), event)

        dx = event.x() - self.mouseMovePoint.x()
        dy = event.y() - self.mouseMovePoint.y()
                        
        if (event.buttons() & QtCore.Qt.LeftButton) and (event.modifiers() & QtCore.Qt.CTRL) :
            self.setEyeRotation(0, 0, dx)
        elif (event.buttons() & QtCore.Qt.LeftButton):
            self.setEyeRotation(-dx, dy, 0)
        elif (event.buttons() & QtCore.Qt.RightButton) and (event.modifiers() & QtCore.Qt.ALT):
            self.moveSelectedScene(dx, dy)
        elif (event.buttons() & QtCore.Qt.RightButton):
            self.rotateSelectedScene(dx, dy)            
        
        self.mouseMovePoint = QtCore.QPoint(event.pos())        

        self.updateGL()
    
    def wheelEvent(self, event):
        if(event.delta() != 0):
            direction = event.delta()/abs(event.delta())
            if(event.modifiers() & QtCore.Qt.CTRL) :
                self.setCuttingPlane(self.cuttingPlane + direction * 0.01);        
            else:
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
        
    def emitCameraChanged(self):
        self.emit(QtCore.SIGNAL("cameraChanged()"))                        
        
