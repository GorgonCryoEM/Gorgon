# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget used to perform manual skeletonization of a volume 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.27  2008/12/02 03:27:05  ssa1
#   putting glpushattrib and glpopattrib when setting colors
#
#   Revision 1.26  2008/12/01 23:16:31  ssa1
#   Restructuring interactive skeleton drawing, and fixing it for scale space changes.
#
#   Revision 1.25  2008/11/28 04:36:17  ssa1
#   Removing error message if pyopengl does not exist.  (To make executable building easier to debug)
#
#   Revision 1.24  2008/11/06 05:29:04  ssa1
#   CGI submission milestone for Interactive Skeletonization, and theme support, and fixing (hopefully) mac-os flicker bug
#
#   Revision 1.23  2008/10/29 19:26:26  ssa1
#   Reducing memory footprint, Increasing performance and adding volume normalization
#
#   Revision 1.22  2008/10/28 22:18:05  ssa1
#   Changing visualization of meshes, and sketches
#
#   Revision 1.21  2008/10/16 19:50:44  ssa1
#   Supporting line deletion
#
#   Revision 1.20  2008/10/16 02:39:58  ssa1
#   Modifying the sketch behavior to supplement line drawing instead of replace it.
#
#   Revision 1.19  2008/10/15 19:41:32  ssa1
#   Esc to cancel path, Clear Button and Tracking of start seed point
#
#   Revision 1.18  2008/10/15 12:23:50  ssa1
#   Modifying the cost function for sketch interraction, and changing mousebehavior to trigger different interaction modes
#
#   Revision 1.17  2008/10/14 14:59:33  ssa1
#   Adding in sketching mode for interactive skeletonization
#
#   Revision 1.16  2008/10/10 14:25:57  ssa1
#   Setting the cost functions to scale with the edge length
#
#   Revision 1.15  2008/10/08 16:43:19  ssa1
#   Interactive skeletonization changes
#
#   Revision 1.14  2008/08/04 15:39:15  ssa1
#   Alternative scoring functions for interactive skeletonization
#
#   Revision 1.13  2008/07/22 13:21:17  ssa1
#   Reducing memory footprint while manual skeletonization
#
#   Revision 1.12  2008/07/15 15:10:17  ssa1
#   Adding snapping to Interactive skeletonization
#
#   Revision 1.11  2008/07/11 16:29:52  ssa1
#   Anisotropic Smoothing, and loading and saving volumes as sets of images
#
#   Revision 1.10  2008/07/07 14:45:06  ssa1
#   Changing the interactive skeletonization to go from OpenGL Hitstack to RayTracing
#
#   Revision 1.9  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from ui_dialog_volume_manual_skeletonization import Ui_DialogVolumeManualSkeletonization
from libpyGORGON import InteractiveSkeletonEngine

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

class VolumeManualSkeletonizationForm(QtGui.QWidget):
    MedialnessScoringFunctionBinary, MedialnessScoringFunctionGlobalRank, MedialnessScoringFunctionLocalRank = range(3)
    
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:StartingPoint", 0, 0, 255, 255)
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:EndingPoint", 0, 255, 0, 255)         
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:Sketch", 0, 128, 0, 255)
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:UnconfirmedCurve", 255, 255, 0, 255)
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:RemovableCurve", 100, 255, 100, 255)         
        self.started = False
        self.sketchStarted = False
        self.viewer = viewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)        
        
        self.isSkeletonClicked = False;
        self.createUI()
        self.createActions()
        self.createMenus()        

    def createUI(self):
        self.ui = Ui_DialogVolumeManualSkeletonization()
        self.ui.setupUi(self)    
        self.dock = QtGui.QDockWidget(self.tr("Interactive Skeletonization Mode"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()          
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.horizontalSliderStartingDensity,QtCore.SIGNAL("valueChanged(int)"),self.startingDensityChanged)     
        self.connect(self.ui.pushButtonStart,QtCore.SIGNAL("pressed ()"),self.startSkeletonization)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.endSkeletonization)   
        self.connect(self.ui.pushButtonClear, QtCore.SIGNAL("pressed ()"), self.clearSkeleton)
        self.connect(self.app, QtCore.SIGNAL("keyReleased(QKeyEvent)"), self.keyReleased)
        self.connect(self.app, QtCore.SIGNAL("keyPressed(QKeyEvent)"), self.keyPressed)
                                            
    def createActions(self):
        self.skeletonizeAct = QtGui.QAction(self.tr("&Interactive Skeletonization"), self)
        self.skeletonizeAct.setStatusTip(self.tr("Perform interactive skeletonization"))
        self.skeletonizeAct.setCheckable(True)
        self.skeletonizeAct.setChecked(False)
        self.skeletonizeAct.setEnabled(False)
        self.connect(self.skeletonizeAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("perform_VolumeManualVisualization", self.skeletonizeAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-volume-skeletonization-manual", self.skeletonizeAct, "actions-volume-skeletonization")                                   
                
    def loadWidget(self):
        if(self.skeletonizeAct.isChecked()) :
            self.showWidget(True)
        else:
            self.showWidget(False)

    def showWidget(self, show):
        if(show):
            self.app.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.dock)
            self.dock.show()          
        else:
            self.app.removeDockWidget(self.dock)
            self.setSkeletonViewerProperties(False)  
            self.ui.pushButtonStart.setEnabled(True)
            self.ui.pushButtonClose.setEnabled(False)               
    
    def startSkeletonization(self):
        self.startEndSkeletonization(True)
                
    def endSkeletonization(self):
        self.startEndSkeletonization(False)
        self.dock.close()
               
    def startEndSkeletonization(self, start):        
        if(start):            
            self.volume = self.viewer.renderer.getVolume()
            if self.skeletonViewer.loaded:
                self.skeletonViewer.unloadData()
            self.mesh = self.skeletonViewer.renderer.getMesh()
                        
            self.skeletonViewer.setScale(self.viewer.renderer.getSpacingX(), self.viewer.renderer.getSpacingY(), self.viewer.renderer.getSpacingZ())
            self.skeletonViewer.setLocation(self.viewer.renderer.getOriginX(), self.viewer.renderer.getOriginY(), self.viewer.renderer.getOriginZ())            
            
            if self.ui.radioButtonBinary.isChecked():
                medialnessScoringFunction = self.MedialnessScoringFunctionBinary
            elif self.ui.radioButtonGlobalRank.isChecked():
                medialnessScoringFunction = self.MedialnessScoringFunctionGlobalRank
            else :
                medialnessScoringFunction = self.MedialnessScoringFunctionLocalRank 
                       
            self.engine = InteractiveSkeletonEngine(self.volume, self.mesh, self.getStartingDensity(), self.getStepCount(), self.getCurveRadius(), self.getMinCurveLength(), medialnessScoringFunction)
            self.engine.setIsoValue(self.viewer.renderer.getSurfaceValue())
            self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.skeletonClicked)
            self.connect(self.app.mainCamera, QtCore.SIGNAL("cameraChanged()"), self.processCameraChanged)
            self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("thicknessChanged(int)"), self.thicknessChanged)
            self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("modelDrawing()"), self.drawOverlay)
            self.engine.setLineThickness(self.skeletonViewer.lineThickness)
            
            
            self.setSkeletonViewerProperties(True)
            self.skeletonViewer.loaded = True
            self.ui.pushButtonStart.setEnabled(False)
            self.ui.pushButtonClose.setEnabled(True)
            self.skeletonViewer.emitModelLoaded()                        
        else:
            self.disconnect(self.app.viewers["skeleton"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.skeletonClicked)
            self.disconnect(self.app.mainCamera, QtCore.SIGNAL("cameraChanged()"), self.processCameraChanged)
            self.disconnect(self.app.viewers["skeleton"], QtCore.SIGNAL("thicknessChanged(int)"), self.thicknessChanged)
            self.disconnect(self.app.viewers["skeleton"], QtCore.SIGNAL("modelDrawing()"), self.drawOverlay)
            self.engine.finalizeSkeleton()
            del self.engine
            self.setSkeletonViewerProperties(False)
            self.ui.pushButtonStart.setEnabled(True)
            self.ui.pushButtonClose.setEnabled(False)
        self.started = start
        self.skeletonViewer.emitModelChanged()

    def startingDensityChanged(self, newLevel):
        self.ui.labelStartingDensityDisplay.setNum(newLevel/100.0)

    def getStartingDensity(self):
        return self.ui.horizontalSliderStartingDensity.value()/100.0

    def getMedialness(self):
        return self.ui.horizontalSliderMedialness.value()
    
    def getSmoothness(self):
        return self.ui.horizontalSliderSmoothness.value()

    def getSketchPriority(self):
        return self.ui.horizontalSliderSketchPriority.value()

    def getStepCount(self):
        return self.ui.horizontalSliderStepCount.value()

    def getCurveRadius(self):
        return self.ui.spinBoxCurveRadius.value()
    
    def getMinCurveLength(self):
        return self.ui.spinBoxMinCurve.value()   
    
    def setSkeletonViewerProperties(self, opening):
        if(opening):
            #self.oldSkeletonViewerSelectEnabled = self.skeletonViewer.selectEnabled
            #self.oldSkeletonViewerMouseMoveEnabled = self.skeletonViewer.mouseMoveEnabled
            self.oldSkeletonViewerMouseMoveEnabledRay = self.skeletonViewer.mouseMoveEnabledRay
            self.skeletonViewer.setViewerAutonomy(False);
            if self.skeletonViewer.loaded:
                self.skeletonViewer.unloadData()
            self.skeletonViewer.setSelectEnabled(True)
            self.skeletonViewer.setMouseMoveEnabledRay(True)
            
            color = self.app.themes.getColor("Volume:Model:0")
            color.setAlpha(150)
            self.viewer.setModelColor(color)
            #self.skeletonViewer.setMouseMoveEnabled(True)            
        else:
            self.skeletonViewer.setViewerAutonomy(True);
            #self.skeletonViewer.setSelectEnabled(self.oldSkeletonViewerSelectEnabled)
            self.skeletonViewer.setMouseMoveEnabledRay(self.oldSkeletonViewerMouseMoveEnabledRay)
            #self.skeletonViewer.setMouseMoveEnabled(self.oldSkeletonViewerMouseMoveEnabled)               
    
    def filterMouseHits(self, mouseHits):
        hits = list()
        for hit_record in mouseHits:
            minDepth, maxDepth, names = hit_record
            names = list(names)
            if(names[0] == self.skeletonViewer.sceneIndex):
                names.pop(0)
                for i in range(2-len(names)):
                    names.append(-1)
                hits.append(names)
        return hits
                    
    def processClickRay(self, rayWorld, rayWidth, eyeWorld, event):
        ray = self.viewer.worldVectorToObjectCoordinates(rayWorld)        
        eye = self.viewer.worldToObjectCoordinates(eyeWorld)
        divisor =  float(self.getMedialness()) + float(self.getSmoothness());

        medialnessRatio = float(self.getMedialness()) / divisor;
        smoothnessRatio = float(self.getSmoothness()) / divisor;
        sketchPriority = float(self.getSketchPriority());

        if(self.started):
            if((event.modifiers() & QtCore.Qt.CTRL) and (event.modifiers() & QtCore.Qt.ALT)):
                self.engine.selectRootRay(ray[0], ray[1], ray[2], eye[0], eye[1], eye[2], rayWidth, medialnessRatio, smoothnessRatio)
            elif(event.modifiers() & QtCore.Qt.CTRL):
                self.engine.selectEndSeed(medialnessRatio, smoothnessRatio)
                self.engine.selectStartSeedRay(ray[0], ray[1], ray[2], eye[0], eye[1], eye[2], rayWidth, medialnessRatio, smoothnessRatio)                    
                self.skeletonViewer.emitModelChanged()                    
            elif (event.modifiers() & QtCore.Qt.ALT):
                self.engine.selectEndSeed(medialnessRatio, smoothnessRatio)
                self.skeletonViewer.emitModelChanged()
                    
    def processMouseOverRay(self, rayWorld, rayWidth, eyeWorld, event):
        ray = self.viewer.worldVectorToObjectCoordinates(rayWorld)        
        eye = self.viewer.worldToObjectCoordinates(eyeWorld)
        if(self.started and event.modifiers() & QtCore.Qt.CTRL ):
            self.engine.browseStartSeedRay(ray[0], ray[1], ray[2], eye[0], eye[1], eye[2], rayWidth)
            self.engine.analyzePathRay(ray[0], ray[1], ray[2], eye[0], eye[1], eye[2], rayWidth)
            self.skeletonViewer.emitModelChanged()                    
        elif(self.started and event.modifiers() & QtCore.Qt.ALT ):
            self.engine.analyzePathRay(ray[0], ray[1], ray[2], eye[0], eye[1], eye[2], rayWidth)
            self.skeletonViewer.emitModelChanged()
        elif(event.modifiers() & QtCore.Qt.SHIFT):
            self.engine.setSketch2D(self.app.mainCamera.width(), self.app.mainCamera.height(), event.x(), event.y())
            self.engine.setSketchRay(ray[0], ray[1], ray[2], eye[0], eye[1], eye[2], rayWidth)               
            self.skeletonViewer.emitModelChanged()
    
    def processCameraChanged(self):
        if(self.engine.clearSketch2D()) :
            self.skeletonViewer.emitModelChanged()        
    
    def modelLoaded(self):
        self.skeletonViewer = self.app.viewers["skeleton"]; 
        #self.oldSkeletonViewerSelectEnabled = self.skeletonViewer.selectEnabled
        #self.oldSkeletonViewerMouseMoveEnabled = self.skeletonViewer.mouseMoveEnabled
        self.oldSkeletonViewerMouseMoveEnabledRay = self.skeletonViewer.mouseMoveEnabledRay
        #self.connect(self.app.mainCamera, QtCore.SIGNAL("mouseMovedRAW(PyQt_PyObject, QMouseEvent)"), self.processMouseOverMultiple)
        #self.connect(self.app.mainCamera, QtCore.SIGNAL("mouseClickedRAW(PyQt_PyObject, QMouseEvent)"), self.processClickMultiple)

        #self.connect(self.skeletonViewer, QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.processClick)    
        #self.connect(self.skeletonViewer, QtCore.SIGNAL("elementMouseOver (int, int, int, int, int, int, QMouseEvent)"), self.processMouseOver)

        self.connect(self.skeletonViewer, QtCore.SIGNAL("mouseClickRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), self.processClickRay)    
        self.connect(self.skeletonViewer, QtCore.SIGNAL("mouseOverRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), self.processMouseOverRay)  
        
        self.skeletonizeAct.setChecked(False)
        self.skeletonizeAct.setEnabled(True)
        self.showWidget(False)
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderStartingDensity.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderStartingDensity.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        self.ui.horizontalSliderStartingDensity.setValue(defaultDensity)
        if(self.started):
            self.engine.setIsoValue(self.viewer.renderer.getSurfaceValue())   
                   
    def modelUnloaded(self):
        self.skeletonizeAct.setChecked(False)
        self.skeletonizeAct.setEnabled(False)
        self.showWidget(False)
            
    def modelChanged(self):
        if(self.started):
            self.engine.setIsoValue(self.viewer.renderer.getSurfaceValue())
            self.skeletonViewer.emitModelChanged()           
                    
    def dockVisibilityChanged(self, visible):
        self.skeletonizeAct.setChecked(visible)
        self.showWidget(visible)
    
    def clearSkeleton(self):
        self.engine.clearSkeleton()
        self.skeletonViewer.emitModelChanged()
        
    def skeletonClicked(self, hit0, hit1, hit2, hit3, hit4, hit5, event):
        self.engine.addSelectionPoint(hit0, hit1)
        self.skeletonViewer.emitModelChanged()
    
    def keyPressed(self, event):
        if self.started:
            if(event.key() == QtCore.Qt.Key_Control):
                self.engine.startEndPolyLineMode(True)
                self.skeletonViewer.emitModelChanged()                                      
            elif(event.key() == QtCore.Qt.Key_Alt):
                self.engine.startEndSingleRootMode(True)
                self.skeletonViewer.emitModelChanged()
                        
    def keyReleased(self, event):
        if self.started:
            divisor =  float(self.getMedialness()) + float(self.getSmoothness());
            medialnessRatio = float(self.getMedialness()) / divisor;
            smoothnessRatio = float(self.getSmoothness()) / divisor;
            sketchPriority = float(self.getSketchPriority());                
            
            if(event.key() == QtCore.Qt.Key_Control):
                self.engine.startEndPolyLineMode(False)
                self.skeletonViewer.emitModelChanged()                              
            elif(event.key() == QtCore.Qt.Key_Alt):
                self.engine.startEndSingleRootMode(False)
                self.skeletonViewer.emitModelChanged()
            elif(event.key() == QtCore.Qt.Key_Escape):
                self.engine.clearCurrentPath();
                self.skeletonViewer.emitModelChanged()
            elif(event.key() == QtCore.Qt.Key_Shift):
                self.engine.endSketchRay(medialnessRatio, smoothnessRatio, 1.0, sketchPriority)
                self.skeletonViewer.emitModelChanged()
            elif(event.key() == QtCore.Qt.Key_Plus):
                self.engine.selectEndSeed(medialnessRatio, smoothnessRatio)
                self.skeletonViewer.emitModelChanged()
            elif(event.key() == QtCore.Qt.Key_Delete):
                self.engine.deleteSelection();
                self.skeletonViewer.emitModelChanged()
    
    def thicknessChanged(self, value):
        self.engine.setLineThickness(value)
    
    def drawOverlay(self):            
        if self.started:
            manualColors = [self.app.themes.getColor("InteractiveSkeletonizer:StartingPoint"), 
                            self.app.themes.getColor("InteractiveSkeletonizer:EndingPoint"),
                            self.app.themes.getColor("InteractiveSkeletonizer:Sketch"),
                            self.app.themes.getColor("InteractiveSkeletonizer:UnconfirmedCurve"),
                            self.app.themes.getColor("InteractiveSkeletonizer:RemovableCurve")]              
            for i in range(5):
                glPushAttrib(GL_LIGHTING_BIT)
                self.skeletonViewer.setMaterials(manualColors[i])
                self.engine.draw(i)        
                glPopAttrib()
                                 