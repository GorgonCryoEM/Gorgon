# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that can be used to find correspondance between helices


from PyQt4 import QtCore, QtGui
from ui_dialog_sse_helix_correspondence_finder import Ui_DialogSSEHelixCorrespondenceFinder
from libpyGORGON import SSECorrespondenceEngine, SSECorrespondenceResult, Vector3DFloat
from correspondence.CorrespondenceLibrary import CorrespondenceLibrary
from correspondence.Correspondence import Correspondence
from correspondence.Match import Match
from correspondence.ObservedHelix import ObservedHelix
from correspondence.ObservedSheet import ObservedSheet
from correspondence.StructureObservation import StructureObservation
from correspondence.StructurePrediction import StructurePrediction
from seq_model.Helix import Helix
from vector_lib import *
import xml.dom.minidom
from base_dock_widget import BaseDockWidget

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import math


class SSEHelixCorrespondenceFinderForm(BaseDockWidget):

    def __init__(self, main, viewer, parent=None):
        BaseDockWidget.__init__(self,
                               main,
                               "Find SSE Correspondences",
                               "Find the correspondence between observed and predicted SSEs",
                               "perform_SSEFindHelixCorrespondences",
                               "actions-sse-findhelixcorrespondences",
                               "actions-sse",
                               QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea,
                               QtCore.Qt.RightDockWidgetArea,
                               parent)
        self.executed = False
        self.app = main
        self.app.themes.addDefaultRGB("CorrespondenceFinder:BackboneTrace", 255, 255, 255, 255)
        self.viewer = viewer
        self.createUI()
        self.createActions()
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
#         self.connect(self.ui.pushButtonReset, QtCore.SIGNAL("pressed ()"), self.loadDefaults)
        self.connect(self.ui.pushButtonOk, QtCore.SIGNAL("pressed ()"), self.accept)
        self.connect(self.ui.checkBoxShowAllPaths, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
        self.connect(self.ui.checkBoxShowSheetCorners, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
        self.connect(self.ui.checkBoxShowHelixCorners, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
        self.connect(self.ui.checkBoxShowSheetColors, QtCore.SIGNAL("toggled (bool)"), self.graphSheetVisibilityChanged)
        self.connect(self.ui.checkBoxShowSkeleton, QtCore.SIGNAL("toggled (bool)"), self.skeletonVisibilityChanged)
        self.connect(self.ui.checkBoxShowSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetVisibilityChanged)
        self.connect(self.ui.checkBoxShowHelices, QtCore.SIGNAL("toggled (bool)"), self.helixVisibilityChanged)
        self.connect(self.ui.checkBoxIncludeSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetIncludeChanged)
        self.connect(self.ui.checkBoxMissingSheets, QtCore.SIGNAL("toggled (bool)"), self.missingSheetChanged)
        self.connect(self.ui.checkBoxMissingHelices, QtCore.SIGNAL("toggled (bool)"), self.missingHelixChanged)
        self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("modelDrawing()"), self.drawOverlay)
        self.ui.tableWidgetCorrespondenceList.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.ui.label.setVisible(False)
        self.ui.lineEditHelixLengthFile.setVisible(False)
        self.ui.pushButtonGetHelixLengthFile.setVisible(False)
          
    def loadDefaultParams(self):
        self.ui.pushButtonExportToRosetta.setVisible(False)
        # Graph Settings tab
        self.ui.spinBoxBorderMarginThreshold.setValue(5)
        self.ui.doubleSpinBoxEuclideanDistance.setValue(0.0)
        self.ui.checkBoxIncludeSheets.setChecked(True)
        #min sheet size
        #max sheet distance
        #sheet self-loop length

        self.ui.checkBoxShowSkeleton.setChecked(self.app.viewers['skeleton'].visualizationOptions.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelices.setChecked(self.app.viewers['sse'].visualizationOptions.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelixCorners.setChecked(False)
        self.ui.checkBoxShowSheets.setChecked(self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel2Visible.isChecked())
        self.ui.checkBoxShowSheetColors.setChecked(self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel3Visible.isChecked())
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
        self.bringToFront()

    def getHelixLocationFile(self):
        self.viewer.loadHelixData()
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.checkOk()
        self.bringToFront()

    def getSheetLocationFile(self):
        self.viewer.loadSheetData()
        self.ui.lineEditSheetLocationFile.setText(self.viewer.sheetFileName)
        self.checkOk()
        self.bringToFront()

    def getSkeletonFile(self):
        self.app.viewers["skeleton"].loadData()
        self.ui.lineEditSkeletonFile.setText(self.app.viewers["skeleton"].fileName)
        self.checkOk()
        self.bringToFront()
        
    def getSequenceFile(self):
        """
        This loads a SEQ file or, for testing purposes, a PDB file.
        """
        self.ui.lineEditSequenceFile.setText(self.openFile("Load Sequence File", "Sequence with SSE predictions (*.seq)\nPDB Helix Annotations (*.pdb)"))
        self.checkOk()
        self.bringToFront()

    def getSettingsFile(self):
        """
        This loads a settings file, which contains filenames and search parameters.
        """
        self.ui.lineEditSettingsFile.setText(self.openFile("Load Settings File", "Settings File (*.txt)"))
        self.loadSettings()
        self.bringToFront()
        
    def checkOk(self):
        """
        This checks if all files necessary for the correspondence search have been loaded. If so, the
        correspondence search parameter tabs are enabled and a basic correspondence is created.   
        """
        print "begin checkOk"
        print "correspondence index at beginning is "
        print self.ui.comboBoxCorrespondences.currentIndex()

        allLoaded = not(self.ui.lineEditHelixLocationFile.text().isEmpty()
                           or self.ui.lineEditSheetLocationFile.text().isEmpty()
                           or self.ui.lineEditSkeletonFile.text().isEmpty()
                           or self.ui.lineEditSequenceFile.text().isEmpty())

        self.dataLoaded = not((self.ui.lineEditHelixLocationFile.text().isEmpty()
                           and self.ui.lineEditSheetLocationFile.text().isEmpty())
                           or self.ui.lineEditSkeletonFile.text().isEmpty()
                           or self.ui.lineEditSequenceFile.text().isEmpty())
        self.ui.pushButtonOk.setEnabled(self.dataLoaded)
        
        self.ui.tabWidget.setTabEnabled(1, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(2, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(3, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(4, self.dataLoaded)
        if(self.dataLoaded):
            self.executed = False
            self.createBasicCorrespondence()
            print "after creating basic correspondence (1), secelDict has length " + str(len(self.viewer.correspondenceLibrary.structurePrediction.secelDict))
            #self.createBasicCorrespondence()
            print "after creating basic correspondence (2), secelDict has length " + str(len(self.viewer.correspondenceLibrary.structurePrediction.secelDict))
            self.viewer.correspondenceLibrary.correspondenceList = self.populateEmptyResults(self.viewer.correspondenceLibrary)
            print "correspondenceList has length " + str(len(self.viewer.correspondenceLibrary.correspondenceList))
            self.populateComboBox(self.viewer.correspondenceLibrary)
            self.viewer.makeSheetSurfaces(self.app.viewers['skeleton'].renderer.getOriginX(), self.app.viewers['skeleton'].renderer.getOriginY(), self.app.viewers['skeleton'].renderer.getOriginZ(), self.app.viewers['skeleton'].renderer.getSpacingX(), self.app.viewers['skeleton'].renderer.getSpacingY(), self.app.viewers['skeleton'].renderer.getSpacingZ())
            if(allLoaded):
                self.ui.tabWidget.setCurrentIndex(1)
        else:
            print "data not loaded"
        print "correspondence index at end is " + str(self.ui.comboBoxCorrespondences.currentIndex())
        print "end checkOk"
    
    def loadWidget(self):
        BaseDockWidget.loadWidget(self)

    def fullGraphVisibilityChanged(self, visible):
        """Called when the visibility checkbox is checked."""
        # to render again
        self.viewer.emitModelChanged()
    
    def skeletonVisibilityChanged(self, visible):
        """Called when the show skeleton checkbox is checked."""
        self.app.viewers['skeleton'].visualizationOptions.ui.checkBoxModelVisible.setChecked(visible)
        self.app.viewers['skeleton'].visualizationOptions.ui.checkBoxModel2Visible.setChecked(visible)
        self.app.viewers['skeleton'].visualizationOptions.ui.checkBoxModel3Visible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()
            
    def sheetVisibilityChanged(self, visible):
        """Called when the show sheet checkbox is checked."""
        #self.visualizationOptions.ui.checkBoxModel2Visible.setChecked(visible)
        self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel2Visible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()

    def graphSheetVisibilityChanged(self, visible):
        """Called when the show graph sheet checkbox is checked."""
        self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel3Visible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()

    def helixVisibilityChanged(self, visible):
        """Called when the show helix checkbox is checked."""
        self.app.viewers['sse'].visualizationOptions.ui.checkBoxModelVisible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()
            
    def sheetIncludeChanged(self, include):
        """Called when the include sheets checkbox is checked."""
        self.ui.doubleSpinBoxSheetImportance.setEnabled(include)
        self.ui.doubleSpinBoxSheetMissingPenalty.setEnabled(include)
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setEnabled(include)
        self.ui.checkBoxMissingSheets.setEnabled(include)
        self.ui.spinBoxMissingSheetCount.setEnabled(include and self.ui.checkBoxMissingSheets.isChecked())

    def missingHelixChanged(self, checked):
        """Called when the missing helices checkbox is checked."""
        self.ui.spinBoxMissingHelixCount.setEnabled(checked)

    def missingSheetChanged(self, checked):
        """Called when the missing helices checkbox is checked."""
        self.ui.spinBoxMissingSheetCount.setEnabled(checked)

    def modelChanged(self):
        if(not self.viewer.loaded) and self.app.actions.getAction("perform_SSEFindHelixCorrespondences").isChecked():
            self.app.actions.getAction("perform_SSEFindHelixCorrespondences").trigger()
        
    def createActions(self):
        self.corrAct = self.displayAct
        self.connect(self.displayAct, QtCore.SIGNAL("toggled (bool)"), self.visibilityChanged)
    
    def visibilityChanged(self, visible):
        self.viewer.renderer.setObjectSpecificColoring(visible)
        if(not visible):
            self.viewer.visualizationOptions.ui.checkBoxModel3Visible.setChecked(False)
            self.viewer.visualizationOptions.ui.checkBoxModel3Visible.setVisible(False)
            self.viewer.visualizationOptions.ui.pushButtonModel3Color.setVisible(False)
        else:
            self.viewer.visualizationOptions.ui.pushButtonModel3Color.setVisible(True)
            self.viewer.visualizationOptions.ui.checkBoxModel3Visible.setVisible(True)
            self.viewer.visualizationOptions.ui.checkBoxModel3Visible.setChecked(self.ui.checkBoxShowSheetColors.isChecked())
        self.viewer.emitModelChanged()
      
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
            self.app.viewers["skeleton"].loadDataFromFile(skeletonFilePath)
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
#         cAlphaViewer = self.app.viewers['calpha']
        sseViewer = self.app.viewers['sse']
        skeletonViewer = self.app.viewers['skeleton']
#         cAlphaViewer.structPred = structPred

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
                q1 = vector3DFloatToTuple(cppSse.getCornerCell3(1))
                q2 = vector3DFloatToTuple(cppSse.getCornerCell3(2))
            
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
        self.viewer.emitModelChanged()
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
        
    def drawOverlay(self):
        if self.executed and self.corrAct.isChecked():
            glPushAttrib(GL_LIGHTING_BIT)
            self.viewer.setMaterials(self.app.themes.getColor("CorrespondenceFinder:BackboneTrace"))
            # calls Draw method of c++ SSECorrespondenceEngine object
            self.viewer.correspondenceEngine.draw(0)
            glPopAttrib()
