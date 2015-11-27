# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget used to perform manual skeletonization of a volume 


from PyQt4 import QtCore, QtGui
from libpyGORGON import InteractiveLoopBuilderEngine, PDBAtom, Vector3DFloat, PDBBond
from ui_dialog_volume_manual_skeletonization import Ui_DialogVolumeManualSkeletonization
from volume_manual_skeletonization_form import VolumeManualSkeletonizationForm
from seq_model import Coil

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

class CAlphaInteractiveLoopBuilder(VolumeManualSkeletonizationForm):
    
    def __init__(self, main, chain, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.chain = chain
        self.app.themes.addDefaultRGB("InteractiveLoopBuilder:StartingPoint", 0, 0, 255, 150)
        self.app.themes.addDefaultRGB("InteractiveLoopBuilder:EndingPoint", 0, 255, 0, 150)         
        self.app.themes.addDefaultRGB("InteractiveLoopBuilder:Sketch", 0, 128, 0, 255)
        self.started = False
        self.sketchStarted = False
        self.viewer = self.app.viewers['volume']
        self.skeletonViewer = self.app.viewers["skeleton"];
        self.calphaViewer = self.app.viewers["calpha"] 
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)        
        
        self.isSkeletonClicked = False
        self.createUI()

        self.oldSkeletonViewerMouseMoveEnabledRay = self.skeletonViewer.mouseMoveEnabledRay
        self.connect(self.skeletonViewer, QtCore.SIGNAL("mouseClickRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), self.processClickRay)    
        self.connect(self.skeletonViewer, QtCore.SIGNAL("mouseOverRay(PyQt_PyObject, float, PyQt_PyObject, QMouseEvent)"), self.processMouseOverRay)  
        self.connect(self.skeletonViewer, QtCore.SIGNAL("modelChanged()"), self.skeletonChanged)
        
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderStartingDensity.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderStartingDensity.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        
        self.ui.horizontalSliderStartingDensity.setValue(defaultDensity)
        self.startIndex = 0
        self.endIndex = 0
        
        self.startSkeletonization()
        
    def __del__(self):
        self.engine.finishLoopBuilding()
        self.skeletonViewer.unloadData()
        self.endSkeletonization()
        
    def endSkeletonization(self):
        self.startEndSkeletonization(False)            
        
    def createUI(self):
        self.ui = Ui_DialogVolumeManualSkeletonization()
        self.ui.setupUi(self)    
        self.connect(self.ui.horizontalSliderStartingDensity,QtCore.SIGNAL("valueChanged(int)"),self.startingDensityChanged)     
        self.connect(self.ui.pushButtonStart,QtCore.SIGNAL("pressed ()"),self.startSkeletonization)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.endSkeletonization)   
        self.connect(self.ui.pushButtonClear, QtCore.SIGNAL("pressed ()"), self.clearSkeleton)        
        self.connect(self.app, QtCore.SIGNAL("keyReleased(QKeyEvent)"), self.keyReleased)
        self.connect(self.app, QtCore.SIGNAL("keyPressed(QKeyEvent)"), self.keyPressed)
                                  

    def initializeEngine(self, medialnessScoringFunction):
        self.engine = InteractiveLoopBuilderEngine(self.volume, self.mesh, self.calphaViewer.renderer, self.getStartingDensity(), self.getStepCount(), self.getCurveRadius(), self.getMinCurveLength(), medialnessScoringFunction)
        self.engine.setTranslation(self.viewer.renderer.getOriginX(), self.viewer.renderer.getOriginY(), self.viewer.renderer.getOriginZ())
        self.engine.setScaling(self.viewer.renderer.getSpacingX(), self.viewer.renderer.getSpacingY(), self.viewer.renderer.getSpacingZ())
                                            
    def modelLoaded(self):
        pass
    
    def skeletonChanged(self):
        self.calphaViewer.emitModelChanged()
    
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
    
    
    def setLoopAtoms(self, startIndex, endIndex):
        
        self.engine.clearAtomList()
        self.engine.clearCurrentPath()
                
        for i in range(startIndex, endIndex+1):
            if(i in self.chain.residueRange()) :
                atom = self.chain[i].getAtom('CA')
                if(not atom):
                    raw = PDBAtom(self.chain.getPdbID(), self.chain.getChainID(), i, 'CA')
                    raw.setPosition(Vector3DFloat(0,0,0))
                    atom = self.calphaViewer.renderer.addAtom(raw)
                    atom.setVisible(False)                 
                    print atom
                    self.chain[i].addAtomObject(atom)
                    self.chain[i].setCAlphaColorToDefault() 
                    bondBefore = None
                    bondAfter = None
                    if i - 1 in self.chain.residueRange():
                        prevCAlpha = self.chain[i - 1].getAtom('CA')
                        if prevCAlpha:
                            print "adding a bond before"
                            bondBefore=PDBBond()
                            bondBefore.setAtom0Ix(prevCAlpha.getHashKey())
                            bondBefore.setAtom1Ix(atom.getHashKey())
                    if i + 1 in self.chain.residueRange():
                        nextCAlpha = self.chain[i + 1].getAtom('CA')
                        if nextCAlpha:
                            print "adding a bond after"
                            bondAfter = PDBBond()
                            bondAfter.setAtom0Ix(nextCAlpha.getHashKey())
                            bondAfter.setAtom1Ix(atom.getHashKey())
                    
                    if bondBefore:
                        self.calphaViewer.renderer.addBond(bondBefore)
                    if bondAfter:
                        self.calphaViewer.renderer.addBond(bondAfter)
                        
                self.engine.addAtom(atom.getHashKey())             
            
        if not self.calphaViewer.loaded:
            self.calphaViewer.loaded = True
            self.calphaViewer.emitModelLoaded()
        else:
            self.calphaViewer.emitModelChanged()        

        
    def drawOverlay(self):            
        if self.started:
            manualColors = [self.app.themes.getColor("InteractiveLoopBuilder:StartingPoint"), 
                            self.app.themes.getColor("InteractiveLoopBuilder:EndingPoint"),
                            self.app.themes.getColor("InteractiveLoopBuilder:Sketch")]              
            for i in range(3):
                glPushAttrib(GL_LIGHTING_BIT)
                self.skeletonViewer.setMaterials(manualColors[i])
                self.engine.draw(i)        
                glPopAttrib()                
