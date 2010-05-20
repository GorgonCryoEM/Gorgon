# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to fit a calpha backbone to the density

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
from ui_dialog_calpha_flexible_fitting import Ui_DialogCAlphaFlexibleFitting
from base_dock_widget import BaseDockWidget

class CAlphaFlexibleFittingForm(BaseDockWidget, Ui_DialogCAlphaFlexibleFitting):
        
    def __init__(self, main, viewer, parent=None):
        BaseDockWidget.__init__(self, 
                                main, 
                                "Fit to density", 
                                "Fit CAlpha atoms to density", 
                                "perform_CAlphaFlexibleFitting", 
                                "actions-calpha-flexiblefitting", 
                                "actions-calpha", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.RightDockWidgetArea, 
                                parent)
        self.app = main
        self.viewer = viewer
        self.cAlphaViewer = viewer
        self.volumeViewer = self.app.viewers["volume"]
        self.sseViewer = self.app.viewers["sse"]        
        self.createUI()
        self.enableDisableWindowElements()

    def createUI(self):
        self.setupUi(self)    
        self.connect(self.pushButtonLoadVolume, QtCore.SIGNAL("clicked (bool)"), self.loadVolume)
        self.connect(self.pushButtonLoadHelices, QtCore.SIGNAL("clicked (bool)"), self.loadHelices)
        self.connect(self.pushButtonLoadCAlpha, QtCore.SIGNAL("clicked (bool)"), self.loadBackbone)
        self.connect(self.cAlphaViewer, QtCore.SIGNAL("modelLoaded()"), self.enableDisableWindowElements)
        self.connect(self.cAlphaViewer, QtCore.SIGNAL("modelUnloaded()"), self.enableDisableWindowElements)
        self.connect(self.volumeViewer, QtCore.SIGNAL("modelLoaded()"), self.enableDisableWindowElements)
        self.connect(self.volumeViewer, QtCore.SIGNAL("modelUnloaded()"), self.enableDisableWindowElements)
        self.connect(self.sseViewer, QtCore.SIGNAL("modelLoaded()"), self.enableDisableWindowElements)
        self.connect(self.sseViewer, QtCore.SIGNAL("modelUnloaded()"), self.enableDisableWindowElements)
            
    def enableDisableWindowElements(self):
        self.pushButtonLoadVolume.setVisible(not self.volumeViewer.loaded)
        self.pushButtonLoadHelices.setVisible(not self.sseViewer.loaded)
        self.pushButtonLoadCAlpha.setVisible(not self.cAlphaViewer.loaded)
        allLoaded = self.volumeViewer.loaded and self.sseViewer.loaded and self.cAlphaViewer.loaded
        self.groupBoxAdvancedSettings.setEnabled(allLoaded)
        self.buttonBox.setEnabled(allLoaded)
        self.radioButtonFlexibleFitting.setEnabled(allLoaded)
        self.radioButtonRigidFitting.setEnabled(allLoaded)      

    def loadVolume(self, temp):
        self.app.actions.getAction("load_Volume").trigger()
        self.bringToFront()

    def loadHelices(self, temp):
        self.app.actions.getAction("load_SSE_Helix").trigger()
        self.bringToFront()

    def loadBackbone(self, temp):
        self.app.actions.getAction("load_CAlpha").trigger()
        self.bringToFront()

    def accept(self):
        pass
    
    def reject(self):
        self.showWidget(False)