from mesh_engine import GLMesh, PrimitiveEngine, ObjLoaderEngine, Isosurface
import sys
import math
from gorgon import MarchingCubes
from PyQt4 import QtCore, QtGui, QtOpenGL

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "OpenGL grabber",
                            "PyOpenGL must be installed to run this example.",
                            QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default,
                            QtGui.QMessageBox.NoButton)
    sys.exit(1)


class MainWindow(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self, parent)

        self.engine = Isosurface()
        
        self.glWidget = GLWidget(self.engine)

        self.xSlider = self.createSlider(QtCore.SIGNAL("xRotationChanged(int)"),
                                         self.glWidget.setXRotation)
        self.ySlider = self.createSlider(QtCore.SIGNAL("yRotationChanged(int)"),
                                         self.glWidget.setYRotation)
        self.zSlider = self.createSlider(QtCore.SIGNAL("zRotationChanged(int)"),
                                         self.glWidget.setZRotation)
        self.scaleSlider = self.createScaleSlider(QtCore.SIGNAL("scaleChanged(int)"),
                                         self.glWidget.setScale)

        self.surfaceSlider, self.delayedSurface = self.createHorizontalSlider(self.engine.setSurfaceValue,1,1000)
        self.sampleSlider, self.delayedSample = self.createHorizontalSlider(self.engine.setSampleDensity,1,10)
        
        self.setCentralWidget(self.glWidget)

        dock = QtGui.QDockWidget(self.tr("Manipulation"), self)
        dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        widget = QtGui.QWidget(dock);
        
        mainLayout = QtGui.QHBoxLayout()
        mainLayout.addWidget(self.xSlider)
        mainLayout.addWidget(self.ySlider)
        mainLayout.addWidget(self.zSlider)
        mainLayout.addWidget(self.scaleSlider)
        widget.setLayout(mainLayout)
        
        dock.setWidget(widget)
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea,dock)

        dock = QtGui.QDockWidget(self.tr("Isosurface"), self)
        dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        widget = QtGui.QWidget(dock);
        
        mainLayout = QtGui.QHBoxLayout()
        mainLayout.addWidget(self.surfaceSlider)
        mainLayout.addWidget(self.sampleSlider)
        widget.setLayout(mainLayout)
        
        dock.setWidget(widget)
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea,dock)
        
        self.xSlider.setValue(15 * 16)
        self.ySlider.setValue(345 * 16)
        self.zSlider.setValue(0 * 16)
        self.scaleSlider.setValue(1000)
        self.surfaceSlider.setValue(10)
        self.sampleSlider.setValue(2)

        self.createActions()
        self.createMenus()
        
        self.setWindowTitle(self.tr("Gorgon Prototype"))

    def createSlider(self, changedSignal, setterSlot):
        slider = QtGui.QSlider(QtCore.Qt.Vertical)

        slider.setRange(0, 360 * 16)
        slider.setSingleStep(16)
        slider.setPageStep(15 * 16)
        slider.setTickInterval(15 * 16)
        slider.setTickPosition(QtGui.QSlider.TicksRight)
        slider.setTracking(True)
        self.glWidget.connect(slider, QtCore.SIGNAL("valueChanged(int)"), setterSlot)
        self.connect(self.glWidget, changedSignal, slider, QtCore.SLOT("setValue(int)"))

        return slider

    def createHorizontalSlider(self, setterSlot, minVal, maxVal):
        slider = QtGui.QSlider(QtCore.Qt.Vertical)
        delayed = DelayedFilter()

        slider.setRange(minVal, maxVal)
        delayed.connect(slider, QtCore.SIGNAL("valueChanged(int)"), delayed.setValue)
        self.engine.connect(delayed, QtCore.SIGNAL("valueChanged(int)"), setterSlot)
        #self.engine.connect(slider, QtCore.SIGNAL("valueChanged(int)"), setterSlot)

        return slider, delayed
    
    def createScaleSlider(self, changedSignal, setterSlot):
        slider = QtGui.QSlider(QtCore.Qt.Vertical)

        slider.setRange(0, 2000)
        #slider.setSingleStep(10)
        #slider.setPageStep(4*10)
        #slider.setTickInterval(4*10)
        slider.setTickPosition(QtGui.QSlider.TicksRight)

        self.glWidget.connect(slider, QtCore.SIGNAL("valueChanged(int)"), setterSlot)
        self.connect(self.glWidget, changedSignal, slider, QtCore.SLOT("setValue(int)"))

        return slider

    def open(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self)
        if not fileName.isEmpty():
            self.engine.load(fileName)
            
    def createActions(self):
        self.openAct = QtGui.QAction(self.tr("&Open..."), self)
        self.openAct.setShortcut(self.tr("Ctrl+O"))
        self.openAct.setStatusTip(self.tr("Open an existing file"))
        self.connect(self.openAct, QtCore.SIGNAL("triggered()"), self.open)

        self.exitAct = QtGui.QAction(self.tr("E&xit"), self)
        self.exitAct.setShortcut(self.tr("Ctrl+Q"))
        self.exitAct.setStatusTip(self.tr("Exit the application"))
        self.connect(self.exitAct, QtCore.SIGNAL("triggered()"), 
                     QtGui.qApp.closeAllWindows)
        
    def createMenus(self):
        self.fileMenu = self.menuBar().addMenu(self.tr("&File"))
        self.fileMenu.addAction(self.openAct)
        self.fileMenu.addAction(self.exitAct)
        
class GLWidget(QtOpenGL.QGLWidget):
    def __init__(self, engine, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)

        self.cubes = engine
        self.connect(self.cubes, QtCore.SIGNAL("meshChanged()"), self.updateGL)
        
        self.engine = ObjLoaderEngine()
        self.xRot = 0
        self.yRot = 0
        self.zRot = 0
        self.scale = 1000

        self.z = 0
        self.x = 0
        self.y = 0
        
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
            self.emit(QtCore.SIGNAL("xRotationChanged(int)"), angle)
            self.updateGL()

    def setYRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.yRot:
            self.yRot = angle
            self.emit(QtCore.SIGNAL("yRotationChanged(int)"), angle)
            self.updateGL()

    def setZRotation(self, angle):
        angle = self.normalizeAngle(angle)
        if angle != self.zRot:
            self.zRot = angle
            self.emit(QtCore.SIGNAL("zRotationChanged(int)"), angle)
            self.updateGL()

    def setX(self, num):
        if num != self.x:
            self.x = num
            #self.emit(QtCore.SIGNAL("zTranslationChanged(double)"), num)
            self.updateGL()

    def setY(self, num):
        if num != self.y:
            self.y = num
            #self.emit(QtCore.SIGNAL("zTranslationChanged(double)"), num)
            self.updateGL()

    def setZ(self, num):
        if num != self.z and num >= 0:
            self.z = num
            #self.emit(QtCore.SIGNAL("zTranslationChanged(double)"), num)
            self.updateGL()
            
    def setScale(self, size):
        if size != self.scale and size >= 0:
            self.scale = size
            self.emit(QtCore.SIGNAL("scaleChanged(int)"), size)
            self.updateGL()
            
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
        glLoadIdentity()
        self.setGluLookAt()
        #self.engine.drawMesh(self.xRot / 16.0, self.yRot / 16.0, self.zRot / 16.0, self.scale/1000.0)
        glutSolidCube(0.5)
        #self.cubes.drawMesh(self.xRot / 16.0, self.yRot / 16.0, self.zRot / 16.0, self.scale/1000.0)
        self.paintSelection()        

    def paintSelection(self):
        glSelectBuffer(512)
        glRenderMode(GL_SELECT)
        
        glInitNames()
        glPushName(0)
        glPushMatrix()
        glLoadName(1)
        glMatrixMode(GL_MODELVIEW)
        glutSolidCube(0.5)
        #self.cubes.drawMesh(self.xRot / 16.0, self.yRot / 16.0, self.zRot / 16.0, self.scale/1000.0)

        glPopMatrix()
        glFlush()

        bufferStack = glRenderMode(GL_RENDER)
        for hit_record in bufferStack:
            min_depth, max_depth, names = hit_record
            print min_depth, max_depth, names
        
    def resizeGL(self, width, height):
        ratio = width/(1.0*height)
        glViewport(0,0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45, ratio, 0.1, 100)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        
    def resize1GL(self, width, height):
        glViewport(0, 0, width, height);
        s = 1.414/2
        
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        if width <= height:
            aspect = height/width
            glOrtho(-s, s, -s*aspect, s*aspect, -10*s, 10*s)
        else:
            aspect = width/height
            glOrtho(-s*aspect, s*aspect, -s, s, -10*s, 10*s)
        
        glMatrixMode(GL_MODELVIEW)

    def mousePressEvent(self, event):
        self.lastPos = QtCore.QPoint(event.pos())

    def mouseMoveEvent(self, event):
        dx = event.x() - self.lastPos.x()
        dy = event.y() - self.lastPos.y()

        if event.buttons() & QtCore.Qt.LeftButton:
            #self.setXRotation(self.xRot + 8 * dy)
            #self.setYRotation(self.yRot + 8 * dx)
            self.setX(self.x + dx/16.0)
            self.setY(self.y + dy/16.0)
            
        elif event.buttons() & QtCore.Qt.RightButton:
            #self.setScale(self.scale + 8 * dx)
            self.setZ(self.z + dx/8.0)
            
            #self.setXRotation(self.xRot + 8 * dy)
            #self.setZRotation(self.zRot + 8 * dx)

        self.lastPos = QtCore.QPoint(event.pos())

    def meshProduced(self):
        #self.updateGL()
        pass

    def normalizeAngle(self, angle):
        while angle < 0:
            angle += 360 * 16
        while angle > 360 * 16:
            angle -= 360 * 16
        return angle

class DelayedFilter(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.delay = 200
        self.waiting = False
        self.value = 0
        self.timer = QtCore.QTimer(self)
        self.connect(self.timer, QtCore.SIGNAL("timeout()"), self.emitValue)
    
    def setValue(self, value):
        self.value = value
        if not self.waiting:
            self.timer.start(self.delay)
            self.waiting = True

    def emitValue(self):
        self.timer.stop()
        self.emit(QtCore.SIGNAL("valueChanged(int)"), self.value)
        self.waiting = False
        

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
    
