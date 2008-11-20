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
#   Revision 1.47  2008/11/20 19:33:48  ssa1
#   Faster drawing of outline boxes
#
#   Revision 1.46  2008/11/20 19:04:07  ssa1
#   Proper scaling for binary and grayscale skeletonization
#
#   Revision 1.45  2008/11/20 18:33:05  ssa1
#   Using the origin of the MRC volume
#
#   Revision 1.44  2008/11/18 22:01:18  ssa1
#   Removing printfs, and adding cropping
#
#   Revision 1.43  2008/11/13 20:54:40  ssa1
#   Using the correct scale when loading volumes
#
#   Revision 1.42  2008/11/06 05:29:04  ssa1
#   CGI submission milestone for Interactive Skeletonization, and theme support, and fixing (hopefully) mac-os flicker bug
#
#   Revision 1.41  2008/10/28 22:18:05  ssa1
#   Changing visualization of meshes, and sketches
#
#   Revision 1.40  2008/10/17 22:58:36  colemanr
#   Fixed a 64 bit issue with the hitstack by converting to int (some
#   variables are of type numpy.32).
#
#   Revision 1.39  2008/10/03 17:26:57  colemanr
#   base_viewer.py: On a 64 bit system, some of the numbers from hitstack
#   list in camera.py are of type numpy.int32 rather than of type int.  This
#   caused mouse clicks on atoms to raise an exception.  I have converted
#   them to type int when they call a function in the renderer.
#
#   Revision 1.38  2008/09/26 18:55:34  colemanr
#   Added glutInit(sys.argv) to the modelChanged method to get it to work
#   with Freeglut.
#
#   Revision 1.37  2008/09/23 16:46:57  ssa1
#   CTRL + Mouse wheel for iso-value modification
#
#   Revision 1.36  2008/09/15 18:43:13  ssa1
#   Adding in right clicking (Focus) functionality
#
#   Revision 1.35  2008/09/15 16:37:54  ssa1
#   Implementing multiple selection behavior
#
#   Revision 1.34  2008/09/13 02:46:51  ssa1
#   Multiple selection behavior for cAlpha atoms
#
#   Revision 1.33  2008/09/03 19:53:20  ssa1
#   First loading the model, and then changing the viewing position
#
#   Revision 1.32  2008/09/03 19:48:19  ssa1
#   Maximizing performance of volume visualization by minimizing method call count
#
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
from libpyGORGON import VolumeRenderer, Vector3DFloat
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
        self.rotation = self.identityMatrix()
        self.connect(self, QtCore.SIGNAL("modelChanged()"), self.modelChanged) 
        self.connect(self, QtCore.SIGNAL("modelLoaded()"), self.modelChanged) 
        self.connect(self, QtCore.SIGNAL("modelUnloaded()"), self.modelChanged) 
        self.connect(self.app.themes, QtCore.SIGNAL("themeChanged()"), self.themeChanged)           
        self.gllist = 0
        self.showBox = False
        
    def initVisualizationOptions(self, visualizationForm):
        self.visualizationOptions = visualizationForm
    
    def identityMatrix(self):
        return [[1.0, 0.0, 0.0, 0.0],[0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, 0.0], [0.0, 0.0, 0.0, 1.0]] 
    
    def setScale(self, scaleX, scaleY, scaleZ):
        self.setScaleNoEmit(scaleX, scaleY, scaleZ)
        self.app.mainCamera.updateGL()

    def setScaleNoEmit(self, scaleX, scaleY, scaleZ):
        self.renderer.setSpacing(scaleX, scaleY, scaleZ)
        self.visualizationOptions.ui.doubleSpinBoxSizeX.setValue(scaleX)
        self.visualizationOptions.ui.doubleSpinBoxSizeY.setValue(scaleY)
        self.visualizationOptions.ui.doubleSpinBoxSizeZ.setValue(scaleZ)
        
    
    def setLocation(self, locationX, locationY, locationZ):
        self.setLocationNoEmit(locationX, locationY, locationZ)
        self.app.mainCamera.updateGL()
        
    def setLocationNoEmit(self, locationX, locationY, locationZ):
        self.renderer.setOrigin(locationX, locationY, locationZ)
        self.visualizationOptions.ui.doubleSpinBoxLocationX.setValue(locationX)
        self.visualizationOptions.ui.doubleSpinBoxLocationY.setValue(locationY)
        self.visualizationOptions.ui.doubleSpinBoxLocationZ.setValue(locationZ)
                        
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

        return [objectCoords[0] * scale[0] + origin[0],  objectCoords[1] * scale[1] + origin[1], objectCoords[2] * scale[2] + origin[2]]
    
    def worldToObjectCoordinates(self, worldCoords):
        origin = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]        
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        
        return [(worldCoords[0] - origin[0]) / scale[0], (worldCoords[1] - origin[1]) / scale[1], (worldCoords[2] - origin[2]) / scale[2]]
        
    
    def setBoundingBox(self, visible):
        self.showBox = visible
        if(hasattr(self.app, "mainCamera")) :
            self.app.mainCamera.updateGL()

    def getBoundingBoxColor(self):
        return self.app.themes.getColor(self.title + ":" + "BoundingBox" )

    def setBoundingBoxColor(self, color):
        self.app.themes.addColor(self.title + ":" + "BoundingBox", color)
        if(hasattr(self.app, "mainCamera")) :
            self.app.mainCamera.updateGL()

    def setDisplayStyle(self, style):
        self.displayStyle = style
        self.emitModelVisualizationChanged()

    def setModelVisibility(self, visible):
        self.modelVisible = visible
        self.emitModelChanged()

    def setModel2Visibility(self, visible):
        self.model2Visible = visible
        self.emitModelChanged()
        
    def getModelColor(self):
        return self.app.themes.getColor(self.title + ":" + "Model:0" )

    def getModel2Color(self):
        return self.app.themes.getColor(self.title + ":" + "Model:1" )
    
    def setModelColor(self, color):
        self.app.themes.addColor(self.title + ":" + "Model:0", color)
        self.emitModelChanged()

    def setModel2Color(self, color):
        self.app.themes.addColor(self.title + ":" + "Model:1", color)
        self.emitModelChanged()

    def setMaterials(self, color):
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF())
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

    def getCenterAndDistance(self):
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        minPos = [(self.renderer.getMin(0)*scale[0] + location[0]), 
                  (self.renderer.getMin(1)*scale[1] + location[1]), 
                  (self.renderer.getMin(2)*scale[2] + location[2])]
        maxPos = [(self.renderer.getMax(0)*scale[0] + location[0]),
                  (self.renderer.getMax(1)*scale[1] + location[1]), 
                  (self.renderer.getMax(2)*scale[2] + location[2])]
        distance = vectorDistance(minPos, maxPos)

        center = vectorScalarMultiply(0.5, vectorAdd(minPos, maxPos))        

        return (center, distance)

    def initializeGLDisplayType(self):
        glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT)
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
        glPushMatrix()
        location = [self.renderer.getOriginX(), self.renderer.getOriginY(), self.renderer.getOriginZ()]
        glTranslated(location[0], location[1], location[2])
        glMultMatrixf(self.rotation)
        scale = [self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ()]
        glScaled(scale[0], scale[1], scale[2])   
                
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_ENABLE_BIT)
        glEnable(GL_DEPTH_TEST);        
        glDepthMask(GL_TRUE);
        
        if(self.loaded and self.showBox):            
            self.setMaterials(self.getBoundingBoxColor())       
            self.renderer.drawBoundingBox()             
        
        self.emitDrawingModel()                
        
        if (self.gllist != 0):          
            self.initializeGLDisplayType()
            glCallList(self.gllist)
            self.unInitializeGLDisplayType();

        glPopAttrib()
        glPopMatrix()
            
          
    def loadData(self):
        self.fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Data"), "", self.tr(self.renderer.getSupportedLoadFileFormats()))
        if not self.fileName.isEmpty():  
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.loadFile(str(self.fileName))
            self.setScaleNoEmit(self.renderer.getSpacingX(), self.renderer.getSpacingY(), self.renderer.getSpacingZ())       
            self.loaded = True
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.emitModelLoadedPreDraw()
            self.emitModelLoaded()            
            self.emitViewerSetCenter()
            
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
        self.renderer.setOrigin(0,0,0)
        self.renderer.setSpacing(1, 1, 1)
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
        colors = [self.getModelColor(),  self.getModel2Color()]
        
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT)
                         
        self.extraDrawingRoutines()
        
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
                            
        

        glPopAttrib()

        glEndList() 
        
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
            self.emitElementSelected(hitStack, event)

    def processMouseClickRay(self, ray, rayWidth, eye, event):
        self.emitMouseClickRay(ray, rayWidth, eye, event)

    def processMouseMove(self, hitStack, event):
        self.emitElementMouseOver(hitStack, event)
        
    def themeChanged(self):
        self.visualizationOptions.ui.pushButtonModelColor.setColor(self.getModelColor())
        self.visualizationOptions.ui.pushButtonModel2Color.setColor(self.getModel2Color())
        self.visualizationOptions.ui.pushButtonBoundingBoxColor.setColor(self.getBoundingBoxColor())  
        self.emitModelChanged()          
       
        
    def processMouseMoveRay(self, ray, rayWidth, eye, event):
        self.emitMouseOverRay(ray, rayWidth, eye, event)
        
    def emitThicknessChanged(self, value):
        self.emit(QtCore.SIGNAL("thicknessChanged(int)"), value);

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
    
    def emitDrawingModel(self):
        self.emit(QtCore.SIGNAL("modelDrawing()"))
    
    def emitViewerSetCenter(self):
        (center, distance) = self.getCenterAndDistance()
        self.emit(QtCore.SIGNAL("viewerSetCenter(float, float, float, float)"), center[0], center[1], center[2], distance)
