# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to crop the volume 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.1  2008/11/19 17:53:43  ssa1
#   Removing printfs, and adding cropping
#

from PyQt4 import QtCore, QtGui
from ui_dialog_volume_crop import Ui_DialogVolumeCrop
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

    
class VolumeCropForm(QtGui.QWidget, Ui_DialogVolumeCrop):
    
    def __init__(self, main, volumeViewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.setupUi(self)
        self.viewer = volumeViewer
        self.connectSignals()
        self.createActions()
        self.createMenus()
        
    def connectSignals(self):
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelDrawing()"), self.modelDrawing)
        self.connect(self.spinBoxTopLeftX, QtCore.SIGNAL("valueChanged(int)"), self.boxChanged)
        self.connect(self.spinBoxTopLeftY, QtCore.SIGNAL("valueChanged(int)"), self.boxChanged)
        self.connect(self.spinBoxTopLeftZ, QtCore.SIGNAL("valueChanged(int)"), self.boxChanged)
        self.connect(self.spinBoxBottomRightX, QtCore.SIGNAL("valueChanged(int)"), self.boxChanged)
        self.connect(self.spinBoxBottomRightY, QtCore.SIGNAL("valueChanged(int)"), self.boxChanged)
        self.connect(self.spinBoxBottomRightZ, QtCore.SIGNAL("valueChanged(int)"), self.boxChanged)
        
        
    def createActions(self):               
        cropAct = QtGui.QAction(self.tr("&Crop"), self)
        cropAct.setStatusTip(self.tr("Crop the volume"))
        self.connect(cropAct, QtCore.SIGNAL("triggered()"), self.show)
        self.app.actions.addAction("crop_Volume", cropAct)   
  
    def createMenus(self):
        self.app.menus.addAction("actions-volume-crop", self.app.actions.getAction("crop_Volume"), "actions-volume");        

    def modelLoaded(self):
        self.spinBoxTopLeftX.setValue(0)
        self.spinBoxTopLeftY.setValue(0)
        self.spinBoxTopLeftZ.setValue(0)
        self.spinBoxTopLeftX.setMinimum(0)
        self.spinBoxTopLeftY.setMinimum(0)
        self.spinBoxTopLeftZ.setMinimum(0)
        self.spinBoxTopLeftX.setMaximum(self.viewer.renderer.getMax(0))
        self.spinBoxTopLeftY.setMaximum(self.viewer.renderer.getMax(1))
        self.spinBoxTopLeftZ.setMaximum(self.viewer.renderer.getMax(2))
        
        self.spinBoxBottomRightX.setValue(self.viewer.renderer.getMax(0))
        self.spinBoxBottomRightY.setValue(self.viewer.renderer.getMax(1))
        self.spinBoxBottomRightZ.setValue(self.viewer.renderer.getMax(2))
        self.spinBoxBottomRightX.setMinimum(0)
        self.spinBoxBottomRightY.setMinimum(0)
        self.spinBoxBottomRightZ.setMinimum(0)
        self.spinBoxBottomRightX.setMaximum(self.viewer.renderer.getMax(0))
        self.spinBoxBottomRightY.setMaximum(self.viewer.renderer.getMax(1))
        self.spinBoxBottomRightZ.setMaximum(self.viewer.renderer.getMax(2))        
    
    def modelUnloaded(self):
        self.close()
        
    def boxChanged(self, dummy):
        self.app.mainCamera.updateGL()
        
    def modelDrawing(self):
        if self.isVisible():
            minPts = [self.spinBoxTopLeftX.value(), self.spinBoxTopLeftY.value(), self.spinBoxTopLeftZ.value()]
            maxPts = [self.spinBoxBottomRightX.value(), self.spinBoxBottomRightY.value(), self.spinBoxBottomRightZ.value()]
            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            glColor4f(1.0, 0.0, 0.0, 0.0)
            glPushMatrix();
            
            glTranslatef(minPts[0]+(maxPts[0]-minPts[0])/2.0, minPts[1]+(maxPts[1]-minPts[1])/2.0, minPts[2]+(maxPts[2]-minPts[2])/2.0);
            glScalef(maxPts[0]-minPts[0], maxPts[1]-minPts[1], maxPts[2]-minPts[2]);
            glutWireCube(1.0);
            glPopMatrix();
            glPopAttrib();
            
          
    def accept(self):
        if(self.viewer.loaded):
            minPts = [self.spinBoxTopLeftX.value(), self.spinBoxTopLeftY.value(), self.spinBoxTopLeftZ.value()]
            maxPts = [self.spinBoxBottomRightX.value(), self.spinBoxBottomRightY.value(), self.spinBoxBottomRightZ.value()]
            self.viewer.renderer.cropVolume(minPts[0], minPts[1], minPts[2], maxPts[0], maxPts[1], maxPts[2])
            self.close();     
            self.viewer.surfaceEditor.modelLoadedPreDraw()
            self.viewer.emitModelChanged();      
        else:
            QtGui.QMessageBox.critical(None, "Source volume unloaded", "A volume must be loaded to perform skeletonization", QtGui.QMessageBox.Ok, QtGui.QMessageBox.NoButton)
    
    def reject(self):
        self.close()