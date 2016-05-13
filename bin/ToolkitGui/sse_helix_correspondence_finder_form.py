from PyQt4 import QtCore, QtGui
from ui_dialog_sse_helix_correspondence_finder import Ui_DialogSSEHelixCorrespondenceFinder
from libpytoolkit import SSEEngine, SSEResult, Vec3F
from Toolkit.sse.correspondence.CorrespondenceLibrary import CorrespondenceLibrary
from Toolkit.sse.correspondence.Correspondence import Correspondence
from Toolkit.sse.correspondence.Match import Match
from Toolkit.sse.correspondence.ObservedHelix import ObservedHelix
from Toolkit.sse.correspondence.ObservedSheet import ObservedSheet
from Toolkit.sse.correspondence.StructureObservation import StructureObservation
from Toolkit.sse.correspondence.StructurePrediction import StructurePrediction
from Toolkit.sse.seq_model.Helix import Helix
from Explorer import Vec3
import xml.dom.minidom

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import math


class SSEHelixCorrespondenceFinderForm(QtGui.QDialog):

    def __init__(self, main, parent=None):
        QtGui.QDialog.__init__(self, main)
        
        self.app = main
        self.viewer = parent
        dock = QtGui.QDockWidget("SSEHelixCorrespondenceFinder", self.app)
        dock.setWidget(self)
        dock.setAllowedAreas(QtCore.Qt.AllDockWidgetAreas)
        self.app.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)
        
        self.executed = False
        self.colors = {}
        self.colors["CorrespondenceFinder:BackboneTrace"] = QtGui.QColor(255, 255, 255, 255)
        self.createUI()
        self.loadingCorrespondance = False
        self.userConstraints = {}
        self.constraintActions = {}
        self.selectedRow = 0
        self.dataLoaded = False

    def createUI(self):
        self.ui = Ui_DialogSSEHelixCorrespondenceFinder()
        self.ui.setupUi(self)
        self.connect(self.ui.pushButtonGetHelixLengthFile, QtCore.SIGNAL("pressed ()"), self.getHelixLengthFile)
        self.connect(self.ui.pushButtonGetHelixLocationFile, QtCore.SIGNAL("pressed ()"), self.getHelixLocationFile)
        self.connect(self.ui.pushButtonGetSheetLocationFile, QtCore.SIGNAL("pressed ()"), self.getSheetLocationFile)
        self.connect(self.ui.pushButtonGetSkeletonFile, QtCore.SIGNAL("pressed ()"), self.getSkeletonFile)
        self.connect(self.ui.pushButtonGetSequenceFile, QtCore.SIGNAL("pressed ()"), self.getSequenceFile)
        self.connect(self.ui.pushButtonGetSettingsFile, QtCore.SIGNAL("pressed ()"), self.getSettingsFile)
        self.connect(self.ui.pushButtonReset, QtCore.SIGNAL("pressed ()"), self.loadDefaults)
        self.connect(self.ui.pushButtonCancel, QtCore.SIGNAL("pressed ()"), self.reject)
        self.connect(self.ui.pushButtonOk, QtCore.SIGNAL("pressed ()"), self.accept)
        self.connect(self.ui.pushButtonRebuildGraph, QtCore.SIGNAL("pressed ()"), self.rebuildGraph)
        self.connect(self.ui.comboBoxCorrespondences, QtCore.SIGNAL("currentIndexChanged (int)"), self.selectCorrespondence)
#         self.connect(self.ui.pushButtonExportToRosetta, QtCore.SIGNAL("pressed ()"), self.exportToRosetta)
#         self.connect(self.ui.checkBoxShowAllPaths, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
#         self.connect(self.ui.checkBoxShowSheetCorners, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
#         self.connect(self.ui.checkBoxShowHelixCorners, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
#         self.connect(self.ui.checkBoxShowSheetColors, QtCore.SIGNAL("toggled (bool)"), self.graphSheetVisibilityChanged)
#         self.connect(self.ui.checkBoxShowSkeleton, QtCore.SIGNAL("toggled (bool)"), self.skeletonVisibilityChanged)
#         self.connect(self.ui.checkBoxShowSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetVisibilityChanged)
#         self.connect(self.ui.checkBoxShowHelices, QtCore.SIGNAL("toggled (bool)"), self.helixVisibilityChanged)
#         self.connect(self.ui.checkBoxIncludeSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetIncludeChanged)
#         self.connect(self.ui.checkBoxMissingSheets, QtCore.SIGNAL("toggled (bool)"), self.missingSheetChanged)
#         self.connect(self.ui.checkBoxMissingHelices, QtCore.SIGNAL("toggled (bool)"), self.missingHelixChanged)
#         self.connect(self.app.skeletonViewer, QtCore.SIGNAL("modelDrawing()"), self.drawOverlay)
#         self.ui.tableWidgetCorrespondenceList.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
#         self.connect(self.ui.tableWidgetCorrespondenceList, QtCore.SIGNAL("customContextMenuRequested (const QPoint&)"), self.customMenuRequested)
#         self.connect(self.viewer, QtCore.SIGNAL("elementClicked (int, int, int, int, int, int, QMouseEvent)"), self.sseClicked)
        self.ui.label.setVisible(False)
        self.ui.lineEditHelixLengthFile.setVisible(False)
        self.ui.pushButtonGetHelixLengthFile.setVisible(False)
          
    # populate parameter boxes with default values for correspondence search
    def loadDefaults(self):
        self.ui.lineEditHelixLengthFile.setText("")
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.ui.lineEditSheetLocationFile.setText(self.viewer.sheetFileName)
        self.ui.lineEditSkeletonFile.setText(self.app.skeletonViewer.fileName)
        self.ui.lineEditSequenceFile.setText(self.app.calphaViewer.fileName)
        self.ui.lineEditSettingsFile.setText("")
        self.loadDefaultParams()

    def loadDefaultParams(self):
        self.ui.pushButtonExportToRosetta.setVisible(False)
        # Graph Settings tab
        self.ui.spinBoxBorderMarginThreshold.setValue(5)
        self.ui.doubleSpinBoxEuclideanDistance.setValue(0.0)
        self.ui.checkBoxIncludeSheets.setChecked(True)
        #min sheet size
        #max sheet distance
        #sheet self-loop length

        self.ui.checkBoxShowSkeleton.setChecked(self.app.skeletonViewer.visualizationOptions.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelices.setChecked(self.app.sseViewer.visualizationOptions.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelixCorners.setChecked(False)
        self.ui.checkBoxShowSheets.setChecked(self.app.sseViewer.visualizationOptions.ui.checkBoxModel2Visible.isChecked())
        self.ui.checkBoxShowSheetColors.setChecked(self.app.sseViewer.visualizationOptions.ui.checkBoxModel3Visible.isChecked())
        self.ui.checkBoxShowSheetCorners.setChecked(False)
        self.ui.checkBoxShowAllPaths.setChecked(False)

        # Matching Settings tab
        self.ui.doubleSpinBoxEuclideanToPDBRatio.setValue(10.0)
        self.ui.radioButtonAbsoluteDifference.setChecked(True)
        self.ui.radioButtonNormalizedDifference.setChecked(False)
        self.ui.radioButtonQuadraticError.setChecked(False)

        self.ui.doubleSpinBoxLoopImportance.setValue(0.2)
        self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.setValue(5.0)
        
        self.ui.doubleSpinBoxHelixImportance.setValue(1.0)
        self.ui.checkBoxMissingHelices.setChecked(False)
        self.ui.spinBoxMissingHelixCount.setValue(0)
        self.ui.doubleSpinBoxHelixMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxHelixMissingPenaltyScaled.setValue(0.0)
        self.ui.doubleSpinBoxEndHelixMissingPenalty.setValue(5.0)

        self.ui.doubleSpinBoxSheetImportance.setValue(1.0)
        self.ui.checkBoxMissingSheets.setChecked(False)
        self.ui.spinBoxMissingSheetCount.setValue(0)
        self.ui.doubleSpinBoxSheetMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setValue(0.0)

        # Results tab
        self.ui.tableWidgetCorrespondenceList.clearContents()
        self.ui.tabWidget.setCurrentIndex(0)
        self.ui.comboBoxCorrespondences.setCurrentIndex(-1)

        self.checkOk()
        
    def openFile(self, title, fileformats):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr(title), "", self.tr(fileformats))
        return fileName
    
    def getHelixLengthFile(self):
        self.ui.lineEditHelixLengthFile.setText(self.openFile("Load Helix Length File", "SSE Hunter results (*.sse)"))
        self.checkOk()
#         self.bringToFront()

    def getHelixLocationFile(self):
        self.viewer.loadHelixData()
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.checkOk()
#         self.bringToFront()

    def getSheetLocationFile(self):
        self.viewer.loadSheetData()
        self.ui.lineEditSheetLocationFile.setText(self.viewer.sheetFileName)
        self.checkOk()
#         self.bringToFront()

    def getSkeletonFile(self):
        self.app.skeletonViewer.loadData()
        self.ui.lineEditSkeletonFile.setText(self.app.skeletonViewer.fileName)
        self.checkOk()
#         self.bringToFront()
        
    def getSequenceFile(self):
        """
        This loads a SEQ file or, for testing purposes, a PDB file.
        """
        self.ui.lineEditSequenceFile.setText(self.openFile("Load Sequence File", "Sequence with SSE predictions (*.seq)\nPDB Helix Annotations (*.pdb)"))
        self.checkOk()
#         self.bringToFront()

    def getSettingsFile(self):
        """
        This loads a settings file, which contains filenames and search parameters.
        """
        self.ui.lineEditSettingsFile.setText(self.openFile("Load Settings File", "Settings File (*.txt)"))
        self.loadSettings()
#         self.bringToFront()
        
    def checkOk(self):
        """
        This checks if all files necessary for the correspondence search have been loaded. If so, the
        correspondence search parameter tabs are enabled and a basic correspondence is created.   
        """
        print "begin checkOk"
        print "correspondence index at beginning is "
        print self.ui.comboBoxCorrespondences.currentIndex()

        allLoaded = True

        self.dataLoaded = True
        self.ui.pushButtonOk.setEnabled(self.dataLoaded)
        
        self.ui.tabWidget.setTabEnabled(1, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(2, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(3, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(4, self.dataLoaded)
        if(True):
            self.executed = False
            self.createBasicCorrespondence()
            print "after creating basic correspondence (1), secelDict has length " + str(len(self.viewer.correspondenceLibrary.structurePrediction.secelDict))
            #self.createBasicCorrespondence()
            print "after creating basic correspondence (2), secelDict has length " + str(len(self.viewer.correspondenceLibrary.structurePrediction.secelDict))
            self.viewer.correspondenceLibrary.correspondenceList = self.populateEmptyResults(self.viewer.correspondenceLibrary)
            print "correspondenceList has length " + str(len(self.viewer.correspondenceLibrary.correspondenceList))
            self.populateComboBox(self.viewer.correspondenceLibrary)
            self.viewer.makeSheetSurfaces(self.app.skeletonViewer.renderer.getOriginX(), self.app.skeletonViewer.renderer.getOriginY(), self.app.skeletonViewer.renderer.getOriginZ(), self.app.skeletonViewer.renderer.getSpacingX(), self.app.skeletonViewer.renderer.getSpacingY(), self.app.skeletonViewer.renderer.getSpacingZ())
            if(allLoaded):
                self.ui.tabWidget.setCurrentIndex(1)
        else:
            print "data not loaded"
        print "correspondence index at end is " + str(self.ui.comboBoxCorrespondences.currentIndex())
        print "end checkOk"
    
    def loadSettings(self):
        
        if (not(self.ui.lineEditSettingsFile.text().isEmpty())):

            print "loading default parameters"
            self.loadDefaultParams()
        
            print "loading settings file"
            oldCursor = self.cursor()
            self.setCursor(QtCore.Qt.BusyCursor)
                   
            print "calling setConstantsFromFile"
            self.setConstants()
            self.viewer.correspondenceEngine.setConstantsFromFile(str(self.ui.lineEditSettingsFile.text()))
    
            print "copying constants from file to GUI"
            self.getConstants()
    
            print "getting settings filename"
            settingsFilename = str(self.ui.lineEditSettingsFile.text())
            
            #self.viewer.correspondenceEngine.printConstants
            print "reading other filenames for parsing"
            helixFilename = self.viewer.correspondenceEngine.getConstantString("VRML_HELIX_FILE_NAME")
            print "helix filename is " + helixFilename
            sheetFilename = self.viewer.correspondenceEngine.getConstantString("VRML_SHEET_FILE_NAME")
            print "sheet filename is " + sheetFilename
            sequenceFilename = self.viewer.correspondenceEngine.getConstantString("SEQUENCE_FILE_NAME")
            print "sequence filename is " + sequenceFilename
            skeletonFilename = self.viewer.correspondenceEngine.getConstantString("MRC_FILE_NAME")
            print "skeleton filename is " + skeletonFilename
            sseFilename = self.viewer.correspondenceEngine.getConstantString("SSE_FILE_NAME")
            print "sse filename is " + sseFilename
            
            import os
            path,settingsFile = os.path.split(settingsFilename)
            ssePath,sseFile = os.path.split(sseFilename)
            helixPath,helixFile = os.path.split(helixFilename)
            sheetPath,sheetFile = os.path.split(sheetFilename)
            skeletonPath,skeletonFile = os.path.split(skeletonFilename)
            seqPath,seqFile = os.path.split(sequenceFilename)
    
            print "The path (raw) is " + path
            path = path.replace('/', os.sep)  # replace forward slash with separator for this OS
            path = path.replace('\\', os.sep) # replace backslash with separator for this OS
            print "The path (fixed) is " + path
    
            settingsFilePath = path + os.sep + settingsFile
            sseFilePath = path + os.sep + sseFile
            helixFilePath = path + os.sep + helixFile
            sheetFilePath = path + os.sep + sheetFile
            skeletonFilePath = path + os.sep + skeletonFile
            seqFilePath = path + os.sep + seqFile
    
            print "The settings file is " + settingsFilePath
            print "The sse file is " + sseFilePath
            print "The helix file is " + helixFilePath
            print "The sheet file is " + sheetFilePath
            print "The skeleton file is " + skeletonFilePath
            print "The seq file is " + seqFilePath
            
            # store sequence filename
            self.ui.lineEditSequenceFile.setText(seqFilePath)
            
            # load helix file and store the filename
            self.viewer.loadHelixDataFromFile(helixFilePath)
            self.ui.lineEditHelixLocationFile.setText(helixFilePath)
    
            # load sheet file and store the filename
            self.viewer.loadSheetDataFromFile(sheetFilePath)
            self.ui.lineEditSheetLocationFile.setText(sheetFilePath)
    
            # load skeleton file and store the filename
            self.app.skeletonViewer.loadDataFromFile(skeletonFilePath)
            self.ui.lineEditSkeletonFile.setText(skeletonFilePath)
    
            # store helix length filename
            self.ui.lineEditHelixLengthFile.setText(sseFilePath)
    
            self.setCursor(oldCursor)
            
            self.checkOk()
    
            self.getConstraints()
            print "done loading settings file"
        
    def setConstants(self):
        #Data Sources tab
        #self.viewer.correspondenceEngine.setConstant("SSE_FILE_NAME", str(self.ui.lineEditHelixLengthFile.text()))
        self.viewer.correspondenceEngine.setConstant("VRML_HELIX_FILE_NAME", str(self.ui.lineEditHelixLocationFile.text()))
        self.viewer.correspondenceEngine.setConstant("VRML_SHEET_FILE_NAME", str(self.ui.lineEditSheetLocationFile.text()))
        self.viewer.correspondenceEngine.setConstant("MRC_FILE_NAME", str(self.ui.lineEditSkeletonFile.text()))
        self.sequenceFileName = str(self.ui.lineEditSequenceFile.text())
        self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_NAME", self.sequenceFileName)
        if self.sequenceFileName.split('.')[-1].lower() == 'pdb':
            self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "PDB")
        elif self.sequenceFileName.split('.')[-1].lower() == 'seq':
            self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "SEQ")
        
        #Graph Settings tab
        self.viewer.correspondenceEngine.setConstantInt("BORDER_MARGIN_THRESHOLD", self.ui.spinBoxBorderMarginThreshold.value())
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_DISTANCE_THRESHOLD", self.ui.doubleSpinBoxEuclideanDistance.value())
        self.viewer.correspondenceEngine.setConstant("MAXIMUM_DISTANCE_SHEET_SKELETON", self.ui.doubleSpinBoxMaxSheetDistance.value())
        self.viewer.correspondenceEngine.setConstantInt("MINIMUM_SHEET_SIZE", self.ui.spinBoxMinSheetSize.value())
        self.viewer.correspondenceEngine.setConstant("SHEET_SELF_LOOP_LENGTH", self.ui.doubleSpinBoxSheetSelfLoopLength.value())
        self.viewer.correspondenceEngine.setConstant("SHEET_MERGE_THRESHOLD", self.ui.doubleSpinBoxSheetMergeThreshold.value())
        if (self.ui.checkBoxIncludeStrands.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 1)
        else:
            self.viewer.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 0)

        #Matching Settings tab
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.ui.doubleSpinBoxEuclideanToPDBRatio.value())
        if(self.ui.radioButtonAbsoluteDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 1)
        elif (self.ui.radioButtonNormalizedDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 2)
        else:
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 3)

        self.viewer.correspondenceEngine.setConstant("LOOP_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxLoopImportance.value())
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_LOOP_PENALTY", self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.value())

        self.viewer.correspondenceEngine.setConstant("HELIX_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxHelixImportance.value())
        if(self.ui.checkBoxMissingHelices.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", self.ui.spinBoxMissingHelixCount.value())
        else:
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", -1)
        self.viewer.correspondenceEngine.setConstant("MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxHelixMissingPenalty.value())
        self.viewer.correspondenceEngine.setConstant("MISSING_HELIX_PENALTY_SCALED", self.ui.doubleSpinBoxHelixMissingPenaltyScaled.value())
        self.viewer.correspondenceEngine.setConstant("START_END_MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxEndHelixMissingPenalty.value())
        
        self.viewer.correspondenceEngine.setConstant("SHEET_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxSheetImportance.value())
        if(self.ui.checkBoxMissingSheets.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("MISSING_SHEET_COUNT", self.ui.spinBoxMissingSheetCount.value())
        else:
            self.viewer.correspondenceEngine.setConstantInt("MISSING_SHEET_COUNT", -1)
        self.viewer.correspondenceEngine.setConstant("MISSING_SHEET_PENALTY", self.ui.doubleSpinBoxSheetMissingPenalty.value())
        self.viewer.correspondenceEngine.setConstant("MISSING_SHEET_PENALTY_SCALED", self.ui.doubleSpinBoxSheetMissingPenaltyScaled.value())
        
        # no longer needed?
        self.viewer.correspondenceEngine.setConstantBool("NORMALIZE_GRAPHS", True)

        #Tab 4 User Constraints
        # comment out the constraint clearing so that constraints can be loaded from settings files
        #self.viewer.correspondenceEngine.clearAllConstraints()
        correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
        if(correspondenceIndex >= 0):
            corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
            predictedGraphNode = 1
            nObservedHelices = len(self.viewer.correspondenceLibrary.structureObservation.helixDict)
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]
                self.userConstraints[i] = match.constrained
                if match.predicted.type == 'helix':
                    if match.constrained:
                        if(match.observed):
                            self.viewer.correspondenceEngine.setHelixConstraint(predictedGraphNode, 2*match.observed.label + 1)
                        else:
                            self.viewer.correspondenceEngine.setHelixConstraint(predictedGraphNode, -1)
                if match.predicted.type == 'strand':
                    if(not self.ui.checkBoxIncludeSheets.isChecked()):
                        self.userConstraints[i]=False # clear all strand constraints
                        match.constrained = False     # clear all strand constraints
                        self.viewer.correspondenceEngine.setNodeConstraint(predictedGraphNode, -1)
                    elif(match.constrained):
                        if(match.observed):
                            self.viewer.correspondenceEngine.setNodeConstraint(predictedGraphNode, match.observed.label + nObservedHelices + 1)
                        else:
                            self.viewer.correspondenceEngine.setNodeConstraint(predictedGraphNode, -1)
                if (match.predicted.type) == 'strand':
                    predictedGraphNode += 1
                if (match.predicted.type) == 'helix':
                    predictedGraphNode += 2

    def getConstants(self):
        
        #Graph Settings tab
        self.ui.spinBoxBorderMarginThreshold.setValue(self.viewer.correspondenceEngine.getConstantInt("BORDER_MARGIN_THRESHOLD"))
        self.ui.doubleSpinBoxEuclideanDistance.setValue(self.viewer.correspondenceEngine.getConstantDouble("EUCLIDEAN_DISTANCE_THRESHOLD"))
        self.ui.spinBoxMinSheetSize.setValue(self.viewer.correspondenceEngine.getConstantInt("MINIMUM_SHEET_SIZE"))
        self.ui.doubleSpinBoxMaxSheetDistance.setValue(self.viewer.correspondenceEngine.getConstantDouble("MAXIMUM_DISTANCE_SHEET_SKELETON"))
        self.ui.doubleSpinBoxSheetSelfLoopLength.setValue(self.viewer.correspondenceEngine.getConstantDouble("SHEET_SELF_LOOP_LENGTH"))
        self.ui.doubleSpinBoxSheetMergeThreshold.setValue(self.viewer.correspondenceEngine.getConstantDouble("SHEET_MERGE_THRESHOLD"))
        if(self.viewer.correspondenceEngine.getConstantInt("INCLUDE_STRANDS") == 1):
            self.ui.checkBoxIncludeStrands.setChecked(True)
        else:
            self.ui.checkBoxIncludeStrands.setChecked(False)

        #Matching settings tab
        self.ui.doubleSpinBoxEuclideanToPDBRatio.setValue(self.viewer.correspondenceEngine.getConstantDouble("EUCLIDEAN_VOXEL_TO_PDB_RATIO"))
        if(self.viewer.correspondenceEngine.getConstantInt("COST_FUNCTION") == 1):
            self.ui.radioButtonAbsoluteDifference.setChecked(True)
            self.ui.radioButtonNormalizedDifference.setChecked(False)
            self.ui.radioButtonQuadraticError.setChecked(False)
        elif (self.viewer.correspondenceEngine.getConstantInt("COST_FUNCTION") == 2):
            self.ui.radioButtonAbsoluteDifference.setChecked(False)
            self.ui.radioButtonNormalizedDifference.setChecked(True)
            self.ui.radioButtonQuadraticError.setChecked(False)
        elif (self.viewer.correspondenceEngine.getConstantInt("COST_FUNCTION") == 3):
            self.ui.radioButtonAbsoluteDifference.setChecked(False)
            self.ui.radioButtonNormalizedDifference.setChecked(False)
            self.ui.radioButtonQuadraticError.setChecked(True)

        self.ui.doubleSpinBoxLoopImportance.setValue(self.viewer.correspondenceEngine.getConstantDouble("LOOP_WEIGHT_COEFFICIENT"))
        self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("EUCLIDEAN_LOOP_PENALTY"))

        self.ui.doubleSpinBoxHelixImportance.setValue(self.viewer.correspondenceEngine.getConstantDouble("HELIX_WEIGHT_COEFFICIENT"))
        if (self.viewer.correspondenceEngine.getConstantInt("MISSING_HELIX_COUNT") == -1):
            self.ui.checkBoxMissingHelices.setChecked(False)
        else:
            self.ui.checkBoxMissingHelices.setChecked(True)
            self.ui.spinBoxMissingHelixCount.setValue(self.viewer.correspondenceEngine.getConstantInt("MISSING_HELIX_COUNT"))
        self.ui.doubleSpinBoxHelixMissingPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_HELIX_PENALTY"))
        self.ui.doubleSpinBoxHelixMissingPenaltyScaled.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_HELIX_PENALTY_SCALED"))
        self.ui.doubleSpinBoxEndHelixMissingPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("START_END_MISSING_HELIX_PENALTY"))
        
        self.ui.checkBoxIncludeSheets.setChecked(True)
        self.ui.doubleSpinBoxSheetImportance.setEnabled(True)
        self.ui.doubleSpinBoxSheetImportance.setValue(self.viewer.correspondenceEngine.getConstantDouble("SHEET_WEIGHT_COEFFICIENT"))
        self.ui.checkBoxMissingSheets.setEnabled(True)
        if (self.viewer.correspondenceEngine.getConstantInt("MISSING_SHEET_COUNT") == -1):
            self.ui.checkBoxMissingSheets.setChecked(False)
        else:
            self.ui.checkBoxMissingSheets.setChecked(True)
            self.ui.spinBoxMissingSheetCount.setValue(self.viewer.correspondenceEngine.getConstantInt("MISSING_SHEET_COUNT"))
        self.ui.doubleSpinBoxSheetMissingPenalty.setEnabled(True)
        self.ui.doubleSpinBoxSheetMissingPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_SHEET_PENALTY"))
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setEnabled(True)
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_SHEET_PENALTY_SCALED"))

    def getConstraints(self):
        print "Reading constraints from c++ layer to python layer"

        corr = self.viewer.correspondenceLibrary.correspondenceList[0]
        numH = len(self.viewer.correspondenceLibrary.structureObservation.helixDict)

        # count number of helices and sheets in this correspondence
        hIx = 1
        sIx = 1
        graphIx = 1
        for i in range(len(corr.matchList)):
            match = corr.matchList[i]
            #print "object has type " + str(type(match.predicted))
            if match.predicted is not None:
                if match.predicted.type == 'strand':
                    #print "reading constraints for strand " + str(sIx) + " (graph node " + str(graphIx) + ")"
                    obsSheet = self.viewer.correspondenceEngine.getStrandConstraint(graphIx,0)
                    constrained = (obsSheet != 0)
                    if (obsSheet == -1):
                        sheetNum = -1
                    elif (obsSheet > 0):
                        sheetNum = obsSheet - 2 * numH
                    if constrained:
                        print "strand " + str(sIx) + " (graph node " + str(graphIx) + ") is constrained to sheet " + str(sheetNum) + " (graph node " + str(obsSheet) + ")"
                        self.constrainSSE(i, sheetNum, 0)
                    sIx += 1
                    graphIx += 1
                elif match.predicted.type == 'helix':
                    #print "reading constraints for helix " + str(hIx) + " (graph node " + str(graphIx) + ")"
                    obsHelixFwd = self.viewer.correspondenceEngine.getHelixConstraintFwd(graphIx)
                    obsHelixRev = self.viewer.correspondenceEngine.getHelixConstraintRev(graphIx)
                    obsHelixUnk = self.viewer.correspondenceEngine.getHelixConstraintUnk(graphIx)
                    #print "  fwd constraint = " + str(obsHelixFwd)
                    #print "  rev constraint = " + str(obsHelixRev)
                    #print "  unk constraint = " + str(obsHelixUnk)
                    constrained = False
                    if (obsHelixFwd == -1 or obsHelixRev==-1 or obsHelixUnk==-1):
                        constrained = True
                        helixNum = -1
                    elif (obsHelixFwd != 0):
                        constrained = True
                        helixNum = (obsHelixFwd+1)/2
                        helixDir = 1
                    elif (obsHelixRev != 0):
                        constrained = True
                        helixNum = obsHelixRev/2
                        helixDir = -1
                    elif (obsHelixUnk != 0):
                        constrained = True
                        helixNum = (obsHelixUnk+1)/2
                        helixDir = 0
                    
                    if (constrained):
                        print "Helix " + str(hIx) + " (graph node " + str(graphIx) + ") is constrained to helix " + str(helixNum) + " in direction " + str(helixDir)
                        self.constrainSSE(i, helixNum, helixDir)
                        
                    hIx += 1
                    graphIx += 2
        # now that constraints are stored, clear from c++ class
        self.viewer.correspondenceEngine.clearAllConstraints()

    def populateEmptyResults(self, library):
        """ add empty result before correspondence search is started """

        # create one empty result
        corrList = []
        matchList = []
        
        # build an empty correspondence result by creating an empty match for each secondary structure element.
        for i in range(len(library.structurePrediction.secelDict)):
            observed = None
            predicted = library.structurePrediction.secelDict[i]
            currentMatch = Match(observed, predicted, Match.FORWARD)
            currentMatch.constrained = False
            matchList.append(currentMatch)
        corr = Correspondence(library=library, matchList=matchList, score=0)
        self.lastCorrespondence = corr # used in accept() method
        # add the empty correspondence to the list
        corrList.append(corr)
        return corrList
            
    def populateResults(self, library):

        print "Starting to populate results. Found " + str(self.resultCount) + " results."

        self.ui.tabWidget.setCurrentIndex(4)
        # clear the correspondence list
        corrList = []
        
        # iterate over all results from correspondence algorithm
        #print "Iterating over results."
        for i in range(self.resultCount):
            # create a Correspondence object and add it to the list
            # start from correspondenceEngine result
            result = self.viewer.correspondenceEngine.getResult(i+1)
            matchList = [] # matchList holds the matches
            
            # iterate over all nodes in the matching from correspondenceEngine
            isSecondHelixNode = False
            helicesPassed = 0
            for j in range(result.getNodeCount()):
                if not isSecondHelixNode:
                    direction = Match.FORWARD

                    # predicted helix or strand in sequence graph
                    predicted = library.structurePrediction.secelDict[j - helicesPassed]
                    predictedType = library.structurePrediction.secelType[j - helicesPassed] # 'helix' or 'strand'
                    if predictedType == 'helix':
                        isSecondHelixNode = True

                    # observed helix or sheet in skeleton graph
                    # j is a helix node in the sequence graph
                    # n1 and n2 are skeleton graph nodes for entry and exit points of the helix
                    n1 = result.getSkeletonNode(j)
                    if (n1 < 2 * result.getHelixCount() and n1 >= 0):
                        observedType = 'helix'
                    elif n1 >= 2 * result.getHelixCount():
                        observedType = 'sheet'
                    else:
                        observedType = 'none'
                    isHelix = (n1 < 2 * result.getHelixCount())
                    if observedType == 'helix':
                        isSecondHelixNode = True
                        n2 = result.getSkeletonNode(j+1)
                        if n1 < n2:
                            direction = Match.FORWARD
                        else:
                            direction = Match.REVERSE
                    elif observedType == 'sheet':
                        direction = Match.FORWARD
                                            
                    # lookup which helix from skeleton graph these nodes refer to
                    observedNo = result.nodeToHelix(n1)
                    if observedNo >= 0:
                        if isHelix:
                            # helix in skeleton graph
                            observed = library.structureObservation.helixDict[observedNo]
                        else: # sheet
                            observed = library.structureObservation.sheetDict[observedNo - result.getHelixCount()]
                    else:
                        observed = None
                        
                    # create Match object holding the observed and predicted helices, and the match direction
                    currentMatch = Match(observed, predicted, direction)
                    
                    # if this helix has a constraint, store the constraint in the Match object
                    if(self.userConstraints.has_key(j - helicesPassed)):
                        currentMatch.constrained = self.userConstraints[j - helicesPassed]
                    else:
                        currentMatch.constrained = False
                    # append this match to the list of matches for this result
                    matchList.append(currentMatch)
                             
                else: # isSecondHelixNode is true
                    isSecondHelixNode = False
                    helicesPassed += 1

            # now matchList holds all the helix correspondences for a single match result

            # create Correspondence object for this correspondence
            corr = Correspondence(library=library, matchList=matchList, score=result.getCost())
            
            # add to list of correspondences
            corrList.append(corr)
            
        # corrList now holds all correspondences between the sequence and the graph,
        # as determined by the graphMatch algorithm
        print "Finished populating results."
        return corrList
            
    def populateComboBox(self, library):
        self.ui.comboBoxCorrespondences.clear()
        # add all correspondence to pulldown menu
        for i in range(len(library.correspondenceList)):
            corr = library.correspondenceList[i]
            self.ui.comboBoxCorrespondences.addItem("Correspondence " + str(i+1) + " - [Cost: " + str(corr.score) + "]")
                   
    def createBasicCorrespondence(self):
        """Writes search parameters to correspondence object, loads predicted structure and observed structure, and creates correspondence library"""
        print "creating basic correspondence"
        oldCursor = self.cursor()
        self.setCursor(QtCore.Qt.BusyCursor)

        # put user-entered match parameters from UI into the correspondence object
        print "setting constants"
        self.setConstants()
        
        #Loading Predicted SSEs
        print "loading predicted SSEs"
        self.viewer.correspondenceEngine.loadSequenceGraph()
        
        print "before calling StructurePrediction.load"
        print "sequenceFileName is " + str(self.sequenceFileName)
        print "app is " + str(self.app)
        includeStrands = self.viewer.correspondenceEngine.getConstantInt("INCLUDE_STRANDS")
        structPred = StructurePrediction.load(self.sequenceFileName, self.app, includeStrands)
        print "after calling StructurePrediction.load"
        cAlphaViewer = self.app.calphaViewer
        sseViewer = self.app.sseViewer
        skeletonViewer = self.app.skeletonViewer
        cAlphaViewer.structPred = structPred

        def vector3DFloatToTuple(v3df):
            return (v3df.x(), v3df.y(), v3df.z())
        
        #Loading Observed SSEs
        print "loading observed SSEs"
        self.viewer.correspondenceEngine.loadSkeletonGraph()
        observedHelices = {}
        helixCount = 0
        observedSheets = {}
        sheetCount = 0
        sseCount = self.viewer.correspondenceEngine.getSkeletonSSECount()

        print "adding helices to list of observed helices"
        for sseIx in range(sseCount):
            # call to c++ method QueryEngine::getSkeletonSSE(), which returns a c++ GeometricShape object
            cppSse = self.viewer.correspondenceEngine.getSkeletonSSE(sseIx)
            
            # create list of observed helices for this correspondence result
            if cppSse.isHelix():
                q1 = cAlphaViewer.worldToObjectCoordinates(sseViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell3(1))))
                q2 = cAlphaViewer.worldToObjectCoordinates(sseViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell3(2))))
            
                pyHelix = ObservedHelix(sseIx, q1, q2)
                observedHelices[helixCount] = pyHelix
                helixCount = helixCount + 1

            elif cppSse.isSheet():
                cornerList = {}
                cornerNum = 1
                while True:
                    corner = vector3DFloatToTuple(cppSse.getCornerCell2(cornerNum))
                    lastSheet = ( corner == (0,0,0) )
                    p1 = cAlphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates(corner))
                    if lastSheet:
                        break
                    cornerList[corner] = p1
                    print "adding sheet corner " + str(cornerNum) + " with coordinates (" + str(p1[0]) + "," + str(p1[1]) + "," + str(p1[2]) + ")"
                    cornerNum = cornerNum + 1
                print "done adding sheet corners."
                pySheet = ObservedSheet(sseIx, cornerList)
                # adding sheets to list of observedHelices
                observedSheets[sheetCount] = pySheet
                print "added a sheet to observedSheets. total number of sheets is now " + str(len(observedSheets))
                sheetCount = sheetCount + 1
        
        #TODO: Mike this raises an error!;
        print "found " + str(helixCount) + " helices and " + str(sheetCount) + " sheets"
        structObserv = StructureObservation(helixDict = observedHelices, sheetDict = observedSheets)
        print "writing to correspondenceLibrary"

        # create a new python CorrespondenceLibrary object
        self.viewer.correspondenceLibrary = CorrespondenceLibrary(sp = structPred, so = structObserv)
               
        self.setCursor(oldCursor)
        
        print "finished creating basic correspondences"
        
    def accept(self):
        print "beginning search"
        oldCursor = self.cursor()
        self.setCursor(QtCore.Qt.BusyCursor)

        # save the settings used to generate the last result, in case this search fails
        if self.executed:
            lastCorrespondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            self.lastCorrespondence = self.viewer.correspondenceLibrary.correspondenceList[lastCorrespondenceIndex]
        
        # read user parameters, read skeleton and sequence files, create correspondence library
        self.createBasicCorrespondence()
                
        # execute correspondence query and do cleanup
        memErr = False
        try:
            self.resultCount = self.viewer.correspondenceEngine.executeQuery()
        except MemoryError:
            print "memory error"
            self.resultCount=0
            memErr = True
        self.viewer.correspondenceEngine.cleanupMemory()
        self.viewer.correspondenceEngine.clearAllConstraints()

        print "found " + str(self.resultCount) + " results. cleaning up memory."
        self.setCursor(oldCursor)

        if self.resultCount > 0:
            self.executed = True
            # populate the list of found correspondences
            print "populating result list"
            self.viewer.correspondenceLibrary.correspondenceList = self.populateResults(self.viewer.correspondenceLibrary)

        else:
            self.executed = False
            if memErr:
                QtGui.QMessageBox.warning(self.app, "Insufficient Memory", "Insufficient memory to complete the search. Try adding or removing constraints.")
            else:
                QtGui.QMessageBox.warning(self.app, "No results found", "The correspondence search returned no results. Try removing constraints.")
            print "no results found. loading the most recent successful correspondence"

            self.viewer.correspondenceLibrary.correspondenceList = []
            self.viewer.correspondenceLibrary.correspondenceList.append(self.lastCorrespondence)

        self.populateComboBox(self.viewer.correspondenceLibrary)
        self.viewer.modelChanged()
        self.ui.tabWidget.setCurrentIndex(4)
        print "done with search"
                
    def getIndexedSheetColor(self, index, size):
        """returns a color for sheet 'index' out of 'size' sheets. colors will be orange or red."""
        # start and end are between 0 and 1
        start = float(0.77)
        end = float(1.0)
        delta = (end - start) / float(size)
        i = start + delta * float(index)
        return self.getIndexedColor(i, 1.0)
            
    def getIndexedHelixColor(self, index, size):
        """returns a color for helix 'index' out of 'size' helices. colors will be blue, green, or yellow."""
        # start and end are between 0 and 1
        start = float(0.0)
        end = float(0.75)
        delta = (end - start) / float(size)
        i = start + delta * float(index)
        return self.getIndexedColor(i, 1.0)
    
    #def getIndexedSheetColor(self, index, size):
    #    """returns a color for sheet 'index' out of 'size' sheets. colors will be white or black."""
    #    # start and end are between 0 and 1
    #    position = float(index)/float(size)
    #    return QtGui.QColor.fromRgba(QtGui.qRgba(position*255, position*255, position*255, 1.0*255))

    #def getIndexedHelixColor(self, index, size):
    #    """returns a color for helix 'index' out of 'size' helices. colors will be blue, cyan, green, yellow or red"""
    #    # start and end are between 0 and 1
    #    start = float(0.0)
    #    end = float(1.0)
    #    delta = (end - start) / float(size)
    #    i = start + delta * float(index)
    #    return self.getIndexedColor(i, 1.0)
            
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
            self.ui.tableWidgetCorrespondenceList.setRowCount(2*len(corr.matchList)-1)
            notMissing = {}
            
            # count number of helices and sheets in this correspondence
            helixCount = 0
            sheetCount = 0
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]
                #print "object has type " + str(type(match.predicted))
                if match.predicted is not None:
                    if match.predicted.type == 'strand':
                        # TODO: only count strands that are different. every sheet must be the same color!
                        #print "found strand"
                        sheetCount += 1
                    elif match.predicted.type == 'helix':
                        #print "found helix"
                        helixCount += 1

            helixIndex = 0
            sheetIndex = 0

            for i in range(len(corr.matchList)):
                sseRow = 2*i
                loopRow = 2*i+1
                match = corr.matchList[i]
                #color = self.getIndexedColor(i, len(corr.matchList))
                if match.predicted is not None:
                    if match.predicted.type == 'strand':
                        if(match.observed):
                            color = self.getIndexedSheetColor(match.observed.label - sheetCount, sheetCount)
                        else:
                            color = self.getIndexedColor(0,100)
                        sheetIndex += 1
                    elif match.predicted.type == 'helix':
                        color = self.getIndexedHelixColor(helixIndex, helixCount)
                        helixIndex += 1
                    match.predicted.setColor(color)
                if(match.predicted):
                    cellItemPredicted =  QtGui.QTableWidgetItem(match.predicted.type + " " + str(match.predicted.serialNo) + " : " + str(match.predicted.label) +
                                                                "\n  "  + str(round(match.predicted.getLengthInAngstroms(),2)) + "A length" +
                                                                "\n  "  + str(match.predicted.getResidueCount()) + " residues")
                                                                 
                    cellItemPredicted.setBackgroundColor(color)
                    self.ui.tableWidgetCorrespondenceList.setItem(sseRow, 0, cellItemPredicted)
                if(match.observed):
                    # compute the percentage of results that have the same match
                    matchPercentage = 0.0
                    for corresp in self.viewer.correspondenceLibrary.correspondenceList:
                        if corresp.matchList[i].observed == match.observed:
                            matchPercentage += 1
                    matchPercentage /= len(self.viewer.correspondenceLibrary.correspondenceList)
                    matchPercentage *= 100.0
                    
                    if match.observed.sseType == 'helix':
                        cellItemObserved =  QtGui.QTableWidgetItem("helix " + str(match.observed.label+1) +
                                                                   "\n  " + str(round(match.observed.getLength(), 2)) + "A length" +
                                                                   "\n   (" + str(int(matchPercentage)) + "%) " + str(match.direction) )
                    if match.observed.sseType == 'sheet':
                        cellItemObserved =  QtGui.QTableWidgetItem("sheet " + str(match.observed.label+1) +
                                                                   #"\n  " + str(round(match.observed.getLength(), 2)) + "A length" +
                                                                   "\n   (" + str(int(matchPercentage)) + "%)"  )
                    cellItemObserved.setBackgroundColor(color)
                    self.ui.tableWidgetCorrespondenceList.setItem(sseRow, 1, cellItemObserved)
#                     if match.observed.sseType == 'helix':
#                         # color is stored in two places: the renderer and the correspondence engine. update both.
#                         self.viewer.renderer.setHelixColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
#                         self.viewer.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
#
#                     if match.observed.sseType == 'sheet':
#                         self.viewer.renderer.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
#                         self.viewer.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())

                    notMissing[match.observed.label] = True
            
                checkBox = QtGui.QCheckBox()
                self.ui.tableWidgetCorrespondenceList.setCellWidget(sseRow, 2, checkBox)
                self.connect(checkBox, QtCore.SIGNAL("stateChanged (int)"), self.constraintAdded)
                if(match.constrained):
                    self.ui.tableWidgetCorrespondenceList.cellWidget(sseRow, 2).setCheckState(QtCore.Qt.Checked)
                else:
                    #print "i=" + str(i)
                    self.ui.tableWidgetCorrespondenceList.cellWidget(sseRow, 2).setCheckState(QtCore.Qt.Unchecked)
                    
                self.ui.tableWidgetCorrespondenceList.resizeRowToContents(sseRow)

                # add row with loop info
                if (i < len(corr.matchList)-1 ):
                    resCount = corr.matchList[i+1].predicted.startIndex - corr.matchList[i].predicted.stopIndex
                    cellItemLoop =  QtGui.QTableWidgetItem("loop: " + str(resCount) + " residues")
                    self.ui.tableWidgetCorrespondenceList.setItem(loopRow, 0, cellItemLoop)
                    self.ui.tableWidgetCorrespondenceList.resizeRowToContents(loopRow)
     
            # relabel rows of table
            rowLabels = []
            for i in range(len(corr.matchList)):
                rowLabels.append(str(i+1))
                if i < len(corr.matchList)-1:
                    rowLabels.append(" ")
            #print "rowLabels string has " + str(len(rowLabels)) + " elements."
            self.ui.tableWidgetCorrespondenceList.setVerticalHeaderLabels(rowLabels)
            
            observedHelices = self.viewer.correspondenceLibrary.structureObservation.helixDict
#             for i in range(len(observedHelices)):
#                 if(not notMissing.has_key(i)):
#                     self.viewer.renderer.setHelixColor(i, 0.5, 0.5, 0.5, 1.0)
                
#         self.viewer.correspondenceEngine.setVisibleCorrespondence(correspondenceIndex)
#         self.viewer.correspondenceLibrary.setCurrentCorrespondenceIndex(correspondenceIndex)
        self.viewer.modelChanged()
        self.loadingCorrespondance = False
        
    def drawOverlay(self):
        if self.executed and self.corrAct.isChecked():
            glPushAttrib(GL_LIGHTING_BIT)
            self.viewer.setMaterials(self.colors["CorrespondenceFinder:BackboneTrace"])
            # calls Draw method of c++ SSECorrespondenceEngine object
            self.viewer.correspondenceEngine.draw(0)
            glPopAttrib()
        if self.corrAct.isChecked() and self.dataLoaded and (self.ui.checkBoxShowAllPaths.isChecked() or self.ui.checkBoxShowHelixCorners.isChecked() or self.ui.checkBoxShowSheetCorners.isChecked() or self.ui.checkBoxShowSheetColors.isChecked() ):
            # TODO: Move this color changing code somewhere else
            # set colors of all SSEs
            # Probably should use the setColor calls in previous sections.
            for i in range(self.viewer.correspondenceEngine.getSkeletonSSECount()):
                color = self.getIndexedHelixColor(i, self.viewer.correspondenceEngine.getSkeletonSSECount())
            glPushAttrib(GL_LIGHTING_BIT)
            self.viewer.setMaterials(self.colors["CorrespondenceFinder:BackboneTrace"])
            self.viewer.correspondenceEngine.drawAllPaths(0,self.ui.checkBoxShowAllPaths.isChecked(),self.ui.checkBoxShowHelixCorners.isChecked(),self.ui.checkBoxShowSheetCorners.isChecked(),False)
            glPopAttrib()
            
    def rebuildGraph(self):
        print "correspondence index before rebuilding is "
        print self.ui.comboBoxCorrespondences.currentIndex()
        self.ui.comboBoxCorrespondences.setCurrentIndex(-1)
        print "correspondence index after setting to -1 is "
        print self.ui.comboBoxCorrespondences.currentIndex()
        self.setConstants()
        self.checkOk()
        self.viewer.makeSheetSurfaces(self.app.skeletonViewer.renderer.getOriginX(), self.app.skeletonViewer.renderer.getOriginY(), self.app.skeletonViewer.renderer.getOriginZ(), self.app.skeletonViewer.renderer.getSpacingX(), self.app.skeletonViewer.renderer.getSpacingY(), self.app.skeletonViewer.renderer.getSpacingZ())
        self.viewer.modelChanged()
        print "correspondence index after rebuilding is "
        print self.ui.comboBoxCorrespondences.currentIndex()
        
    def constraintAdded(self, state):
        if(not self.loadingCorrespondance):
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
                for i in range(len(corr.matchList)):
                    match = corr.matchList[i]
                    match.constrained = (self.ui.tableWidgetCorrespondenceList.cellWidget(2*i, 2).checkState() == QtCore.Qt.Checked)
                    
    def createActionsForCell(self, row, col):
        self.selectedRow = row/2
        for act in self.ui.tableWidgetCorrespondenceList.actions()[:]:
            self.ui.tableWidgetCorrespondenceList.removeAction(act)
        if(col == 1 and row%2==0):
            observedHelices = self.viewer.correspondenceLibrary.structureObservation.helixDict
            observedSheets = self.viewer.correspondenceLibrary.structureObservation.sheetDict
            constrained = {}
            
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
                for i in range(len(corr.matchList)):
                    match = corr.matchList[i]
                    if(match.constrained and match.observed):
                        constrained[match.observed.label] = True
                match = corr.matchList[row/2]
                print "match at row=" + str(row)
            else:
                match = False
            if match.predicted.type == 'helix':
                for i in range(len(observedHelices)):
                    constrainAction = QtGui.QAction(self.tr("Observed helix " + str(i+1) + " (Length: " + str(round(observedHelices[i].getLength(), 2)) + "A)"), self)
                    constrainAction.setCheckable(True)
                    if(match and match.observed):
                        constrainAction.setChecked(match.observed.label == i)
                    else:
                        constrainAction.setChecked(False)
                    constrainAction.setEnabled(not constrained.has_key(i))
                    self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedHelix(i))
                    self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)
            if match.predicted.type == 'strand' and self.ui.checkBoxIncludeSheets.isChecked():
                numH = len(observedHelices)
                for i in range(len(observedSheets)):
                    constrainAction = QtGui.QAction(self.tr("Observed sheet " + str(i+numH+1)), self)
                    #constrainAction = QtGui.QAction(self.tr("Observed sheet " + str(i+numH+1) + " (Area: " + str(round(observedSheets[i].getSize(), 2)) + " voxels)"), self)
                    constrainAction.setCheckable(True)
                    if(match and match.observed):
                        constrainAction.setChecked(match.observed.label == i)
                    else:
                        constrainAction.setChecked(False)
                    constrainAction.setEnabled(True)
                    self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedSheet(i))
                    self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)
                
            constrainAction = QtGui.QAction(self.tr("Not observed"), self)
            constrainAction.setCheckable(True)
            constrainAction.setChecked(match and not match.observed)
            constrainAction.setEnabled(True)
            self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedHelix(-1))
            self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)
