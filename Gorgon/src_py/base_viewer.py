from PyQt4 import QtGui, QtCore, QtOpenGL
from gorgon_cpp_wrapper import VolumeRenderer
from model_visualization_form import ModelVisualizationForm

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "Gorgon", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class BaseViewer(QtGui.QWidget):
    DisplayStyleWireframe, DisplayStyleFlat, DisplayStyleSmooth = range(3)
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)        
        self.app = main      
        self.title = "Untitled"
        self.sceneIndex = -1;
        self.loaded = False
        self.isClosedMesh = True
        self.displayStyle = self.DisplayStyleSmooth;
        self.modelVisible = True
        self.model2Visible = False
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.modelChanged) 
        self.connect(self, QtCore.SIGNAL("modelLoaded()"), self.modelChanged) 
        self.connect(self, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged)    
        self.gllist = 0
        self.showBox = True
        self.modelColor = QtGui.QColor.fromRgba(QtGui.qRgba(180, 180, 180, 255))
        self.model2Color = QtGui.QColor.fromRgba(QtGui.qRgba(180, 180, 180, 255))
        self.boxColor = QtGui.QColor.fromRgba(QtGui.qRgba(255, 255, 255, 255))
        
    def initVisualizationOptions(self):
        self.visualizationOptions = ModelVisualizationForm(self.app, self)
                        
    def setBoundingBox(self, visible):
        self.showBox = visible
        self.emitModelChanged();

    def setBoundingBoxColor(self, color):
        self.boxColor = color
        self.emitModelChanged()

    def setDisplayStyle(self, style):
        self.displayStyle = style
        self.emitModelVisualizationChanged()

    def setModelVisibility(self, visible):
        self.modelVisible = visible
        self.emitModelChanged()

    def setModel2Visibility(self, visible):
        self.model2Visible = visible
        self.emitModelChanged()

    def setModelColor(self, color):
        self.modelColor = color
        self.emitModelChanged()

    def setModel2Color(self, color):
        self.model2Color = color
        self.emitModelChanged()

    def setMaterials(self, color):
        diffuseMaterial = [color.redF(), color.greenF(), color.blueF(), color.alphaF()]
        ambientMaterial = [color.redF()*0.2, color.greenF()*0.2, color.blueF()*0.2, color.alphaF()]
        specularMaterial = [1.0, 1.0, 1.0, 1.0]
        glMaterialfv(GL_BACK, GL_AMBIENT,   ambientMaterial) 
        glMaterialfv(GL_BACK, GL_DIFFUSE,   diffuseMaterial) 
        glMaterialfv(GL_BACK, GL_SPECULAR,  specularMaterial) 
        glMaterialf( GL_BACK, GL_SHININESS, 0.1)
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial) 
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial) 
        glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial) 
        glMaterialf( GL_FRONT, GL_SHININESS, 0.1)

    def initializeGLDisplayType(self):
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT)
        glEnable(GL_DEPTH_TEST);        
        glDepthMask(GL_TRUE);
        if(self.isClosedMesh):
            glEnable(GL_CULL_FACE)
        else:
            glDisable(GL_CULL_FACE)
                        
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
        if (self.gllist != 0):          
            self.initializeGLDisplayType()
            glCallList(self.gllist)
            self.unInitializeGLDisplayType();
          
    def loadData(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadFile(str(fileName))
            self.loaded = True
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoaded()
            self.emitViewerSetCenter()
    
    def unloadData(self):
        self.renderer.unload()
        self.loaded = False
        self.emitModelUnloaded()
        
    def modelChanged(self):
        self.updateActionsAndMenus()
        if self.gllist != 0:
            glDeleteLists(self.gllist,1)
            
        self.gllist = glGenLists(1)
        glNewList(self.gllist, GL_COMPILE)
        
        if(self.loaded and self.modelVisible):
            self.setMaterials(self.modelColor)
            self.renderer.draw(0)
        
        if(self.loaded and self.showBox):
            self.setMaterials(self.boxColor)
            self.renderer.drawBoundingBox()

        glEndList() 

    def emitModelLoaded(self):
        self.emit(QtCore.SIGNAL("modelLoaded()"))

    def emitModelUnloaded(self):
        self.emit(QtCore.SIGNAL("modelUnloaded()"))
        
    def emitModelChanged(self):
        self.emit(QtCore.SIGNAL("modelChanged()"))
        
    def emitModelVisualizationChanged(self):
        self.emit(QtCore.SIGNAL("modelVisualizationChanged()"))
               
    def emitViewerSetCenter(self):
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float, float, float)"), 
                                self.renderer.getMin(0), self.renderer.getMin(1), self.renderer.getMin(2),
                                self.renderer.getMax(0), self.renderer.getMax(1), self.renderer.getMax(2))
