# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   The base class for a viewable scene. 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.31  2008/08/27 15:26:52  marshm
#   Updates to SequenceView.  Updated coloring scheme for correspondence matches.
#
#   Revision 1.30  2008/07/07 14:45:06  ssa1
#   Changing the interactive skeletonization to go from OpenGL Hitstack to RayTracing
#
#   Revision 1.29  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtGui, QtCore, QtOpenGL
from libpyGORGON import VolumeRenderer
from model_visualization_form import ModelVisualizationForm
from vector_lib import *

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "Gorgon", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class BaseViewer(QtOpenGL.QGLWidget):
    DisplayStyleWireframe, DisplayStyleFlat, DisplayStyleSmooth = range(3)
    
    def __init__(self, main, parent=None):
        QtOpenGL.QGLWidget.__init__(self, parent)        
        self.app = main      
        self.title = "Untitled"
        self.fileName = "";
        self.sceneIndex = -1;
        self.loaded = False
        self.selectEnabled = False
        self.mouseMoveEnabled = False
        self.mouseMoveEnabledRay = False
        self.isClosedMesh = True
        self.viewerAutonomous = True
        self.displayStyle = self.DisplayStyleSmooth;
        self.modelVisible = True
        self.model2Visible = False
        self.location = [0,0,0]
        self.scale = [1,1,1]
        self.rotation = self.identityMatrix()
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
    
    def identityMatrix(self):
        return [[1.0, 0.0, 0.0, 0.0],[0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, 0.0], [0.0, 0.0, 0.0, 1.0]] 
    
    def setScale(self, scaleX, scaleY, scaleZ):
        self.scale = [scaleX, scaleY, scaleZ]
        self.emitModelChanged()
    
    def setLocation(self, locationX, locationY, locationZ):
        self.location = [locationX, locationY, locationZ]
        self.emitModelChanged()
                        
    def setRotation(self, axis, angle):
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        glRotatef(angle, axis[0], axis[1], axis[2])
        
        glMultMatrixf(self.rotation)
        
        self.rotation = glGetFloatv(GL_MODELVIEW_MATRIX)        
        glPopMatrix()
                        
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
        glMaterialf(GL_BACK, GL_SHININESS, 0.1)
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ambientMaterial) 
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuseMaterial) 
        glMaterialfv(GL_FRONT, GL_SPECULAR,  specularMaterial) 
        glMaterialf(GL_FRONT, GL_SHININESS, 0.1)

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

    def getCenterAndDistance(self):
        minPos = [(self.renderer.getMin(0)*self.scale[0] + self.location[0]), 
                  (self.renderer.getMin(1)*self.scale[1] + self.location[1]), 
                  (self.renderer.getMin(2)*self.scale[2] + self.location[2])]
        maxPos = [(self.renderer.getMax(0)*self.scale[0] + self.location[0]),
                  (self.renderer.getMax(1)*self.scale[1] + self.location[1]), 
                  (self.renderer.getMax(2)*self.scale[2] + self.location[2])]
        distance = vectorDistance(minPos, maxPos)

        center = vectorScalarMultiply(0.5, vectorAdd(minPos, maxPos))        

        return (center, distance)

    def initializeGLDisplayType(self):
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT)
        glEnable(GL_DEPTH_TEST);        
        glDepthMask(GL_TRUE);
        if(self.isClosedMesh):
            glEnable(GL_CULL_FACE)
        else:
            glDisable(GL_CULL_FACE)
                        
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
        if (self.gllist != 0):          
            self.initializeGLDisplayType()
            glCallList(self.gllist)
            self.unInitializeGLDisplayType();
          
    def loadData(self):
        self.fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not self.fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadFile(str(self.fileName))
	    #SequenceView(filename)
            self.loaded = True
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitViewerSetCenter()
            self.emitModelLoadedPreDraw()
            self.emitModelLoaded()            
            
    def saveData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Data"), "", self.tr(self.renderer.getSupportedSaveFileFormats()))
        if not self.fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
    
    def unloadData(self):
        self.fileName = ""
        self.renderer.unload()
        self.loaded = False
        self.dirty = False
        self.location = [0,0,0]
        self.scale = [1,1,1]
        self.rotation = self.identityMatrix()
        self.emitModelUnloaded()
        
    def extraDrawingRoutines(self):
        pass
        
    def modelChanged(self):
        self.updateActionsAndMenus()
        if self.gllist != 0:
            glDeleteLists(self.gllist,1)
            
        self.gllist = glGenLists(1)
        glNewList(self.gllist, GL_COMPILE)
        visibility = [self.modelVisible, self.model2Visible]
        colors = [self.modelColor, self.model2Color]
        
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
        
        glPushMatrix()
        glTranslated(self.location[0], self.location[1], self.location[2])
        glMultMatrixf(self.rotation)
        glScaled(self.scale[0], self.scale[1], self.scale[2])            
        
        if(self.loaded and self.showBox):            
            self.setMaterials(self.boxColor)       
            self.renderer.drawBoundingBox()        
        
        for i in range(2):
            if(self.loaded and visibility[i]):
                self.setMaterials(colors[i])
                if(colors[i].alpha() < 255):
                    glDepthFunc(GL_LESS)        
                    glColorMask(False, False, False, False)
                    self.renderer.draw(i, False)
                    glDepthFunc(GL_LEQUAL)
                    glColorMask(True, True, True, True) 
                    self.renderer.draw(i, self.selectEnabled or self.mouseMoveEnabled)
                else:
                    self.renderer.draw(i, self.selectEnabled or self.mouseMoveEnabled)                    
                            
        self.extraDrawingRoutines()

        glPopMatrix()
        glPopAttrib()

        glEndList() 
        
    def performElementSelection(self, hitStack):
        #Override this method to enable mouse selection functionality
        pass
            
                                
    def processMouseClick(self, hitStack, event):
        print self.title, ": ", hitStack
        hits = [-1,-1,-1,-1,-1]
        if(self.selectEnabled):
            for i in range(5):
                if(len(hitStack) > i+1):
                    hits[i] = hitStack[i+1]
            self.performElementSelection(hitStack)
            if len(hitStack) == 0:
                hitStack.append(-1)            
            if(len(hitStack) <= 6):
                self.renderer.select(hitStack[0], hits[0], hits[1], hits[2], hits[3], hits[4])
            else:
                raise Exception("Unable to call renderer.select method due as there are too many levels in the hit stack")
            self.emitModelChanged()            
            self.emitElementSelected(hitStack, event)

    def processMouseClickRay(self, ray, rayWidth, eye, event):
        self.emitMouseClickRay(ray, rayWidth, eye, event)

    def processMouseMove(self, hitStack, event):
        self.emitElementMouseOver(hitStack, event)
        
    def processMouseMoveRay(self, ray, rayWidth, eye, event):
        self.emitMouseOverRay(ray, rayWidth, eye, event)

    def emitMouseClickRay(self, ray, rayWidth, eye, event):
        self.emit(QtCore.SIGNAL("mouseClickRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, event);

    def emitMouseOverRay(self, ray, rayWidth, eye, event):
        self.emit(QtCore.SIGNAL("mouseOverRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), ray, rayWidth, eye, event);

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

    def emitModelUnloaded(self):
        self.emit(QtCore.SIGNAL("modelUnloaded()"))
        
    def emitModelChanged(self):
        self.emit(QtCore.SIGNAL("modelChanged()"))
        
    def emitModelVisualizationChanged(self):
        self.emit(QtCore.SIGNAL("modelVisualizationChanged()"))
    
    def emitViewerSetCenter(self):
        (center, distance) = self.getCenterAndDistance()
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), center[0], center[1], center[2], distance)
