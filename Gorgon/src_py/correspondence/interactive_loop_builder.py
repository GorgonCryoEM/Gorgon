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

from PyQt4 import QtCore, QtGui
from libpyGORGON import InteractiveLoopBuilderEngine
from ui_dialog_volume_manual_skeletonization import Ui_DialogVolumeManualSkeletonization
from volume_manual_skeletonization_form import VolumeManualSkeletonizationForm

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

class InteractiveLoopBuilder(VolumeManualSkeletonizationForm):
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:StartingPoint", 0, 0, 255, 255)
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:EndingPoint", 0, 255, 0, 255)         
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:Sketch", 0, 128, 0, 255)
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:UnconfirmedCurve", 255, 255, 0, 255)
        self.app.themes.addDefaultRGB("InteractiveSkeletonizer:RemovableCurve", 100, 255, 100, 255)         
        self.started = False
        self.sketchStarted = False
        self.viewer = self.app.viewers['volume']
        self.skeletonViewer = self.app.viewers["skeleton"]; 
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)        
        
        self.isSkeletonClicked = False
        self.createUI()

        self.oldSkeletonViewerMouseMoveEnabledRay = self.skeletonViewer.mouseMoveEnabledRay
        self.connect(self.skeletonViewer, QtCore.SIGNAL("mouseClickRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), self.processClickRay)    
        self.connect(self.skeletonViewer, QtCore.SIGNAL("mouseOverRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), self.processMouseOverRay)  
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderStartingDensity.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderStartingDensity.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        
        self.ui.horizontalSliderStartingDensity.setValue(defaultDensity)
        self.loopAtomCount = 0

        
        self.startSkeletonization()
        
    def __del__(self):
        self.endSkeletonization()
        
    def createUI(self):
        self.ui = Ui_DialogVolumeManualSkeletonization()
        self.ui.setupUi(self)    
        self.connect(self.ui.horizontalSliderStartingDensity,QtCore.SIGNAL("valueChanged(int)"),self.startingDensityChanged)     
        self.connect(self.ui.pushButtonStart,QtCore.SIGNAL("pressed ()"),self.startSkeletonization)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.endSkeletonization)   
        self.connect(self.ui.pushButtonClear, QtCore.SIGNAL("pressed ()"), self.clearSkeleton)        
        self.connect(self.app, QtCore.SIGNAL("keyReleased(QKeyEvent)"), self.keyReleased)
        self.connect(self.app, QtCore.SIGNAL("keyPressed(QKeyEvent)"), self.keyPressed)
                                  

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
                       
            self.engine = InteractiveLoopBuilderEngine(self.volume, self.mesh, self.getStartingDensity(), self.getStepCount(), self.getCurveRadius(), self.getMinCurveLength(), medialnessScoringFunction)
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
                                            
    def modelLoaded(self):
        pass
    
    def setLoopAtomCount(self, count):
        self.loopAtomCount = count
