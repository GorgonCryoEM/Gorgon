# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget used to perform grayscale skeletonization on a volume 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.3  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from ui_dialog_volume_grayscale_skeletonization import Ui_DialogVolumeGrayscaleSkeletonization
from delayed_filter import DelayedFilter
import threading


class VolumeGrayscaleSkeletonizationForm(QtGui.QDialog):
    def __init__(self, main, volumeViewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = volumeViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.createUI()
        self.createActions()
        self.createMenus()

    def createUI(self):
        self.ui = Ui_DialogVolumeGrayscaleSkeletonization()
        self.ui.setupUi(self)
        self.connect(self.ui.horizontalSliderStartingDensity,QtCore.SIGNAL("valueChanged(int)"),self.startingDensityChanged)
        self.connect(self.ui.comboBoxMethod, QtCore.SIGNAL("currentIndexChanged (int)"), self.methodChanged)                                            
        self.methodChanged(0)
        
    def createActions(self):               
        graySkeletonizeAct = QtGui.QAction(self.tr("&Grayscale Skeletonization"), self)
        graySkeletonizeAct.setStatusTip(self.tr("Apply grayscale skeletonization on the volume"))
        self.connect(graySkeletonizeAct, QtCore.SIGNAL("triggered()"), self.loadAndShow)        
        self.app.actions.addAction("perform_VolumeGrayscaleSkeletonization", graySkeletonizeAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-volume-skeletonization-grayscale", self.app.actions.getAction("perform_VolumeGrayscaleSkeletonization"), "actions-volume-skeletonization")        

    def modelLoaded(self):
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderStartingDensity.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderStartingDensity.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        self.ui.horizontalSliderStartingDensity.setValue(defaultDensity)
    
    def modelUnloaded(self):
        self.close()

    def startingDensityChanged(self, newLevel):
        self.ui.labelStartingDensityDisplay.setNum(newLevel/100.0)

    def getStartingDensity(self):
        return self.ui.horizontalSliderStartingDensity.value()/100.0

    def getStepCount(self):
        return self.ui.horizontalSliderStepCount.value()
    
    def getSkeletonizationMethod(self):
        return self.ui.comboBoxMethod.currentIndex()
    
    def getMinCurveLength(self):
        return self.ui.spinBoxMinCurve.value()

    def getMinSurfaceSize(self):
        return self.ui.spinBoxMinSurface.value()
    
    def getCurveRadius(self):
        return self.ui.spinBoxCurveRadius.value()
    
    def getSurfaceRadius(self):
        return self.ui.spinBoxSurfaceRadius.value()
    
    def getSkeletonRadius(self):
        return self.ui.spinBoxSkeletonRadius.value()
    
    def accept(self):
        if(self.viewer.loaded):
            self.setCursor(QtCore.Qt.BusyCursor)
            method = self.getSkeletonizationMethod()
            if(method == 0):
                if(self.app.viewers["skeleton"].loaded and self.ui.checkBoxPreserveSkeleton.isChecked()):
                    skeletonRenderer = self.app.viewers["skeleton"].renderer
                    skeleton = self.viewer.renderer.performPreservingGrayscaleSkeletonizationAbeysinghe2008(skeletonRenderer.getMesh(), self.getStartingDensity(), self.getStepCount(), self.getMinCurveLength(), self.getMinSurfaceSize(), self.getCurveRadius(), self.getSurfaceRadius(), self.getSkeletonRadius())
                else:
                    skeleton = self.viewer.renderer.performGrayscaleSkeletonizationAbeysinghe2008(self.getStartingDensity(), self.getStepCount(), self.getMinCurveLength(), self.getMinSurfaceSize(), self.getCurveRadius(), self.getSurfaceRadius(), self.getSkeletonRadius())
                self.app.viewers["skeleton"].loadVolume(skeleton)
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.close()
        else:
            QtGui.QMessageBox.critical(None, "Source volume unloaded", "A volume must be loaded to perform skeletonization", QtGui.QMessageBox.Ok, QtGui.QMessageBox.NoButton)
        
    
    def getCitationHtml(self, title, author, journal):
        return "<b>" + title + "</b><br>" + author + "<br><i>" + journal + "</i>"
                  
    def loadAndShow(self):
        self.ui.checkBoxPreserveSkeleton.setEnabled(self.app.viewers["skeleton"].loaded)
        self.show()
        
    def methodChanged(self, id):
        if(id == 0):
            self.ui.textCitation.setHtml(self.getCitationHtml("Segmentation-free skeletonization of grayscale volumes for shape understanding", "Sasakthi Abeysinghe, Matthew Baker, Wah Chiu and Tao Ju", "IEEE International Conference on Shape Modeling and Applications, 2008 (Accepted)"))
        else :
            self.ui.textCitation.setHtml("")