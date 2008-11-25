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
#   Revision 1.24  2008/11/25 00:05:39  colemanr
#   updates the index of the currently selected correspondence in the CorrespondenceLibrary
#
#   Revision 1.23  2008/11/24 21:11:49  colemanr
#   changes to accept method so that the ObservedHelix objects have C-alpha coordinates
#
#   Revision 1.22  2008/11/24 20:49:52  ssa1
#   Updating information on the correspondence finder
#
#   Revision 1.21  2008/11/24 20:02:49  ssa1
#   User constraints on finding correspondences (v1)
#
#   Revision 1.20  2008/11/23 19:56:07  ssa1
#   Setting volume origin to be center of mass instead of bottom left...
#
#   Revision 1.19  2008/11/20 21:05:16  ssa1
#   Removing depricated items
#
#   Revision 1.18  2008/11/20 20:56:30  ssa1
#   Properly scaling the suggested backbone
#
#   Revision 1.17  2008/11/17 19:43:08  colemanr
#   In self.accept, moved the logic for creating a StructurePrediciton object
#   to StructurePrediction.load()
#
#   Revision 1.16  2008/11/14 22:42:45  colemanr
#   temporary hack to set the chain object in the structure prediction; set
#   CAlphaViewer.structPred to the structure prediction, so SequenceView.py
#   will get the correct object
#
#   Revision 1.15  2008/11/13 00:30:32  colemanr
#   changes concerning *.seq files
#
#   Revision 1.14  2008/11/11 15:38:55  colemanr
#   fixed indentation problems
#
#   Revision 1.13  2008/11/10 21:12:54  colemanr
#   allows either SEQ or PDB files to define the sequence
#
#   Revision 1.12  2008/08/27 15:26:52  marshm
#   Updates to SequenceView.  Updated coloring scheme for correspondence matches.
#
#   Revision 1.11  2008/08/06 06:21:37  ssa1
#   Tracing protein path, for SSE Correspondance matching
#
#   Revision 1.10  2008/07/29 20:40:43  ssa1
#   Re-formatting graph matching results
#
#   Revision 1.9  2008/07/29 16:57:18  ssa1
#   Visualizing the correspondence and fixing bug in the CorrespondenceLibrary
#
#   Revision 1.8  2008/07/29 15:50:31  ssa1
#   Fixing import errors
#
#   Revision 1.7  2008/07/28 16:28:16  ssa1
#   Adding in correspondance data repository
#
#   Revision 1.6  2008/07/28 16:19:22  ssa1
#   Adding in correspondance data repository
#
#   Revision 1.5  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from ui_dialog_sse_helix_correspondence_finder import Ui_DialogSSEHelixCorrespondenceFinder
from libpyGORGON import SSECorrespondenceEngine, SSECorrespondenceResult
from correspondence.CorrespondenceLibrary import CorrespondenceLibrary
from correspondence.Correspondence import Correspondence
from correspondence.Match import Match
from correspondence.ObservedHelix import ObservedHelix
from correspondence.ObservedSheet import ObservedSheet
from correspondence.StructureObservation import StructureObservation
from correspondence.StructurePrediction import StructurePrediction
from seq_model.Helix import Helix
import sans_numpy as snum

class SSEHelixCorrespondenceFinderForm(QtGui.QWidget):   
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.executed = False
        self.app = main
        self.viewer = viewer        
        self.createUI()
        self.createActions()
        self.createMenus()
        self.loadingCorrespondance = False
        self.userConstraints = {}
        self.constraintActions = {}
        self.selectedRow = 0

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
        self.connect(self.ui.comboBoxCorrespondences, QtCore.SIGNAL("currentIndexChanged (int)"), self.selectCorrespondence)
        self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("modelDrawing()"), self.drawOverlay)
        self.connect(self.ui.tableWidgetCorrespondenceList, QtCore.SIGNAL("cellClicked (int,int)"), self.cellClicked)
        self.ui.tableWidgetCorrespondenceList.setContextMenuPolicy(QtCore.Qt.ActionsContextMenu)
        #self.connect(self.ui.tableWidgetCorrespondenceList, QtCore.SIGNAL("customContextMenuRequested (const QPoint&)"), self.customMenuRequested)
            
    def loadDefaults(self):
        self.ui.lineEditHelixLengthFile.setText("")
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.ui.lineEditSkeletonFile.setText(self.app.viewers["skeleton"].fileName)
        self.ui.lineEditSequenceFile.setText(self.app.viewers["calpha"].fileName)        

        self.ui.radioButtonAbsoluteDifference.setChecked(True)
        self.ui.radioButtonNormalizedDifference.setChecked(False)
        self.ui.radioButtonQuadraticError.setChecked(False)
        self.ui.doubleSpinBoxEuclideanDistance.setValue(0.0)
        self.ui.checkBoxMissingHelices.setChecked(False)
        self.ui.spinBoxMissingHelixCount.setValue(0)
        
        self.ui.doubleSpinBoxHelixMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxEndHelixMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.setValue(5.0)
        self.ui.doubleSpinBoxHelixImportance.setValue(1.0)
        self.ui.doubleSpinBoxLoopImportance.setValue(0.2)
        self.ui.doubleSpinBoxAverageMissingHelixLength.setValue(5.0)
        self.ui.doubleSpinBoxEuclideanToPDBRatio.setValue(10.0)
        self.ui.spinBoxBorderMarginThreshold.setValue(5)               
                
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
        self.ui.lineEditSequenceFile.setText(self.openFile("Load Helix Length File", "Sequence with SSE predictions (*.seq)\nPDB Helix Annotations (*.pdb)"))
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
        self.sequenceFileName = str(self.ui.lineEditSequenceFile.text())
        self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_NAME", self.sequenceFileName)
        if self.sequenceFileName.split('.')[-1].lower() == 'pdb':
            self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "PDB")
        elif self.sequenceFileName.split('.')[-1].lower() == 'seq':
            self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "SEQ")
        
        #Tab 2
        if(self.ui.radioButtonAbsoluteDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 1)
        elif (self.ui.radioButtonNormalizedDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 2)
        else :
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 3)

        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_DISTANCE_THRESHOLD", self.ui.doubleSpinBoxEuclideanDistance.value())
                                                      
        if(self.ui.checkBoxMissingHelices.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", self.ui.spinBoxMissingHelixCount.value())
        else:
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", -1)            
        
        
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
    
    
        #Tab 4 User Constraints
        self.viewer.correspondenceEngine.clearAllConstraints()
        correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
        if(correspondenceIndex >= 0):
            corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]            
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]                
                self.userConstraints[i] = match.constrained
                if(match.constrained):
                    self.viewer.correspondenceEngine.setHelixConstraint(match.predicted.serialNo + 1, match.observed.label + 1)      
    
    def populateResults(self, library):
        self.ui.tabWidget.setCurrentIndex(3)
        corrList = []
        for i in range(self.resultCount):                                
            result = self.viewer.correspondenceEngine.getResult(i+1)
                                    
            matchList = []            
            for j in range(result.getNodeCount()/2):
                n1 = result.getSkeletonNode(j*2)
                n2 = result.getSkeletonNode(j*2+1)
                if n1 < n2:
                    direction = Match.FORWARD
                else:
                    direction = Match.REVERSE
                
                observedNo = result.nodeToHelix(n1)
                if observedNo >= 0:
                    observed = library.structureObservation.helixDict[observedNo]
                else:
                    observed = None
                    
                predicted = library.structurePrediction.secelDict[j]
                currentMatch = Match(observed, predicted, direction)
                if(self.userConstraints.has_key(j)):
                    currentMatch.constrained = self.userConstraints[j]
                else :                      
                    currentMatch.constrained = False     
                matchList.append(currentMatch)         
                                      
            corr = Correspondence(library=library, matchList=matchList, score=result.getCost())
            
            
            corrList.append(corr)
        return corrList
            
    def populateComboBox(self, library):
        self.ui.comboBoxCorrespondences.clear()
        for i in range(len(library.correspondenceList)):
            corr = library.correspondenceList[i]                                
            self.ui.comboBoxCorrespondences.addItem("Correspondence " + str(i+1) + " - [Cost: " + str(corr.score) + "]")        
            
                
    def accept(self):
        self.setCursor(QtCore.Qt.BusyCursor)
        self.setConstants()          
        
        
        #Loading Predicted SSEs                     
        self.viewer.correspondenceEngine.loadSequenceGraph()

        structPred = StructurePrediction.load(self.sequenceFileName, self.app)
        cAlphaViewer = self.app.viewers['calpha']
        sseViewer = self.app.viewers['sse']
        skeletonViewer = self.app.viewers['skeleton']
        cAlphaViewer.structPred = structPred
        def vector3DFloatToTuple(v3df):
            return (v3df.x(), v3df.y(), v3df.z())
        
        #Loading Observed SSEs
        self.viewer.correspondenceEngine.loadSkeletonGraph()
        observedHelices = {}
        helixCount = 0
        observedSheets = {}
        sheetCount = 0
        sseCount = self.viewer.correspondenceEngine.getSkeletonSSECount()
        for sseIx in range(sseCount):
            cppSse = self.viewer.correspondenceEngine.getSkeletonSSE(sseIx)        
            p1 = cAlphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell2(1))))
            p2 = cAlphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell2(2))))
            q1 = cAlphaViewer.worldToObjectCoordinates(sseViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell3(1))))
            q2 = cAlphaViewer.worldToObjectCoordinates(sseViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell3(2))))
            if snum.vectorMagnitude(snum.vectorAdd(p1, snum.scalarTimesVector(-1, q1))) > snum.vectorMagnitude(
                                    snum.vectorAdd(p1, snum.scalarTimesVector(-1, q2))): #to get proper orientation
                q1, q2 = q2, q1 #python trick for exchanging values
            
            if cppSse.isHelix():            
                pyHelix = ObservedHelix(sseIx, q1, q2)
                observedHelices[helixCount] = pyHelix
                helixCount = helixCount + 1
            elif cppSse.isSheet():
                pass
                #TODO: Add Sheet support
        
        #TODO: Mike this raises an error!;
        structObserv = StructureObservation(helixDict = observedHelices, sheetDict = observedSheets)          
                
        self.resultCount = self.viewer.correspondenceEngine.executeQuery()
        self.viewer.correspondenceEngine.cleanupMemory()
        self.viewer.correspondenceLibrary = CorrespondenceLibrary(sp = structPred, so = structObserv)
        self.viewer.correspondenceLibrary.correspondenceList = self.populateResults(self.viewer.correspondenceLibrary)
        
        self.populateComboBox(self.viewer.correspondenceLibrary)       
        
        self.setCursor(QtCore.Qt.ArrowCursor)
        self.executed = True 
        self.viewer.emitModelChanged()
        
        
    def reject(self):  
        self.executed = False
        self.app.actions.getAction("perform_SSEFindHelixCorrespondences").trigger()
        
    
    def getIndexedColor(self, index, size):
        a = 1.0
        i = float(index)
        N = float(size)

        # blue  (0,0,1)
        if (i/N) == 0:
            r=0;g=0;b=1.0

        # blue - cyan(0,1,1)
        elif (i/N) <= 0.25:
            coeff = (i/N)/0.25
            r=0.0
            g=coeff
            b=1.0

        # cyan - green (0,1,0)
        elif (i/N) > 0.25:
            coeff=((i/N)-0.25)/0.25
            r=0.0
            g=1.0
            b=1.0-coeff

            # green - yellow (1,1,0)
            if (i/N) > 0.50:
                coeff=((i/N)-0.25)/0.50
                r=coeff
                g=1.0
                b=0.0

                # yelow - red (1,0,0)
                if (i/N) > 0.75:
                    coeff=((i/N)-0.25)/0.75
                    r=1.0
                    g=1-coeff
                    b=0.0

                    # red
                    if (i/N)==1.0:
                        r=1.0;g=0;b=0
                    
        return QtGui.QColor.fromRgba(QtGui.qRgba(r*255, g*255, b*255, a*255))
        
    def selectCorrespondence(self, correspondenceIndex):
        self.loadingCorrespondance = True
        self.ui.tableWidgetCorrespondenceList.clearContents()
        if(correspondenceIndex >= 0):
            corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
            self.ui.tableWidgetCorrespondenceList.setRowCount(len(corr.matchList))   
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]
                color = self.getIndexedColor(i, len(corr.matchList))
                match.predicted.setColor(color)
                if(match.predicted):
                    #print match.predicted, match.predicted.type, match.predicted.serialNo, match.predicted.label
                    cellItemPredicted =  QtGui.QTableWidgetItem(match.predicted.type + " " + str(match.predicted.serialNo + 1) + " : " + str(match.predicted.label) +
                                                                "\n  "  + str(round(match.predicted.getLengthInAngstroms(),2)) + "A length"
                                                                "\n  "  + str(match.predicted.getResidueCount()) + " residues")
                                                                 
                    cellItemPredicted.setBackgroundColor(color)
                    self.ui.tableWidgetCorrespondenceList.setItem(i, 0, cellItemPredicted)
                if(match.observed):
                    cellItemObserved =  QtGui.QTableWidgetItem("helix " + str(match.observed.label + 1) +
                                                               "\n  " + str(round(match.observed.getLength(), 2)) + "A length" +
                                                               "\n  " )
                    cellItemObserved.setBackgroundColor(color)
                    self.ui.tableWidgetCorrespondenceList.setItem(i, 1, cellItemObserved)
                    self.viewer.renderer.setHelixColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                checkBox = QtGui.QCheckBox()
                self.ui.tableWidgetCorrespondenceList.setCellWidget(i, 2, checkBox)
                self.connect(checkBox, QtCore.SIGNAL("stateChanged (int)"), self.constraintAdded)
                if(match.constrained):
                    self.ui.tableWidgetCorrespondenceList.cellWidget(i, 2).setCheckState(QtCore.Qt.Checked)
                else :
                    self.ui.tableWidgetCorrespondenceList.cellWidget(i, 2).setCheckState(QtCore.Qt.Unchecked)
                    
                self.ui.tableWidgetCorrespondenceList.resizeRowToContents(i)
                    
        self.viewer.correspondenceEngine.setVisibleCorrespondence(correspondenceIndex)
        self.viewer.correspondenceLibrary.setCurrentCorrespondenceIndex(correspondenceIndex)
        self.viewer.emitModelChanged()
        self.loadingCorrespondance = False
        
    def drawOverlay(self):
        if self.executed:
            self.viewer.correspondenceEngine.draw(0)

    def constraintAdded(self, state):
        if(not self.loadingCorrespondance):
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]            
                for i in range(len(corr.matchList)):
                    match = corr.matchList[i]
                    match.constrained = (self.ui.tableWidgetCorrespondenceList.cellWidget(i, 2).checkState() == QtCore.Qt.Checked)
                    
    def cellClicked(self, row, col):
        self.selectedRow = row
        for act in self.ui.tableWidgetCorrespondenceList.actions()[:]:
            self.ui.tableWidgetCorrespondenceList.removeAction(act)        
        if(col == 1):
            observedHelices = self.viewer.correspondenceLibrary.structureObservation.helixDict
            constrained = {}
            
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]  
                for i in range(len(corr.matchList)):
                    match = corr.matchList[i]
                    if(match.constrained and match.observed) :
                        constrained[match.observed.label] = True
                match = corr.matchList[row]
                
            else:
                match = False
            
            for i in range(len(observedHelices)):                
                constrainAction = QtGui.QAction(self.tr("Helix " + str(i+1) + " (Length: " + str(round(observedHelices[i].getLength(), 2)) + "A)"), self)
                constrainAction.setCheckable(True)
                if(match and match.observed):
                    constrainAction.setChecked(match.observed.label == i)
                else:
                    constrainAction.setChecked(False)
                constrainAction.setEnabled(not constrained.has_key(i))
                self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedHelix(i))       
                self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)
                
            constrainAction = QtGui.QAction(self.tr("Not observed"), self)
            constrainAction.setCheckable(True)
            constrainAction.setChecked(match and not match.observed)
            constrainAction.setEnabled(True)
            self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedHelix(-1))       
            self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)                

                
            
    def constrainObservedHelix(self, i):
        def constrainObservedHelix_i():
                correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
                if(correspondenceIndex >= 0):
                    corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
                    for j in range(len(corr.matchList)):
                        match = corr.matchList[j]
                        if(match and match.observed and (match.observed.label == i)) :
                            match.observed = None
                    
                    match = corr.matchList[self.selectedRow]
                    match.constrained = True
                    if(i == -1):
                        match.observed = None
                    else:
                        match.observed = self.viewer.correspondenceLibrary.structureObservation.helixDict[i]
                self.selectCorrespondence(correspondenceIndex)
        return constrainObservedHelix_i
