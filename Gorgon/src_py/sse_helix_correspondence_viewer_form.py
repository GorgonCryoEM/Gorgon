# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: A widget that can be used to view correspondence between helices


from PyQt4 import QtCore, QtGui
from ui_dialog_sse_helix_correspondence_viewer import Ui_DialogSSEHelixCorrespondenceViewer
from libpyGORGON import SSECorrespondenceEngine, SSECorrespondenceResult

class SSEHelixCorrespondenceFinderForm(QtGui.QWidget):   
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.executed = False
        self.app = main
        self.viewer = viewer        
        self.createUI()
        self.createActions()
        self.createMenus()        

    def createUI(self):
        self.ui = Ui_DialogSSEHelixCorrespondenceFinder()
        self.ui.setupUi(self)       
        self.dock = QtGui.QDockWidget(self.tr("SSE - Helix Correspondence Viewer"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
            
    def loadDefaults(self):
        pass
            
    def loadWidget(self):
        if(self.app.actions.getAction("view_SSEHelixCorrespondences").isChecked()) :
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.loadDefaults()
            self.dock.show()
        else:
            self.app.removeDockWidget(self.dock)
            
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("view_SSEHelixCorrespondences").setChecked(visible)
           
    def createActions(self):               
        corrAct = QtGui.QAction(self.tr("View Alpha-Helix Correspondences"), self)
        corrAct.setStatusTip(self.tr("View Alpha-Helix Correspondences"))
        corrAct.setCheckable(True)
        corrAct.setChecked(False)
        self.connect(corrAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("view_SSEHelixCorrespondences", corrAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-sse-findhelixcorrespondences", self.app.actions.getAction("perform_SSEFindHelixCorrespondences"), "actions-sse")        
    
    def setConstants(self):
        #Tab 1
        self.engine.setConstant("SSE_FILE_NAME", str(self.ui.lineEditHelixLengthFile.text()))
        self.engine.setConstant("VRML_HELIX_FILE_NAME", str(self.ui.lineEditHelixLocationFile.text()))
        self.engine.setConstant("MRC_FILE_NAME", str(self.ui.lineEditSkeletonFile.text()))
        self.engine.setConstant("PDB_FILE_NAME", str(self.ui.lineEditSequenceFile.text()))
        
        #Tab 2
        if(self.ui.radioButtonAbsoluteDifference.isChecked()):
            self.engine.setConstant("COST_FUNCTION", 1)
        elif (self.ui.radioButtonNormalizedDifference.isChecked()):
            self.engine.setConstant("COST_FUNCTION", 2)
        else :
            self.engine.setConstant("COST_FUNCTION", 3)

        self.engine.setConstant("VOXEL_SIZE", self.ui.doubleSpinBoxVoxelSize.value())        
        self.engine.setConstant("EUCLIDEAN_DISTANCE_THRESHOLD", self.ui.doubleSpinBoxEuclideanDistance.value())
        
        if(self.ui.checkBoxMissingHelices.isChecked()):
            self.engine.setConstant("MISSING_HELIX_COUNT", self.ui.spinBoxMissingHelixCount.value())
        else:
            self.engine.setConstant("MISSING_HELIX_COUNT", -1)            
        
        self.engine.setConstant("TRANSLATE_VOLUMETRIC_COORDINATES", self.ui.checkBoxRepositionSkeleton.isChecked());
        
        #Tab 3
        self.engine.setConstant("MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxHelixMissingPenalty.value())
        self.engine.setConstant("START_END_MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxEndHelixMissingPenalty.value())
        self.engine.setConstant("EUCLIDEAN_LOOP_PENALTY", self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.value())
        self.engine.setConstant("HELIX_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxHelixImportance.value())
        self.engine.setConstant("LOOP_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxLoopImportance.value())
        self.engine.setConstant("MISSING_HELIX_LENGTH", self.ui.doubleSpinBoxAverageMissingHelixLength.value())    
        self.engine.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.ui.doubleSpinBoxEuclideanToPDBRatio.value())
        self.engine.setConstant("BORDER_MARGIN_THRESHOLD", self.ui.spinBoxBorderMarginThreshold.value())
        self.engine.setConstant("NORMALIZE_GRAPHS", True)        
    
    def populateResults(self):
        self.ui.tabWidget.setCurrentIndex(3)
        self.ui.tableWidgetResults.setEnabled(True)
        self.ui.tableWidgetResults.setRowCount(self.resultCount)
        for i in range(self.resultCount):                    
            result = self.engine.getResult(i+1)
            self.ui.tableWidgetResults.setItem(i, 0, QtGui.QTableWidgetItem(result.getNodeString()))
            self.ui.tableWidgetResults.setItem(i, 1, QtGui.QTableWidgetItem(str(result.getCost())))
            
    
    def accept(self):
        if(self.executed):
            self.engine.cleanupMemory()
            
        self.setCursor(QtCore.Qt.BusyCursor)
        self.engine = SSECorrespondenceEngine()
        self.setConstants()                       
        self.engine.loadSequenceGraph()
        self.engine.loadSkeletonGraph()
        self.resultCount = self.engine.executeQuery()        
        self.populateResults()                                       
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.emitModelChanged()
        self.executed = True 
        
    def reject(self):
        if(self.executed):
            self.engine.cleanupMemory()
            
        self.app.actions.getAction("perform_SSEFindHelixCorrespondences").trigger()
        