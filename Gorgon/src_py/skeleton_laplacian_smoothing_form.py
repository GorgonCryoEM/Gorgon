from PyQt4 import QtCore, QtGui
from ui_dialog_skeleton_laplacian_smoothing import Ui_DialogSkeletonLaplacianSmoothing

class SkeletonLaplacianSmoothingForm(QtGui.QWidget):
    def __init__(self, main, skeletonViewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = skeletonViewer
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.createUI()
        self.createActions()
        self.createMenus()

    def createUI(self):
        self.ui = Ui_DialogSkeletonLaplacianSmoothing()
        self.ui.setupUi(self)       
        self.dock = QtGui.QDockWidget(self.tr("Skeleton - Laplacian Smoothing"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        
    def loadWidget(self):
        if(self.app.actions.getAction("perform_SkeletonLaplacianSmoothing").isChecked()) :
            self.app.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.dock)
            self.dock.show()
        else:
            self.app.removeDockWidget(self.dock)
            
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("perform_SkeletonLaplacianSmoothing").setChecked(visible)
    
    def modelChanged(self):
        if(not self.viewer.loaded) and self.app.actions.getAction("perform_SkeletonLaplacianSmoothing").isChecked():
            self.app.actions.getAction("perform_SkeletonLaplacianSmoothing").trigger()        
        
    def createActions(self):               
        smoothingAct = QtGui.QAction(self.tr("&Laplacian Smoothing"), self)
        smoothingAct.setStatusTip(self.tr("Perform laplacian smoothing"))
        smoothingAct.setCheckable(True)
        smoothingAct.setChecked(False)
        self.connect(smoothingAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("perform_SkeletonLaplacianSmoothing", smoothingAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-skeleton-laplacianSmoothing", self.app.actions.getAction("perform_SkeletonLaplacianSmoothing"), "actions-skeleton")        
    
    def accept(self):
        self.viewer.renderer.smoothLaplacian(self.ui.doubleSpinBoxConvergenceRate.value(), self.ui.spinBoxNoOfIterations.value())
        self.viewer.emitModelChanged()
        
    def reject(self):
        self.app.actions.getAction("perform_SkeletonLaplacianSmoothing").trigger()
        