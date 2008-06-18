# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that can be used to find correspondance between helices 

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
from ui_dialog_sse_helix_correspondence_finder import Ui_DialogSSEHelixCorrespondenceFinder
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
        self.dock = QtGui.QDockWidget(self.tr("SSE - Helix Correspondence Finder"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.pushButtonGetHelixLengthFile, QtCore.SIGNAL("pressed ()"), self.getHelixLengthFile)
        self.connect(self.ui.pushButtonGetHelixLocationFile, QtCore.SIGNAL("pressed ()"), self.getHelixLocationFile)
        self.connect(self.ui.pushButtonGetSkeletonFile, QtCore.SIGNAL("pressed ()"), self.getSkeletonFile)
        self.connect(self.ui.pushButtonGetSequenceFile, QtCore.SIGNAL("pressed ()"), self.getSequenceFile)
        self.connect(self.ui.pushButtonReset, QtCore.SIGNAL("pressed ()"), self.loadDefaults)
        self.connect(self.ui.pushButtonCancel, QtCore.SIGNAL("pressed ()"), self.reject)
        self.connect(self.ui.pushButtonOk, QtCore.SIGNAL("pressed ()"), self.accept)
            
    def loadDefaults(self):
        self.ui.lineEditHelixLengthFile.setText("")
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.ui.lineEditSkeletonFile.setText(self.app.viewers["skeleton"].fileName)
        self.ui.lineEditSequenceFile.setText(self.app.viewers["calpha"].fileName)        

        self.ui.radioButtonAbsoluteDifference.setChecked(True)
        self.ui.radioButtonNormalizedDifference.setChecked(False)
        self.ui.radioButtonQuadraticError.setChecked(False)
        self.ui.doubleSpinBoxVoxelSize.setValue(1.0)
        self.ui.doubleSpinBoxEuclideanDistance.setValue(0.0)
        self.ui.checkBoxMissingHelices.setChecked(False)
        self.ui.spinBoxMissingHelixCount.setValue(0)
        self.ui.checkBoxRepositionSkeleton.setChecked(False)
        
        self.ui.doubleSpinBoxHelixMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxEndHelixMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.setValue(5.0)
        self.ui.doubleSpinBoxHelixImportance.setValue(1.0)
        self.ui.doubleSpinBoxLoopImportance.setValue(0.2)
        self.ui.doubleSpinBoxAverageMissingHelixLength.setValue(5.0)
        self.ui.doubleSpinBoxEuclideanToPDBRatio.setValue(10.0)
        self.ui.spinBoxBorderMarginThreshold.setValue(3)               
                
        self.checkOk()

        
    def openFile(self, title, fileformats):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr(title), "", self.tr(fileformats))
        return fileName
    
    def getHelixLengthFile(self):
        self.ui.lineEditHelixLengthFile.setText(self.openFile("Load Helix Length File", "SSE Hunter results (*.sse)"))
        self.checkOk()

    def getHelixLocationFile(self):
        self.viewer.loadHelixData()
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.checkOk()               

    def getSkeletonFile(self):
        self.app.viewers["skeleton"].loadData()
        self.ui.lineEditSkeletonFile.setText(self.app.viewers["skeleton"].fileName)
        self.checkOk()
        
    def getSequenceFile(self):
        self.ui.lineEditSequenceFile.setText(self.openFile("Load Helix Length File", "PDB Helix Annotations (*.pdb)"))
        self.checkOk()
    
    def checkOk(self):
        self.ui.pushButtonOk.setEnabled(not(self.ui.lineEditHelixLengthFile.text().isEmpty() or self.ui.lineEditHelixLocationFile.text().isEmpty()
                                            or self.ui.lineEditSkeletonFile.text().isEmpty() or self.ui.lineEditSequenceFile.text().isEmpty()))
    
    def loadWidget(self):
        if(self.app.actions.getAction("perform_SSEFindHelixCorrespondences").isChecked()) :
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.loadDefaults()
            self.dock.show()
        else:
            self.app.removeDockWidget(self.dock)
            
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("perform_SSEFindHelixCorrespondences").setChecked(visible)
    
    def modelChanged(self):
        if(not self.viewer.loaded) and self.app.actions.getAction("perform_SSEFindHelixCorrespondences").isChecked():
            self.app.actions.getAction("perform_SSEFindHelixCorrespondences").trigger()        
        
    def createActions(self):               
        corrAct = QtGui.QAction(self.tr("Find Alpha-Helix Correspondences"), self)
        corrAct.setStatusTip(self.tr("Find Alpha-Helix Correspondences"))
        corrAct.setCheckable(True)
        corrAct.setChecked(False)
        self.connect(corrAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("perform_SSEFindHelixCorrespondences", corrAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-sse-findhelixcorrespondences", self.app.actions.getAction("perform_SSEFindHelixCorrespondences"), "actions-sse")        
    
    def setConstants(self):
        #Tab 1
        self.viewer.correspondenceEngine.setConstant("SSE_FILE_NAME", str(self.ui.lineEditHelixLengthFile.text()))
        self.viewer.correspondenceEngine.setConstant("VRML_HELIX_FILE_NAME", str(self.ui.lineEditHelixLocationFile.text()))
        self.viewer.correspondenceEngine.setConstant("MRC_FILE_NAME", str(self.ui.lineEditSkeletonFile.text()))
        self.viewer.correspondenceEngine.setConstant("PDB_FILE_NAME", str(self.ui.lineEditSequenceFile.text()))
        
        #Tab 2
        if(self.ui.radioButtonAbsoluteDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 1)
        elif (self.ui.radioButtonNormalizedDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 2)
        else :
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 3)

        self.viewer.correspondenceEngine.setConstant("VOXEL_SIZE", self.ui.doubleSpinBoxVoxelSize.value())        
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_DISTANCE_THRESHOLD", self.ui.doubleSpinBoxEuclideanDistance.value())
                                                      
        if(self.ui.checkBoxMissingHelices.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", self.ui.spinBoxMissingHelixCount.value())
        else:
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", -1)            
        
        self.viewer.correspondenceEngine.setConstantBool("TRANSLATE_VOLUMETRIC_COORDINATES", self.ui.checkBoxRepositionSkeleton.isChecked());
        
        #Tab 3
        self.viewer.correspondenceEngine.setConstant("MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxHelixMissingPenalty.value())
        self.viewer.correspondenceEngine.setConstant("START_END_MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxEndHelixMissingPenalty.value())
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_LOOP_PENALTY", self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.value())
        self.viewer.correspondenceEngine.setConstant("HELIX_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxHelixImportance.value())
        self.viewer.correspondenceEngine.setConstant("LOOP_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxLoopImportance.value())
        self.viewer.correspondenceEngine.setConstant("MISSING_HELIX_LENGTH", self.ui.doubleSpinBoxAverageMissingHelixLength.value())    
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.ui.doubleSpinBoxEuclideanToPDBRatio.value())
        self.viewer.correspondenceEngine.setConstantInt("BORDER_MARGIN_THRESHOLD", self.ui.spinBoxBorderMarginThreshold.value())
        self.viewer.correspondenceEngine.setConstantBool("NORMALIZE_GRAPHS", True)        
    
    def populateResults(self):
        self.ui.tabWidget.setCurrentIndex(3)
        self.ui.tableWidgetResults.setEnabled(True)
        self.ui.tableWidgetResults.setRowCount(self.resultCount)
        for i in range(self.resultCount):                    
            result = self.viewer.correspondenceEngine.getResult(i+1)
            self.ui.tableWidgetResults.setItem(i, 0, QtGui.QTableWidgetItem(result.getNodeString()))
            self.ui.tableWidgetResults.setItem(i, 1, QtGui.QTableWidgetItem(str(result.getCost())))
                
    def accept(self):
        self.setCursor(QtCore.Qt.BusyCursor)
        self.setConstants()                       
        self.viewer.correspondenceEngine.loadSequenceGraph()
        self.viewer.correspondenceEngine.loadSkeletonGraph()
        self.resultCount = self.viewer.correspondenceEngine.executeQuery()
        self.viewer.correspondenceEngine.cleanupMemory()
        self.populateResults()                                       
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.viewer.emitModelChanged()
        self.executed = True 
        
    def reject(self):           
        self.app.actions.getAction("perform_SSEFindHelixCorrespondences").trigger()
        