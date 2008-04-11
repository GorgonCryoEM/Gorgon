from PyQt4 import QtCore, QtGui
from ui_dialog_volume_manual_skeletonization import Ui_DialogVolumeManualSkeletonization

class VolumeManualSkeletonizationForm(QtGui.QWidget):
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = viewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)          
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
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.dock.show()
            self.setSkeletonViewerProperties(True)
        else:
             self.app.removeDockWidget(self.dock)
             self.setSkeletonViewerProperties(False)         
    
    def setSkeletonViewerProperties(self, opening):
        if(opening):
            self.oldSkeletonViewerSelectEnabled = self.skeletonViewer.selectEnabled
            self.oldSkeletonViewerMouseMoveEnabled = self.skeletonViewer.mouseMoveEnabled
            self.skeletonViewer.setViewerAutonomy(False);
            if self.skeletonViewer.loaded:
                self.skeletonViewer.unloadData()
            self.skeletonViewer.setSelectEnabled(True)
            self.skeletonViewer.setMouseMoveEnabled(True)            
        else:
            self.skeletonViewer.setViewerAutonomy(True);
            self.skeletonViewer.setSelectEnabled(self.oldSkeletonViewerSelectEnabled)
            self.skeletonViewer.setMouseMoveEnabled(self.oldSkeletonViewerMouseMoveEnabled)               
    
    def modelLoaded(self):
        self.skeletonViewer = self.app.viewers["skeleton"]; 
        self.oldSkeletonViewerSelectEnabled = self.skeletonViewer.selectEnabled
        self.oldSkeletonViewerMouseMoveEnabled = self.skeletonViewer.mouseMoveEnabled        
        self.skeletonizeAct.setChecked(False)
        self.skeletonizeAct.setEnabled(True)
        self.showWidget(False)
    
    def modelUnloaded(self):
        self.skeletonizeAct.setChecked(False)
        self.skeletonizeAct.setEnabled(False)
        self.showWidget(False)    
                    
    def dockVisibilityChanged(self, visible):
        self.skeletonizeAct.setChecked(visible)
        self.setSkeletonViewerProperties(False)
                                 