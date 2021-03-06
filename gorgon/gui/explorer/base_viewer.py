from PyQt4 import QtGui, QtCore

from base_dock_widget import BaseDockWidget
from gorgon.libs import Vec3
from .display_styles import *
from ...toolkit.libpytoolkit import *
from .common.model_visualization_form import ModelVisualizationForm

from .common.ui_dialog_model_visualization import Ui_DialogModelVisualization

class BaseViewer(BaseDockWidget):
    
    colorChanged = QtCore.pyqtSignal(QtGui.QColor)
    visualizationUpdated = QtCore.pyqtSignal()
    centerRequested = QtCore.pyqtSignal(float, float, float, float)
    centerAllRequested = QtCore.pyqtSignal()
    
    display_styles = [wireframe, flat, smooth]
    
    def __init__(self, renderer, main, form=ModelVisualizationForm):
        super(BaseViewer, self).__init__(
                                main,
                                self,
                                self.title,
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea,
                                QtCore.Qt.RightDockWidgetArea)
        self.renderer = renderer
        self.app = main
        self.loaded = False
        self.selectEnabled = False
        self.mouseMoveEnabled = True
        self.mouseMoveEnabledRay = True
        self.isClosedMesh = True
#         self.displayStyle = self.DisplayStyleSmooth
        self.modelVisible = True
        self.rotation = self.identityMatrix()

        self.glList =  GLuint()
        self.twoWayLighting = False
        self.isSetMaterial = True
        
        self.multipleSelection = True
        self.color = QtGui.QColor(180, 180, 180, 150)
        
        self.ui = form(self.app, self)
        # self.ui.createUI()
        # self.setupSignals()
        
        self.runDisplayType = smooth
        self.addSaveButton()
        self.dock.hide()

        self._offset = [0, 0, 0]

    # def setupSignals(self):
    #     self.ui.pushButtonModelColor.valueChanged.connect(self.setColor)
    #     # self.ui.checkBoxModelVisible.toggled.connect(self.setModelVisibility)
    #             
    #     self.colorChanged.connect(self.ui.pushButtonModelColor.setColor)
    #     self.ui.pushButtonCenter.clicked.connect(self.on_center_clicked)
    #     self.ui.pushButtonSave.clicked.connect(self.saveData)
    #     self.ui.labelModelSize.setText("{" +
    #                                    str(round(self.renderer.getMaxPos(0) - self.renderer.getMinPos(0) ,2)) + ", " +
    #                                    str(round(self.renderer.getMaxPos(1) - self.renderer.getMinPos(1) ,2)) + ", " +
    #                                    str(round(self.renderer.getMaxPos(2) - self.renderer.getMinPos(2) ,2)) + "}")
    #     
    #     self.ui.loc_scale_xyz.locChanged.connect(self.setLocation)
    #     self.ui.loc_scale_xyz.scaleChanged.connect(self.setScale)
    #     self.visualizationUpdated.connect(self.modelChanged)

#         self.ui.doubleSpinBoxSizeX.editingFinished.connect(self.scaleChanged)
#         self.ui.doubleSpinBoxSizeY.editingFinished.connect(self.scaleChanged)
#         self.ui.doubleSpinBoxSizeZ.editingFinished.connect(self.scaleChanged)
#         self.ui.spinBoxThickness.valueChanged.connect(self.setThickness)

    @property
    def offset(self):
        return self._offset

    @offset.setter
    def offset(self, val):
        self._offset = val
        self.modelChanged()

    def addSaveButton(self):
        self.pushButtonSave = QtGui.QPushButton(self)
        self.pushButtonSave.setObjectName("pushButtonSave")
        self.pushButtonSave.setText(QtGui.QApplication.translate("DialogModelVisualization", "Save", None, QtGui.QApplication.UnicodeUTF8))

        form = self.ui.ui
        if isinstance(form, Ui_DialogModelVisualization):
            gridlayout = form.gridlayout6
        else:
            gridlayout = form.gridlayout5

        gridlayout.addWidget(self.pushButtonSave, 0, 1, 1, 1)
        self.pushButtonSave.pressed.connect(self.saveData)

    def saveData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Data"), "")
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)

    def initializeGL(self):
        self.setupGlList()

    def initializeGLDisplayType(self):
        try:
            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT)
        except:
            print "...Exception: %s: initializeGLDisplayType()" % self.title
            
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
        
        self.runDisplayType()

    def unInitializeGLDisplayType(self):
        glPopAttrib()

    def paintGL(self):
        glPushMatrix()
        try:
            loc = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
#             loc = -loc
#             loc = [0, 0, 0]
#             print self.title, " (loc): ", loc
        except:
            pass
        else:
            glTranslated(loc[0] + self.offset[0], loc[1] + self.offset[1], loc[2] + self.offset[2])
        glMultMatrixf(self.rotation)
        try:
            scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        except:
            pass
        else:
            glScaled(scale[0], scale[1], scale[2])
                
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT)
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        
        if(self.loaded and self.modelVisible):
            if self.isSetMaterial:
                self.setMaterials()
            self.initializeGLDisplayType()
            
#             print "self.glList: %s: %d" %(self.title, self.glList)
#             print glIsList(self.glList)
            
            glCallList(self.glList)
            self.unInitializeGLDisplayType();

        glPopAttrib()
        glPopMatrix()

    def modelChanged(self):
        glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
        self.extraDrawingRoutines()
        if(self.loaded):
            self.setupGlList()
        glPopAttrib()
        self.app.mainCamera.updateGL()

    def setupGlList(self):
        self.glList = glGenLists(1)
        glNewList(self.glList, GL_COMPILE)
        self.drawGL()
        glEndList()

    def drawGL(self):
        ss=[0,1,2]
        for s in ss:
            if(self.color.alpha() < 255):
                glDepthFunc(GL_LESS)
                glColorMask(False, False, False, False)
                self.renderer.draw(s, False)
                glDepthFunc(GL_LEQUAL)
                glColorMask(True, True, True, True)
                self.renderer.draw(s, True)
            else:
                self.renderer.draw(s, True)
        
        self.extraDrawingRoutines()

    def setMaterials(self):
        color = self.color
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

    def extraDrawingRoutines(self):
        pass

    def getColor(self):
        return self.color

    def setColor(self, color):
        if(self.color != color):
            self.color = color
            self.visualizationUpdated.emit()
            self.colorChanged.emit(self.color)

    def setScale(self, x, y, z):
        self.renderer.setSpacing(x, y, z)
        self.visualizationUpdated.emit()

    def setLocation(self, x, y, z):
        # self.renderer.setOrigin(x, y, z)
        # self.app.mainCamera.setCenter(Vec3(x, y, z))
        self.offset = [x, y, z]
        self.visualizationUpdated.emit()

    def getCenter(self):
        min, max = self.getMinMax()
        return (min + max)*0.5

    def getDistance(self):
        min, max = self.getMinMax()
        return (min - max).length()

    def setModelVisibility(self, visible):
        self.modelVisible = visible
        self.visualizationUpdated.emit()

    def setThickness(self, value):
        self.thickness = value
        self.renderer.setLineThickness(value)
        self.modelChanged()

    def on_center_clicked(self):
        center   = self.getCenter()
        distance = self.getDistance()
        self.centerRequested.emit(center[0], center[1], center[2], distance)

    def getMinMax(self):
        scale    = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        # location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = Vec3([(self.renderer.getMinPos(i)) for i in range(3)])
        maxPos = Vec3([(self.renderer.getMaxPos(i)) for i in range(3)])
        return minPos, maxPos

    def load(self, fileName):
        try:
            self.renderer.loadFile(str(fileName))
            print self.title, self.renderer.getSize()
            self.setScale(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())
            self.loaded = True
            self.modelLoadedPreDraw()
            self.modelChanged()
            self.centerAllRequested.emit()
            self.ui.modelLoaded()
        except AttributeError:
            raise
        except:
            QtGui.QMessageBox.critical(self, "Unable to load data file", "The file might be corrupt, or the format may not be supported.", "Ok")

            self.loaded = False
        self.ui.ui.labelModelName.setText(fileName)

    def modelLoadedPreDraw(self):
        pass

    def save(self,fileName):
        self.setCursor(QtCore.Qt.WaitCursor)
        self.renderer.saveFile(str(fileName))
        self.setCursor(QtCore.Qt.ArrowCursor)

    def identityMatrix(self):
        return [[1.0, 0.0, 0.0, 0.0],
				[0.0, 1.0, 0.0, 0.0],
				[0.0, 0.0, 1.0, 0.0],
				[0.0, 0.0, 0.0, 1.0]
				]

    def setSelectEnabled(self, value):
        if(value != self.selectEnabled):
            self.selectEnabled = value
            self.modelChanged()

    def setRotation(self, axis, angle):
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        glRotatef(angle, axis[0], axis[1], axis[2])
        
        glMultMatrixf(self.rotation)
        
        self.rotation = glGetFloatv(GL_MODELVIEW_MATRIX)
        glPopMatrix()

    def setMouseMoveEnabled(self, value):
        if(value != self.mouseMoveEnabled):
            self.mouseMoveEnabled = value
            self.modelChanged()
            self.emitMouseTrackingChanged()

    def setMouseMoveEnabledRay(self, value):
        if(value != self.mouseMoveEnabledRay):
            self.mouseMoveEnabledRay = value
            self.emitMouseTrackingChanged()

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
        
    def objectToWorldCoordinatesVector(self, objectCoords):
        coords = self.objectToWorldCoordinates([objectCoords.x(), objectCoords.y(), objectCoords.z()])
        return Vector3Float(coords[0], coords[1], coords[2])
    
    def worldToObjectCoordinatesVector(self, worldCoords):
        coords = self.worldToObjectCoordinates([worldCoords.x(), worldCoords.y(), worldCoords.z()])
        return Vector3Float(coords[0], coords[1], coords[2])
    
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
        print "   ...clearSelection", self.title
        self.renderer.selectionClear()
        self.modelChanged()
            
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
            # self.performElementSelection(hitStack)
            if len(hitStack) == 0:
                hitStack.append(-1)
            if(len(hitStack) <= 6):
                #On a 64 bit system, hitStack[0] is of type numpy.int32 rather than int (which is 64 bit)
                self.renderer.selectionToggle(int(hitStack[0]), forceTrue, hits[0], hits[1], hits[2], hits[3], hits[4])
            # else:
            #     raise Exception("Unable to call renderer.select method due as there are too many levels in the hit stack")
            self.modelChanged()
            self.emitElementClicked(hitStack, e)
            # self.emitElementSelected(hitStack, e)

    def processMouseClickRay(self, ray, rayWidth, eye, e):
        self.emitMouseClickRay(ray, rayWidth, eye, e)

    def processMouseMove(self, hitStack, e):
        self.emitElementMouseOver(hitStack, e)

    def processMouseMoveRay(self, ray, rayWidth, eye, e):
        self.emitMouseOverRay(ray, rayWidth, eye, e)

    def emitMouseClickRay(self, ray, rayWidth, eye, e):
        self.emit(QtCore.SIGNAL("mouseClickRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, e);

    def emitMouseOverRay(self, ray, rayWidth, eye, e):
        self.emit(QtCore.SIGNAL("mouseOverRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, e);

    def emitElementClicked(self, hitStack, e):
        print "emitElementClicked: ", self.title, hitStack
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
