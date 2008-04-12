from PyQt4 import QtCore, QtGui
from ui_dialog_volume_manual_skeletonization import Ui_DialogVolumeManualSkeletonization
from gorgon_cpp_wrapper import InteractiveSkeletonEngine

class VolumeManualSkeletonizationForm(QtGui.QWidget):
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.started = False
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
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.dock.close)
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.horizontalSliderStartingDensity,QtCore.SIGNAL("valueChanged(int)"),self.startingDensityChanged)     
        self.connect(self.ui.pushButtonStart,QtCore.SIGNAL("pressed ()"),self.startSkeletonization)   
                                            
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
               
    def startEndSkeletonization(self, start):
        self.started = start
        if(start):
            self.volume = self.viewer.renderer.getVolume()
            self.mesh = self.skeletonViewer.renderer.getMesh()
            
            skeletonRatio = float(self.getMedialness())/float(self.getMedialness()+self.getSmoothness())
            stRatio = float(self.getSmoothness())/float(self.getMedialness()+self.getSmoothness())
            self.engine = InteractiveSkeletonEngine(self.volume, self.mesh, skeletonRatio, stRatio, self.getStartingDensity(), self.getStepCount(), self.getCurveRadius(), self.getMinCurveLength())
            self.setSkeletonViewerProperties(True)
            self.skeletonViewer.loaded = True
            self.ui.pushButtonStart.setEnabled(False)
            self.ui.pushButtonClose.setEnabled(True)
            self.skeletonViewer.emitModelLoaded()                        
        else:
            self.setSkeletonViewerProperties(False)
            self.ui.pushButtonStart.setEnabled(True)
            self.ui.pushButtonClose.setEnabled(False)      

    def startingDensityChanged(self, newLevel):
        self.ui.labelStartingDensityDisplay.setNum(newLevel/100.0)

    def getStartingDensity(self):
        return self.ui.horizontalSliderStartingDensity.value()/100.0

    def getMedialness(self):
        return self.ui.horizontalSliderMedialness.value()
    
    def getSmoothness(self):
        return self.ui.horizontalSliderSmoothness.value()

    def getStepCount(self):
        return self.ui.horizontalSliderStepCount.value()

    def getCurveRadius(self):
        return self.ui.spinBoxCurveRadius.value()
    
    def getMinCurveLength(self):
        return self.ui.spinBoxMinCurve.value()   
    
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
    
    def processClick(self, h0, h1, h2, h3, h4):
        if(self.started and h0 >= 0 and h1 >= 0):
            print "a"
            self.engine.selectSeed(h0, h1)
            pass
        
    def processMouseOver(self, h0, h1, h2, h3, h4):
        if(self.started and h0 >= 0 and h1 >= 0):
            self.engine.analyzePath(h0, h1)
            self.skeletonViewer.emitModelChanged()
            pass
    
    def modelLoaded(self):
        self.skeletonViewer = self.app.viewers["skeleton"]; 
        self.oldSkeletonViewerSelectEnabled = self.skeletonViewer.selectEnabled
        self.oldSkeletonViewerMouseMoveEnabled = self.skeletonViewer.mouseMoveEnabled  
        self.connect(self.skeletonViewer, QtCore.SIGNAL("elementSelected (int, int, int, int, int)"), self.processClick)    
        self.connect(self.skeletonViewer, QtCore.SIGNAL("elementMouseOver (int, int, int, int, int)"), self.processMouseOver)  
        self.skeletonizeAct.setChecked(False)
        self.skeletonizeAct.setEnabled(True)
        self.showWidget(False)
        maxDensity = self.viewer.renderer.getMaxDensity()
        minDensity = self.viewer.renderer.getMinDensity()
        self.ui.horizontalSliderStartingDensity.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderStartingDensity.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        self.ui.horizontalSliderStartingDensity.setValue(defaultDensity)        
                   
    def modelUnloaded(self):
        self.skeletonizeAct.setChecked(False)
        self.skeletonizeAct.setEnabled(False)
        self.showWidget(False)    
                    
    def dockVisibilityChanged(self, visible):
        self.skeletonizeAct.setChecked(visible)
        self.showWidget(visible)
                                 