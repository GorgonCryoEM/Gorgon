# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget can be used to perform laplacian smoothing on a volume 



from PyQt4 import QtCore, QtGui
from ui_dialog_skeleton_laplacian_smoothing import Ui_DialogSkeletonLaplacianSmoothing
from base_dock_widget import BaseDockWidget

class VolumeLaplacianSmoothingForm(BaseDockWidget):
    def __init__(self, main, volumeViewer, parent=None):
        BaseDockWidget.__init__(self, 
                               main, 
                               "&Laplacian Smoothing", 
                               "Perform laplacian smoothing", 
                               "perform_VolumeLaplacianSmoothing", 
                               "actions-volume-laplacianSmoothing", 
                               "actions-volume", 
                               QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                               QtCore.Qt.RightDockWidgetArea, 
                               parent)
        self.app = main
        self.viewer = volumeViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.createUI()
        self.createActions()

    def createUI(self):
        self.ui = Ui_DialogSkeletonLaplacianSmoothing()
        self.ui.setupUi(self)       
            
    def modelChanged(self):
        if(not self.viewer.loaded) and self.app.actions.getAction("perform_VolumeLaplacianSmoothing").isChecked():
            self.app.actions.getAction("perform_VolumeLaplacianSmoothing").trigger()        
        
    def createActions(self):   
        self.displayAct.setEnabled(False)            
  
    def createMenus(self):
        self.app.menus.addAction("actions-volume-laplacianSmoothing", self.app.actions.getAction("perform_VolumeLaplacianSmoothing"), "actions-volume")        
    
    def accept(self):
        self.setCursor(QtCore.Qt.BusyCursor)
        self.viewer.renderer.performSmoothLaplacian(self.ui.doubleSpinBoxConvergenceRate.value(), self.ui.spinBoxNoOfIterations.value())
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.emitModelChanged()
        
    def reject(self):
        self.app.actions.getAction("perform_VolumeLaplacianSmoothing").trigger()
        
    def modelLoaded(self):
        self.displayAct.setEnabled(True)
    
    def modelUnloaded(self):
        self.displayAct.setEnabled(False)
        
