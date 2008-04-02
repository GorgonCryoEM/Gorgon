from PyQt4 import QtGui, QtCore, QtOpenGL
from delayed_filter import DelayedFilter
from render_thread import RenderThread

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

class IsoSurface(QtGui.QWidget):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.createUI()
        
        self.thread = RenderThread()
        self.connect(self.thread, QtCore.SIGNAL("meshCreated()"), self.meshChanged)
        self.gllist = 0

    def createUI(self):
        self.createActions()
        self.createMenus()
        self.surfaceSlider, self.delayedSurface = self.createHorizontalSlider(self.setSurfaceValue,1,1000)
        self.sampleSlider, self.delayedSample = self.createHorizontalSlider(self.setSampleDensity,1,10)

        dock = QtGui.QDockWidget(self.tr("IsoSurface"), self.app)
        dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        widget = QtGui.QWidget(dock);
        
        mainLayout = QtGui.QHBoxLayout()
        mainLayout.addWidget(self.surfaceSlider)
        mainLayout.addWidget(self.sampleSlider)
        widget.setLayout(mainLayout)
        
        dock.setWidget(widget)
        self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea,dock)

    def createActions(self):
        self.openAct = QtGui.QAction(self.tr("&Open MRC..."), self)
        self.openAct.setShortcut(self.tr("Ctrl+O"))
        self.openAct.setStatusTip(self.tr("Open an existing file"))
        self.connect(self.openAct, QtCore.SIGNAL("triggered()"), self.openDialog)

    def createMenus(self):
        self.app.menuOpen().addAction(self.openAct)
    
    def createHorizontalSlider(self, setterSlot, minVal, maxVal):
        slider = QtGui.QSlider(QtCore.Qt.Vertical)
        delayed = DelayedFilter()

        slider.setRange(minVal, maxVal)
        delayed.connect(slider, QtCore.SIGNAL("valueChanged(int)"), delayed.setValue)
        self.connect(delayed, QtCore.SIGNAL("valueChanged(int)"), setterSlot)

        return slider, delayed

    def openDialog(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self)
        if not fileName.isEmpty():
            self.load(fileName)
            
    def draw(self):
        self.drawMesh()
        
    def drawMesh(self):
        if self.gllist != 0:
            glPushMatrix()
            glTranslated(-0.5, -0.5, -0.5)
            glCallList(self.gllist)
            glPopMatrix()
            
    def load(self, filename):
        self.thread.loadMRC(filename)
        
    def setSurfaceValue(self, value):
        self.thread.setSurfaceValue(value)

    def setSampleDensity(self, value):
        pass

    def meshChanged(self):
        if self.gllist != 0:
            glDeleteLists(self.gllist,1)
            
        self.gllist = glGenLists(1)
        glNewList(self.gllist, GL_COMPILE)
        self.thread.drawMesh()
        glEndList()
        
        self.emit(QtCore.SIGNAL("meshChanged()"))
