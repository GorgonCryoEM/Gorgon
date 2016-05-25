from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

from libpytoolkit import Display
from libpytoolkit import *
from .libs import Vec3
from .ui_common import Ui_Common
from base_dock_widget import BaseDockWidget

from Explorer.display_styles import *


class BaseViewer(BaseDockWidget):
    display_styles = [wireframe, flat, smooth]
    
    def __init__(self, main, parent=None):
        super(BaseViewer, self).__init__(
                                main,
                                self,
                                self.title,
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea,
                                QtCore.Qt.RightDockWidgetArea)
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
#         self.displayStyle = self.DisplayStyleSmooth
        self.modelVisible = True
        self.rotation = self.identityMatrix()
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self, QtCore.SIGNAL("modelLoaded()"), self.modelChanged)
        self.connect(self, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged)

        self.glList =  GLuint()
        self.twoWayLighting = False
        
        self.multipleSelection = True
        self.color = QtGui.QColor(180, 180, 180, 150)
        
        self.ui = Ui_Common()
        self.ui.setupUi(self)
        self.setupSignals()
#         self.ui.buttonGroup.setExclusive(False)
        
        self.runDisplayType = wireframe

    def setupSignals(self):
        self.ui.pushButtonModelColor.valueChanged.connect(self.setColor)
        self.ui.checkBoxModelVisible.toggled.connect(self.setModelVisibility)
                
        buttons = self.ui.buttonGroup.buttons()
        self.bg = self.ui.buttonGroup
        for i in range(len(self.display_styles)):
            self.bg.setId(buttons[i], i)
        
        print [self.bg.id(b) for b in buttons]
        
        self.bg.buttonClicked[int].connect(self.repaintCamera)
#         self.ui.pushButtonCenter.clicked.connect(self.viewer.emitViewerSetCenterLocal)
#         self.ui.pushButtonClose.clicked.connect(self.viewer.unload)
#         self.ui.doubleSpinBoxSizeX.editingFinished.connect(self.scaleChanged)
#         self.ui.doubleSpinBoxSizeY.editingFinished.connect(self.scaleChanged)
#         self.ui.doubleSpinBoxSizeZ.editingFinished.connect(self.scaleChanged)
#         self.ui.doubleSpinBoxLocationX.editingFinished.connect(self.locationChanged)
#         self.ui.doubleSpinBoxLocationY.editingFinished.connect(self.locationChanged)
#         self.ui.doubleSpinBoxLocationZ.editingFinished.connect(self.locationChanged)
#         self.ui.spinBoxThickness.editingFinished.connect(self.thicknessChanged)
        
    def initVisualizationOptions(self, visualizationForm):
        self.visualizationOptions = visualizationForm
    
    def getColor(self):
        return self.color

    def setColor(self, color):
        self.color = color
        self.repaintCamera()

    def identityMatrix(self):
        return [[1.0, 0.0, 0.0, 0.0],
				[0.0, 1.0, 0.0, 0.0],
				[0.0, 0.0, 1.0, 0.0],
				[0.0, 0.0, 0.0, 1.0]
				]
    
    def setScale(self, x, y, z):
        self.renderer.setSpacing(x, y, z)
        self.repaintCamera()
        
    def setLocation(self, x, y, z):
        self.renderer.setOrigin(x, y, z)
        self.repaintCamera()
                        
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
        
    def repaintCamera(self):
        self.app.mainCamera.updateGL()
        
    def setModelVisibility(self, visible):
        self.modelVisible = visible
        self.repaintCamera()

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

    def getMinMax(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = Vec3([(self.renderer.getMin(i)*scale[i] + location[i]) for i in range(3)])
        maxPos = Vec3([(self.renderer.getMax(i)*scale[i] + location[i]) for i in range(3)])
        
        return minPos, maxPos
        
    def getCenterAndDistance(self):
        min, max = self.getMinMax()
        dist = (min - max).length()

        center = (min + max)*0.5

        return center, dist

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
        
        self.display_styles[self.bg.checkedId()]()
    
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
        
        self.emitDrawingModel()
        
        if(self.loaded and self.modelVisible):
            self.setMaterials(self.getColor())
            self.initializeGLDisplayType()
            glCallList(self.glList)
            self.unInitializeGLDisplayType();

        glPopAttrib()
        glPopMatrix()
        
    def load(self, fileName):
        try:
            self.renderer.loadFile(str(fileName))
            print self.renderer.getSize()
            self.setScale(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())
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
        self.loaded = False
        self.dirty = False
        self.renderer.setOrigin(0,0,0)
        self.renderer.setSpacing(1, 1, 1)
        self.rotation = self.identityMatrix()
        
    def extraDrawingRoutines(self):
        pass
    
    def modelChanged(self):
        glDeleteLists(self.glList,1)
            
        glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
                         
        self.extraDrawingRoutines()
        
        if(self.loaded):
            self.glList = glGenLists(1)
            glNewList(self.glList, GL_COMPILE)

            if(self.getColor().alpha() < 255):
                glDepthFunc(GL_LESS)
                glColorMask(False, False, False, False)
                self.renderer.draw(0, False)
                glDepthFunc(GL_LEQUAL)
                glColorMask(True, True, True, True)
                self.renderer.draw(0, self.selectEnabled or self.mouseMoveEnabled)
            else:
                self.renderer.draw(0, self.selectEnabled or self.mouseMoveEnabled)
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
        
    def performElementSelection(self, hitStack):
        #Override this method to enable mouse selection functionality
        pass
            
    def processMouseWheel(self, amount, e):
        #Override this method to enable mouse wheel functionality
        pass
                                
    def processMouseClick(self, hitStack, e, forceTrue):
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
            self.emitModelChanged()
            self.emitElementClicked(hitStack, e)
            self.emitElementSelected(hitStack, e)

    def processMouseClickRay(self, ray, rayWidth, eye, e):
        self.emitMouseClickRay(ray, rayWidth, eye, e)

    def processMouseMove(self, hitStack, e):
        self.emitElementMouseOver(hitStack, e)
        
    def processMouseMoveRay(self, ray, rayWidth, eye, e):
        self.emitMouseOverRay(ray, rayWidth, eye, e)

    def setCenter(self, center):
        return False
    
    def emitThicknessChanged(self, value):
        self.emit(QtCore.SIGNAL("thicknessChanged(int)"), value);

    def emitMouseClickRay(self, ray, rayWidth, eye, e):
        self.emit(QtCore.SIGNAL("mouseClickRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, e);

    def emitMouseOverRay(self, ray, rayWidth, eye, e):
        self.emit(QtCore.SIGNAL("mouseOverRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, e);

    def emitElementClicked(self, hitStack, e):
        hits = [-1,-1,-1,-1,-1,-1]
        for i in range(6):
                if(len(hitStack) > i):
                    hits[i] = hitStack[i]
        self.emit(QtCore.SIGNAL("elementClicked (int, int, int, int, int, int, QMouseEvent)"), hits[0], hits[1], hits[2], hits[3], hits[4], hits[5], e)

    def emitElementSelected(self, hitStack, e):
        hits = [-1,-1,-1,-1,-1,-1]
        for i in range(6):
                if(len(hitStack) > i):
                    hits[i] = hitStack[i]
        self.emit(QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), hits[0], hits[1], hits[2], hits[3], hits[4], hits[5], e)
        
    def emitMouseTrackingChanged(self):
        self.emit(QtCore.SIGNAL("mouseTrackingChanged ()"))
        
    def emitElementMouseOver(self, hitStack, e):
        hits = [-1,-1,-1,-1,-1,-1]
        for i in range(6):
                if(len(hitStack) > i):
                    hits[i] = hitStack[i]
        self.emit(QtCore.SIGNAL("elementMouseOver (int, int, int, int, int, int, QMouseEvent)"), hits[0], hits[1], hits[2], hits[3], hits[4], hits[5], e)

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
