# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: A widget used to perform manual skeletonization of a volume

from PyQt4 import QtCore, QtGui
from ui_dialog_volume_manual_skeletonization import Ui_DialogVolumeManualSkeletonization
from libpyGORGON import InteractiveSkeletonEngine

try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
except ImportError:
    app = QtGui.QApplication(sys.argv)
    QtGui.QMessageBox.critical(None, "Gorgon", "PyOpenGL must be installed to run Gorgon.", QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
    sys.exit(1)

class VolumeManualSkeletonizationForm(QtGui.QWidget):
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.started = False
        self.viewer = viewer
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.manualColors = [QtGui.QColor.fromRgba(QtGui.qRgba(0, 0, 255, 255)),
                             QtGui.QColor.fromRgba(QtGui.qRgba(0, 255, 0, 255)),
                             QtGui.QColor.fromRgba(QtGui.qRgba(0, 255, 255, 255)),
                             QtGui.QColor.fromRgba(QtGui.qRgba(255, 0, 0, 255))]
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
        self.started = start
        if(start):
            self.volume = self.viewer.renderer.getVolume()
            self.mesh = self.skeletonViewer.renderer.getMesh()
            
            skeletonRatio = float(self.getMedialness())/float(self.getMedialness()+self.getSmoothness())
            stRatio = float(self.getSmoothness())/float(self.getMedialness()+self.getSmoothness())
            self.engine = InteractiveSkeletonEngine(self.volume, self.mesh, skeletonRatio, stRatio, self.getStartingDensity(), self.getStepCount(), self.getCurveRadius(), self.getMinCurveLength())
            self.engine.setIsoValue(self.viewer.renderer.getSurfaceValue())
            self.setSkeletonViewerProperties(True)
            self.skeletonViewer.loaded = True
            self.ui.pushButtonStart.setEnabled(False)
            self.ui.pushButtonClose.setEnabled(True)
            self.skeletonViewer.emitModelLoaded()                        
        else:
            self.engine.finalizeSkeleton()
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
                
    
    def processClickMultiple(self, mouseHits, event):
        hits = self.filterMouseHits(mouseHits)
        
        if(self.started):
            if(event.modifiers() & QtCore.Qt.CTRL):
                for i in range(len(hits)):
                    start = (i == 0)
                    end = (i == len(hits)-1)                                    
                    self.engine.selectStartSeedMultiple(hits[i][0], hits[i][1], start, end)                    
                self.skeletonViewer.emitModelChanged()                    
            elif (event.modifiers() & QtCore.Qt.ALT):
                self.engine.selectEndSeed(-1, -1)
                self.skeletonViewer.emitModelChanged()        
                
    
    def processClick(self, h0, h1, h2, h3, h4, h5, event):
        print "single click"
        if(self.started):
            if(event.modifiers() & QtCore.Qt.CTRL):
                self.engine.selectStartSeedMultiple(h0, h1, True, True)
                self.skeletonViewer.emitModelChanged()
            elif (event.modifiers() & QtCore.Qt.ALT):
                self.engine.selectEndSeed(h0, h1)
                self.skeletonViewer.emitModelChanged()
                


    def processMouseOverMultiple(self, mouseHits, event):
        hits = self.filterMouseHits(mouseHits)
                
        if(self.started and event.modifiers() & QtCore.Qt.ALT ):
            for i in range(len(hits)):
                if(hits[i][0] >= 0 and hits[i][1] >= 0):
                    start = (i == 0)
                    end = (i == len(hits)-1)                        
                    self.engine.analyzePathMultiple(hits[i][0], hits[i][1], start, end)
            self.skeletonViewer.emitModelChanged()        
               
       
    def processMouseOver(self, h0, h1, h2, h3, h4, h5, event):
        print "single over"
        if(self.started and h0 >= 0 and h1 >= 0):
            if(event.modifiers() & QtCore.Qt.ALT ):
                self.engine.analyzePathMultiple(h0, h1, True, True)
                self.skeletonViewer.emitModelChanged()
    
    def modelLoaded(self):
        self.skeletonViewer = self.app.viewers["skeleton"]; 
        self.oldSkeletonViewerSelectEnabled = self.skeletonViewer.selectEnabled
        self.oldSkeletonViewerMouseMoveEnabled = self.skeletonViewer.mouseMoveEnabled
        self.connect(self.app.mainCamera, QtCore.SIGNAL("mouseMovedRAW(PyQt_PyObject, QMouseEvent)"), self.processMouseOverMultiple)
        self.connect(self.app.mainCamera, QtCore.SIGNAL("mouseClickedRAW(PyQt_PyObject, QMouseEvent)"), self.processClickMultiple)

        #self.connect(self.skeletonViewer, QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.processClick)    
        #self.connect(self.skeletonViewer, QtCore.SIGNAL("elementMouseOver (int, int, int, int, int, int, QMouseEvent)"), self.processMouseOver)  
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
        
    def drawOverlay(self):     
        if self.started:
            for i in range(4):
                self.skeletonViewer.setMaterials(self.manualColors[i])
                self.engine.draw(i)        
                                 