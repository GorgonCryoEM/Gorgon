from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from libpytoolkit import Display
from libpytoolkit import *
from .libs import Vec3


class BaseViewer(QtOpenGL.QGLWidget):
    DisplayStyleWireframe, DisplayStyleFlat, DisplayStyleSmooth = range(3)
    
    def __init__(self, main, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)
        self.app = main
        self.title = "Untitled"
        self.shortTitle = "UNT"
        self.fileName = "";
        self.sceneIndex = -1;
        self.loaded = False
        self.selectEnabled = True
        self.mouseMoveEnabled = True
        self.mouseMoveEnabledRay = True
        self.isClosedMesh = True
        self.viewerAutonomous = True
        self.displayStyle = self.DisplayStyleSmooth;
        self.modelVisible = True
        self.model2Visible = True
        self.model3Visible = True
        self.rotation = self.identityMatrix()
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self, QtCore.SIGNAL("modelLoaded()"), self.modelChanged)
        self.connect(self, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged)

        self.glLists = []
        self.showBox = False
        self.twoWayLighting = False
        
        self.multipleSelection = True
        self.modelColor = QtGui.QColor(180, 180, 180, 150)
        
    def initVisualizationOptions(self, visualizationForm):
        self.visualizationOptions = visualizationForm
    
    def setModelColor(self, color):
        oldColor = self.getModelColor()
        self.setModelColor0(color)
        self.repaintCamera()

    def setModelColor0(self, color):
        self.modelColor = color

    def identityMatrix(self):
        return [[1.0, 0.0, 0.0, 0.0],
				[0.0, 1.0, 0.0, 0.0],
				[0.0, 0.0, 1.0, 0.0],
				[0.0, 0.0, 0.0, 1.0]
				]
    
    def setScale(self, x, y, z):
        self.setScaleNoEmit(x, y, z)
        self.app.mainCamera.updateGL()

    def setScaleNoEmit(self, x, y, z):
        self.renderer.setSpacing(x, y, z)
        
    def setLocationV(self, v):
        self.setLocation(v[0], v[1], v[2])

    def setLocation(self, x, y, z):
        self.renderer.setOrigin(x, y, z)
        self.app.mainCamera.updateGL()
                        
    def setRotation(self, axis, angle):
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        glRotatef(angle, axis[0], axis[1], axis[2])
        
        glMultMatrixf(self.rotation)
        
        self.rotation = glGetFloatv(GL_MODELVIEW_MATRIX)
        glPopMatrix()
                        
    def objectToWorldCoordinates(self, objectCoords):
        #Need to apply rotations
        origin = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]

        return Vec3([objectCoords[i] * scale[i] + origin[i] for i in range(3)])
    
    def worldToObjectCoordinates(self, worldCoords):
        origin = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        
        return Vec3([(worldCoords[i] - origin[i]) / scale[i] for i in range(3)])

    def objectVectorToWorldCoordinates(self, objectCoords):
        #Need to apply rotations
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        return Vec3([objectCoords[i] * scale[i] for i in range(3)])
    
    def worldVectorToObjectCoordinates(self, worldCoords):
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        return Vec3([worldCoords[i] / scale[i] for i in range(3)])
        
    def setBoundingBox(self, visible):
        self.showBox = visible
        if(hasattr(self.app, "mainCamera")):
            self.app.mainCamera.updateGL()

    def getBoundingBoxColor(self):
        return QtGui.QColor(255, 255, 255, 255)

    def repaintCamera(self):
        if(hasattr(self.app, "mainCamera")):
            self.app.mainCamera.updateGL()
        
    def setDisplayStyle(self, style):
        self.displayStyle = style
        self.emitModelVisualizationChanged()

    def setModelVisibility(self, visible):
        self.modelVisible = visible
        self.repaintCamera()

    def setModel2Visibility(self, visible):
        self.model2Visible = visible
        self.repaintCamera()
        
    def setModel3Visibility(self, visible):
        self.model3Visible = visible
        self.repaintCamera()
        
    def getModelColor(self):
        return self.modelColor

    def getModel2Color(self):
        return QtGui.QColor(180, 180, 180, 150)
    
    def getModel3Color(self):
        return QtGui.QColor(180, 180, 180, 150)
    
    def setMaterials(self, color):
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF())
        diffuseMaterial = [color.redF(), color.greenF(), color.blueF(), color.alphaF()]
        ambientMaterial = [color.redF()*0.2, color.greenF()*0.2, color.blueF()*0.2, color.alphaF()]
        specularMaterial = [1.0, 1.0, 1.0, 1.0]
        glMaterialfv(GL_BACK,  GL_AMBIENT,   ambientMaterial)
        glMaterialfv(GL_BACK,  GL_DIFFUSE,   diffuseMaterial)
        glMaterialfv(GL_BACK,  GL_SPECULAR,  specularMaterial)
        glMaterialf (GL_BACK,  GL_SHININESS, 0.1)
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial)
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial)
        glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial)
        glMaterialf (GL_FRONT, GL_SHININESS, 0.1)

    def setThickness(self, value):
        self.thickness = value
        self.renderer.setLineThickness(value)
        self.emitThicknessChanged(value)
        self.emitModelChanged()
        
    def setSelectEnabled(self, value):
        if(value != self.selectEnabled):
            self.selectEnabled = value
            self.emitModelChanged()

    def setMouseMoveEnabled(self, value):
        if(value != self.mouseMoveEnabled):
            self.mouseMoveEnabled = value
            self.emitModelChanged()
            self.emitMouseTrackingChanged()

    def setMouseMoveEnabledRay(self, value):
        if(value != self.mouseMoveEnabledRay):
            self.mouseMoveEnabledRay = value
            self.emitMouseTrackingChanged()

    #Override this method to handle menu enabling / disabling when another viewer takes control of this one.
    def updateViewerAutonomy(self, value):
        pass

    def setViewerAutonomy(self, value):
        self.viewerAutonomous = value;
        self.updateViewerAutonomy(value)
    
    def getBoundingBox(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = Vec3([(self.renderer.getMin(i)*scale[i] + location[i]) for i in range(3)])
        maxPos = Vec3([(self.renderer.getMax(i)*scale[i] + location[i]) for i in range(3)])
        return (minPos, maxPos)
        
    def getCenterAndDistance(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = Vec3([(self.renderer.getMin(i)*scale[i] + location[i]) for i in range(3)])
        maxPos = Vec3([(self.renderer.getMax(i)*scale[i] + location[i]) for i in range(3)])
        distance = (minPos - maxPos).length()

        center = (minPos + maxPos)*0.5

        return (center, distance)

    def initializeGLDisplayType(self):
        glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT)
        if(self.isClosedMesh):
            glEnable(GL_CULL_FACE)
        else:
            glDisable(GL_CULL_FACE)
            
        if(self.twoWayLighting):
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        else:
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
            
        #glDisable(GL_CULL_FACE)
        glEnable(GL_LIGHTING)
        
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if self.displayStyle == self.DisplayStyleWireframe:
            glPolygonMode(GL_FRONT, GL_LINE)
            glPolygonMode(GL_BACK, GL_LINE)
            
        elif self.displayStyle == self.DisplayStyleFlat:
            glPolygonMode(GL_FRONT, GL_FILL)
            glPolygonMode(GL_BACK, GL_FILL)
            glShadeModel(GL_FLAT)
            
        elif self.displayStyle == self.DisplayStyleSmooth:
            glPolygonMode(GL_FRONT, GL_FILL)
            glPolygonMode(GL_BACK, GL_FILL)
            glShadeModel(GL_SMOOTH)
            
        else:
            self.displayStyle = self.DisplayStyleSmooth;
            self.setDisplayType()
    
    def unInitializeGLDisplayType(self):
        glPopAttrib()

    def draw(self):
        glPushMatrix()
        loc = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        glTranslated(loc[0], loc[1], loc[2])
        glMultMatrixf(self.rotation)
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        glScaled(scale[0], scale[1], scale[2])
                
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT)
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        
        if(self.loaded and self.showBox):
            self.setMaterials(self.getBoundingBoxColor())
            self.renderer.drawBoundingBox()
        
        self.emitDrawingModel()
        
        visibility = self.getDrawVisibility()
        colors     = self.getDrawColors()
                
        for i in range(len(self.glLists)):
            if(self.loaded and visibility[i]):
                self.setMaterials(colors[i])
                self.initializeGLDisplayType()
                glCallList(self.glLists[i])
                self.unInitializeGLDisplayType();

        glPopAttrib()
        glPopMatrix()
        
    def load(self, fileName):
        try:
            self.renderer.loadFile(str(fileName))
            print self.renderer.getSize()
            self.setScaleNoEmit(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())
            self.loaded = True
            self.dirty = False
            self.emitModelLoadedPreDraw()
            self.emitModelLoaded()
            self.emitViewerSetCenter()
        except:
            QtGui.QMessageBox.critical(self, "Unable to load data file", "The file might be corrupt, or the format may not be supported.", "Ok")

            self.loaded = False
        
    def save(self,fileName):
        self.setCursor(QtCore.Qt.WaitCursor)
        self.renderer.saveFile(str(fileName))
        self.dirty = False
        self.setCursor(QtCore.Qt.ArrowCursor)
    
    def unload(self):
        self.fileName = ""
        self.renderer.unload()
        self.loaded = False
        self.dirty = False
        self.renderer.setOrigin(0,0,0)
        self.renderer.setSpacing(1, 1, 1)
        self.rotation = self.identityMatrix()
        self.emitModelUnloaded()
        
    def extraDrawingRoutines(self):
        pass
    
    def getDrawColors(self):
        return [self.getModelColor(),  self.getModel2Color(), self.getModel3Color()]
    
    def getDrawVisibility(self):
        return [self.modelVisible, self.model2Visible, self.model3Visible]
        
    def modelChanged(self):
        for list in self.glLists:
            glDeleteLists(list,1)
        self.glLists = []
            
        visibility = self.getDrawVisibility()
        colors = self.getDrawColors()
        
        glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
                         
        self.extraDrawingRoutines()
        
        if(self.loaded):
            for i in range(3):
                list = glGenLists(1)
                glNewList(list, GL_COMPILE)
                self.glLists.append(list)

                if(colors[i].alpha() < 255):
                    glDepthFunc(GL_LESS)
                    glColorMask(False, False, False, False)
                    self.renderer.draw(i, False)
                    glDepthFunc(GL_LEQUAL)
                    glColorMask(True, True, True, True)
                    self.renderer.draw(i, self.selectEnabled or self.mouseMoveEnabled)
                else:
                    self.renderer.draw(i, self.selectEnabled or self.mouseMoveEnabled)
                glEndList()
                                    
        glPopAttrib()

    def getClickCoordinates(self, hitStack):
        hits = [-1,-1,-1,-1,-1]
        for i in range(5):
            if(len(hitStack) > i+1):
                hits[i] = int(hitStack[i+1]) #int conversion helpful for 64 bit systems

        if len(hitStack) == 0:
            hitStack.append(-1)
        
        if(len(hitStack) <= 6):
            coords = self.renderer.get3DCoordinates(int(hitStack[0]), hits[0], hits[1], hits[2], hits[3], hits[4])
            return [coords.x(), coords.y(), coords.z()]
        else:
            raise Exception("Unable to call renderer.get3DCoordinates method due as there are too many levels in the hit stack")
        
    def clearSelection(self):
        if self.renderer.selectionClear():
            self.emitModelChanged()
        
    def performElementSelection(self, hitStack):
        #Override this method to enable mouse selection functionality
        pass
            
    def processMouseWheel(self, amount, event):
        #Override this method to enable mouse wheel functionality
        pass
                                
    def processMouseClick(self, hitStack, event, forceTrue):
        print self.title, ": ", hitStack
        hits = [-1,-1,-1,-1,-1]
        self.multipleSelection = not forceTrue
        if(self.selectEnabled):
            for i in range(5):
                if(len(hitStack) > i+1):
                    hits[i] = int(hitStack[i+1]) #On a 64 bit system, some of these are type numpy.int32 rather than int
            self.performElementSelection(hitStack)
            if len(hitStack) == 0:
                hitStack.append(-1)
            if(len(hitStack) <= 6):
                #On a 64 bit system, hitStack[0] is of type numpy.int32 rather than int (which is 64 bit)
                self.renderer.selectionToggle(int(hitStack[0]), forceTrue, hits[0], hits[1], hits[2], hits[3], hits[4])
            else:
                raise Exception("Unable to call renderer.select method due as there are too many levels in the hit stack")
            self.emitModelChanged()
            self.emitElementClicked(hitStack, event)
            self.emitElementSelected(hitStack, event)

    def processMouseClickRay(self, ray, rayWidth, eye, event):
        self.emitMouseClickRay(ray, rayWidth, eye, event)

    def processMouseMove(self, hitStack, event):
        self.emitElementMouseOver(hitStack, event)
        
    def processMouseMoveRay(self, ray, rayWidth, eye, event):
        self.emitMouseOverRay(ray, rayWidth, eye, event)

    def setCenter(self, center):
        return False
    
    def emitThicknessChanged(self, value):
        self.emit(QtCore.SIGNAL("thicknessChanged(int)"), value);

    def emitMouseClickRay(self, ray, rayWidth, eye, event):
        self.emit(QtCore.SIGNAL("mouseClickRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, event);

    def emitMouseOverRay(self, ray, rayWidth, eye, event):
        self.emit(QtCore.SIGNAL("mouseOverRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, event);

    def emitElementClicked(self, hitStack, event):
        hits = [-1,-1,-1,-1,-1,-1]
        for i in range(6):
                if(len(hitStack) > i):
                    hits[i] = hitStack[i]
        self.emit(QtCore.SIGNAL("elementClicked (int, int, int, int, int, int, QMouseEvent)"), hits[0], hits[1], hits[2], hits[3], hits[4], hits[5], event)

    def emitElementSelected(self, hitStack, event):
        hits = [-1,-1,-1,-1,-1,-1]
        for i in range(6):
                if(len(hitStack) > i):
                    hits[i] = hitStack[i]
        self.emit(QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), hits[0], hits[1], hits[2], hits[3], hits[4], hits[5], event)
        
    def emitMouseTrackingChanged(self):
        self.emit(QtCore.SIGNAL("mouseTrackingChanged ()"))
        
    def emitElementMouseOver(self, hitStack, event):
        hits = [-1,-1,-1,-1,-1,-1]
        for i in range(6):
                if(len(hitStack) > i):
                    hits[i] = hitStack[i]
        self.emit(QtCore.SIGNAL("elementMouseOver (int, int, int, int, int, int, QMouseEvent)"), hits[0], hits[1], hits[2], hits[3], hits[4], hits[5], event)

    def emitModelLoadedPreDraw(self):
        self.emit(QtCore.SIGNAL("modelLoadedPreDraw()"))
        
    def emitModelLoaded(self):
        self.emit(QtCore.SIGNAL("modelLoaded()"))
        
    def emitModelChanged(self):
        self.emit(QtCore.SIGNAL("modelChanged()"))
        
    def emitModelVisualizationChanged(self):
        self.emit(QtCore.SIGNAL("modelVisualizationChanged()"))
    
    def emitDrawingModel(self):
        self.emit(QtCore.SIGNAL("modelDrawing()"))

    def emitViewerSetCenterLocal(self):
        (center, distance) = self.getCenterAndDistance()
        self.emit(QtCore.SIGNAL("viewerSetCenterLocal(float, float, float, float)"), center[0], center[1], center[2], distance)
    
    def emitViewerSetCenter(self):
        (center, distance) = self.getCenterAndDistance()
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), center[0], center[1], center[2], distance)
