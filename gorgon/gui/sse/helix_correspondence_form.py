from PyQt4 import QtCore, QtGui
from ui_helix_correspondence_finder import Ui_DialogSSEHelixCorrespondenceFinder
from ...toolkit.libpytoolkit import SSEEngine, SSEResult, IBackEnd#, Vec3F
from ...toolkit.libpytoolkit import setHelixConstraint
from ...toolkit.sse.correspondence.CorrespondenceLibrary import CorrespondenceLibrary
from ...toolkit.sse.correspondence.Correspondence import Correspondence
from ...toolkit.sse.correspondence.Match import Match
from ...toolkit.sse.correspondence.ObservedHelix import ObservedHelix
# from ...toolkit.sse.correspondence.ObservedSheet import ObservedSheet
from ...toolkit.sse.correspondence.StructureObservation import StructureObservation
from ...toolkit.sse.correspondence.StructurePrediction import StructurePrediction
# from ...toolkit.sse.seq_model.Helix import Helix
# from ...Explorer import Vec3
import xml.dom.minidom

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

# import math
import termcolor
from ..helix import create_helix

from ...toolkit.libpytoolkit import SSEEngine
from viewer import SSEViewer
from ...toolkit import HelixCorrespondence

class SSEHelixCorrespondenceForm(QtGui.QDialog, HelixCorrespondence):

    def __init__(self, parent):
        self.parent = parent
        self.args = self.parent.args
        self.correspondenceLibrary = CorrespondenceLibrary()

        QtGui.QDialog.__init__(self, self.parent)
        HelixCorrespondence.__init__(self, self.args)
        
        dock = QtGui.QDockWidget("SSEHelixCorrespondenceFinder", self.parent)
        dock.setWidget(self)
        dock.setAllowedAreas(QtCore.Qt.AllDockWidgetAreas)
        self.parent.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)
        
        self.executed = False
        self.colors = {}
        self.colors["CorrespondenceFinder:BackboneTrace"] = QtGui.QColor(200, 155, 155, 180)
        self.loadingCorrespondance = False
        self.userConstraints = {}
        self.constraintActions = {}
        self.selectedRow = 0
        self.dataLoaded = True
        self.allLoaded  = True

        # KLUDGE: This class should be drawn as itself,
        # not by injecting its drawing function into SSEViewer
        SSEViewer.extraDrawingRoutines = self.extraDrawingRoutines

#         exit()

    def createUI(self):
        self.sse = self.parent.sse
        self.ui = Ui_DialogSSEHelixCorrespondenceFinder()
        self.ui.setupUi(self)
#         self.connect(self.ui.pushButtonGetHelixLengthFile, QtCore.SIGNAL("pressed ()"), self.getHelixLengthFile)
#         self.connect(self.ui.pushButtonGetHelixLocationFile, QtCore.SIGNAL("pressed ()"), self.getHelixLocationFile)
#         self.connect(self.ui.pushButtonGetSheetLocationFile, QtCore.SIGNAL("pressed ()"), self.getSheetLocationFile)
#         self.connect(self.ui.pushButtonGetSkeletonFile, QtCore.SIGNAL("pressed ()"), self.getSkeletonFile)
#         self.connect(self.ui.pushButtonGetSequenceFile, QtCore.SIGNAL("pressed ()"), self.getSequenceFile)
#         self.connect(self.ui.pushButtonGetSettingsFile, QtCore.SIGNAL("pressed ()"), self.getSettingsFile)
#         self.connect(self.ui.pushButtonReset, QtCore.SIGNAL("pressed ()"), self.loadDefaults)
        self.connect(self.ui.pushButtonAddHelices, QtCore.SIGNAL("pressed ()"), self.create_all_helices)
        self.connect(self.ui.pushButtonOk, QtCore.SIGNAL("pressed ()"), self.accept)
        self.connect(self.ui.comboBoxCorrespondences, QtCore.SIGNAL("currentIndexChanged (int)"), self.selectCorrespondence)
#         self.connect(self.ui.pushButtonExportToRosetta, QtCore.SIGNAL("pressed ()"), self.exportToRosetta)
        self.ui.checkBoxShowAllPaths.clicked.connect(self.sse.modelChanged)
        self.ui.checkBoxShowSheetCorners.clicked.connect(self.sse.modelChanged)
        self.ui.checkBoxShowHelixCorners.clicked.connect(self.sse.modelChanged)
#         self.connect(self.ui.checkBoxShowSheetColors, QtCore.SIGNAL("toggled (bool)"), self.graphSheetVisibilityChanged)
        self.ui.checkBoxShowSkeleton.clicked.connect(self.parent.skeleton.setModelVisibility)
#         self.connect(self.ui.checkBoxShowSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetVisibilityChanged)
        self.ui.checkBoxShowHelices.clicked.connect(self.sse.setModelVisibility)
#         self.connect(self.ui.checkBoxIncludeSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetIncludeChanged)
#         self.connect(self.ui.checkBoxMissingSheets, QtCore.SIGNAL("toggled (bool)"), self.missingSheetChanged)
#         self.connect(self.ui.checkBoxMissingHelices, QtCore.SIGNAL("toggled (bool)"), self.missingHelixChanged)
#         self.connect(self.parent.skeleton, QtCore.SIGNAL("modelDrawing()"), self.drawOverlay)
#         self.ui.tableWidgetCorrespondenceList.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
#         self.connect(self.ui.tableWidgetCorrespondenceList, QtCore.SIGNAL("customContextMenuRequested (const QPoint&)"), self.customMenuRequested)
        self.connect(self.sse, QtCore.SIGNAL("elementClicked (int, int, int, int, int, int, QMouseEvent)"), self.sseClicked)
        self.ui.label.setVisible(False)
        self.ui.lineEditHelixLengthFile.setVisible(False)
        self.ui.pushButtonGetHelixLengthFile.setVisible(False)

        self.ui.checkBoxShowSkeleton.setChecked(self.parent.skeleton.ui.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelices.setChecked(self.sse.ui.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelixCorners.setChecked(False)
        self.ui.checkBoxShowSheetCorners.setChecked(False)
        self.ui.checkBoxShowAllPaths.setChecked(False)
          
#     # populate parameter boxes with default values for correspondence search
#     def loadDefaults(self):
#         self.ui.lineEditHelixLengthFile.setText("")
#         self.ui.lineEditHelixLocationFile.setText(self.sse.helixFileName)
#         self.ui.lineEditSheetLocationFile.setText(self.sse.sheetFileName)
#         self.ui.lineEditSkeletonFile.setText(self.parent.skeleton.fileName)
#         self.ui.lineEditSequenceFile.setText(self.parent.calpha.fileName)
#         self.ui.lineEditSettingsFile.setText("")
#         self.loadDefaultParams()

#     def openFile(self, title, fileformats):
#         fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr(title), "", self.tr(fileformats))
#         return fileName
#
#     def getHelixLengthFile(self):
#         self.ui.lineEditHelixLengthFile.setText(self.openFile("Load Helix Length File", "SSE Hunter results (*.sse)"))
#         self.checkOk()
# #         self.bringToFront()
#
#     def getHelixLocationFile(self):
#         self.sse.loadHelixData()
#         self.ui.lineEditHelixLocationFile.setText(self.sse.helixFileName)
#         self.checkOk()
# #         self.bringToFront()
#
#     def getSheetLocationFile(self):
#         self.sse.loadSheetData()
#         self.ui.lineEditSheetLocationFile.setText(self.sse.sheetFileName)
#         self.checkOk()
# #         self.bringToFront()
#
#     def getSkeletonFile(self):
#         self.parent.skeleton.loadData()
#         self.ui.lineEditSkeletonFile.setText(self.parent.skeleton.fileName)
#         self.checkOk()
# #         self.bringToFront()
#
#     def getSequenceFile(self):
#         """
#         This loads a SEQ file or, for testing purposes, a PDB file.
#         """
#         self.ui.lineEditSequenceFile.setText(self.openFile("Load Sequence File", "Sequence with SSE predictions (*.seq)\nPDB Helix Annotations (*.pdb)"))
#         self.checkOk()
# #         self.bringToFront()
#
#     def getSettingsFile(self):
#         """
#         This loads a settings file, which contains filenames and search parameters.
#         """
#         self.ui.lineEditSettingsFile.setText(self.openFile("Load Settings File", "Settings File (*.txt)"))
#         self.loadSettings()
# #         self.bringToFront()
        
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
            print "after creating basic correspondence (1), secelDict has length " + str(len(self.correspondenceLibrary.structurePrediction.secelDict))
            #self.createBasicCorrespondence()
            print "after creating basic correspondence (2), secelDict has length " + str(len(self.correspondenceLibrary.structurePrediction.secelDict))

            self.correspondenceLibrary.correspondenceList = self.populateEmptyResults(self.correspondenceLibrary)
            print "correspondenceList has length " + str(len(self.correspondenceLibrary.correspondenceList))
            self.populateComboBox(self.correspondenceLibrary)
            self.makeSheetSurfaces(self.parent.skeleton.renderer.getOriginX(),
                                       self.parent.skeleton.renderer.getOriginY(),
                                       self.parent.skeleton.renderer.getOriginZ(),
                                       self.parent.skeleton.renderer.getSpacingX(),
                                       self.parent.skeleton.renderer.getSpacingY(),
                                       self.parent.skeleton.renderer.getSpacingZ())
            if(allLoaded):
                self.ui.tabWidget.setCurrentIndex(1)
        else:
            print "data not loaded"
        print "correspondence index at end is " + str(self.ui.comboBoxCorrespondences.currentIndex())
        print "end checkOk"
    
#     def loadSettings(self):
#
#         if (not(self.ui.lineEditSettingsFile.text().isEmpty())):
#
#             print "loading default parameters"
#             self.loadDefaultParams()
#
#             print "loading settings file"
#             oldCursor = self.cursor()
#             self.setCursor(QtCore.Qt.BusyCursor)
#
#             print "calling setConstantsFromFile"
#             self.setConstants()
#             self.correspondenceEngine.setConstantsFromFile(str(self.ui.lineEditSettingsFile.text()))
#
#             print "copying constants from file to GUI"
#             self.getConstants()
#
#             print "getting settings filename"
#             settingsFilename = str(self.ui.lineEditSettingsFile.text())
#
#             #self.correspondenceEngine.printConstants
#             print "reading other filenames for parsing"
#             helixFilename = self.correspondenceEngine.getConstantString("VRML_HELIX_FILE_NAME")
#             print "helix filename is " + helixFilename
#             sheetFilename = self.correspondenceEngine.getConstantString("VRML_SHEET_FILE_NAME")
#             print "sheet filename is " + sheetFilename
#             sequenceFilename = self.correspondenceEngine.getConstantString("SEQUENCE_FILE_NAME")
#             print "sequence filename is " + sequenceFilename
#             skeletonFilename = self.correspondenceEngine.getConstantString("MRC_FILE_NAME")
#             print "skeleton filename is " + skeletonFilename
#             sseFilename = self.correspondenceEngine.getConstantString("SSE_FILE_NAME")
#             print "sse filename is " + sseFilename
#
#             import os
#             path,settingsFile = os.path.split(settingsFilename)
#             ssePath,sseFile = os.path.split(sseFilename)
#             helixPath,helixFile = os.path.split(helixFilename)
#             sheetPath,sheetFile = os.path.split(sheetFilename)
#             skeletonPath,skeletonFile = os.path.split(skeletonFilename)
#             seqPath,seqFile = os.path.split(sequenceFilename)
#
#             print "The path (raw) is " + path
#             path = path.replace('/', os.sep)  # replace forward slash with separator for this OS
#             path = path.replace('\\', os.sep) # replace backslash with separator for this OS
#             print "The path (fixed) is " + path
#
#             settingsFilePath = path + os.sep + settingsFile
#             sseFilePath = path + os.sep + sseFile
#             helixFilePath = path + os.sep + helixFile
#             sheetFilePath = path + os.sep + sheetFile
#             skeletonFilePath = path + os.sep + skeletonFile
#             seqFilePath = path + os.sep + seqFile
#
#             print "The settings file is " + settingsFilePath
#             print "The sse file is " + sseFilePath
#             print "The helix file is " + helixFilePath
#             print "The sheet file is " + sheetFilePath
#             print "The skeleton file is " + skeletonFilePath
#             print "The seq file is " + seqFilePath
#
#             # store sequence filename
#             self.ui.lineEditSequenceFile.setText(seqFilePath)
#
#             # load helix file and store the filename
#             self.sse.loadHelixDataFromFile(helixFilePath)
#             self.ui.lineEditHelixLocationFile.setText(helixFilePath)
#
#             # load sheet file and store the filename
#             self.sse.loadSheetDataFromFile(sheetFilePath)
#             self.ui.lineEditSheetLocationFile.setText(sheetFilePath)
#
#             # load skeleton file and store the filename
#             self.parent.skeleton.loadDataFromFile(skeletonFilePath)
#             self.ui.lineEditSkeletonFile.setText(skeletonFilePath)
#
#             # store helix length filename
#             self.ui.lineEditHelixLengthFile.setText(sseFilePath)
#
#             self.setCursor(oldCursor)
#
#             self.checkOk()
#
#             self.getConstraints()
#             print "done loading settings file"
        
    def setConstants(self):
        HelixCorrespondence.setConstants(self)
        # #Tab 4 User Constraints
        # # comment out the constraint clearing so that constraints can be loaded from settings files
        # #self.correspondenceEngine.clearAllConstraints()
        # correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
        # if(correspondenceIndex >= 0):
        #     corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
        #     predictedGraphNode = 1
        #     nObservedHelices = len(self.correspondenceLibrary.structureObservation.helixDict)
        #     for i in range(len(corr.matchList)):
        #         match = corr.matchList[i]
        #         self.userConstraints[i] = match.constrained
        #         if match.predicted.type == 'helix':
        #             if match.constrained:
        #                 if(match.observed):
        #                     setHelixConstraint(predictedGraphNode, 2*match.observed.label + 1)
        #                 else:
        #                     setHelixConstraint(predictedGraphNode, -1)
        #         if match.predicted.type == 'strand':
        #             if(not self.ui.checkBoxIncludeSheets.isChecked()):
        #                 self.userConstraints[i]=False # clear all strand constraints
        #                 match.constrained = False     # clear all strand constraints
        #                 self.correspondenceEngine.setNodeConstraint(predictedGraphNode, -1)
        #             elif(match.constrained):
        #                 if(match.observed):
        #                     self.correspondenceEngine.setNodeConstraint(predictedGraphNode, match.observed.label + nObservedHelices + 1)
        #                 else:
        #                     self.correspondenceEngine.setNodeConstraint(predictedGraphNode, -1)
        #         if (match.predicted.type) == 'strand':
        #             predictedGraphNode += 1
        #         if (match.predicted.type) == 'helix':
        #             predictedGraphNode += 2

    def getConstants(self):
        
        #Graph Settings tab
        self.ui.spinBoxBorderMarginThreshold.setValue(self.constants.getConstantInt("BORDER_MARGIN_THRESHOLD"))
        self.ui.doubleSpinBoxEuclideanDistance.setValue(self.constants.getConstantDouble("EUCLIDEAN_DISTANCE_THRESHOLD"))
        self.ui.spinBoxMinSheetSize.setValue(self.constants.getConstantInt("MINIMUM_SHEET_SIZE"))
        self.ui.doubleSpinBoxMaxSheetDistance.setValue(self.constants.getConstantDouble("MAXIMUM_DISTANCE_SHEET_SKELETON"))
        self.ui.doubleSpinBoxSheetSelfLoopLength.setValue(self.constants.getConstantDouble("SHEET_SELF_LOOP_LENGTH"))
        self.ui.doubleSpinBoxSheetMergeThreshold.setValue(self.constants.getConstantDouble("SHEET_MERGE_THRESHOLD"))
        if(self.constants.getConstantInt("INCLUDE_STRANDS") == 1):
            self.ui.checkBoxIncludeStrands.setChecked(True)
        else:
            self.ui.checkBoxIncludeStrands.setChecked(False)
    
        #Matching settings tab
        self.ui.doubleSpinBoxEuclideanToPDBRatio.setValue(self.constants.getConstantDouble("EUCLIDEAN_VOXEL_TO_PDB_RATIO"))
        if(self.constants.getConstantInt("COST_FUNCTION") == 1):
            self.ui.radioButtonAbsoluteDifference.setChecked(True)
            self.ui.radioButtonNormalizedDifference.setChecked(False)
            self.ui.radioButtonQuadraticError.setChecked(False)
        elif (self.constants.getConstantInt("COST_FUNCTION") == 2):
            self.ui.radioButtonAbsoluteDifference.setChecked(False)
            self.ui.radioButtonNormalizedDifference.setChecked(True)
            self.ui.radioButtonQuadraticError.setChecked(False)
        elif (self.constants.getConstantInt("COST_FUNCTION") == 3):
            self.ui.radioButtonAbsoluteDifference.setChecked(False)
            self.ui.radioButtonNormalizedDifference.setChecked(False)
            self.ui.radioButtonQuadraticError.setChecked(True)
    
        self.ui.doubleSpinBoxLoopImportance.setValue(self.constants.getConstantDouble("LOOP_WEIGHT_COEFFICIENT"))
        self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.setValue(self.constants.getConstantDouble("EUCLIDEAN_LOOP_PENALTY"))
    
        self.ui.doubleSpinBoxHelixImportance.setValue(self.constants.getConstantDouble("HELIX_WEIGHT_COEFFICIENT"))
        if (self.constants.getConstantInt("MISSING_HELIX_COUNT") == -1):
            self.ui.checkBoxMissingHelices.setChecked(False)
        else:
            self.ui.checkBoxMissingHelices.setChecked(True)
            self.ui.spinBoxMissingHelixCount.setValue(self.constants.getConstantInt("MISSING_HELIX_COUNT"))
        self.ui.doubleSpinBoxHelixMissingPenalty.setValue(self.constants.getConstantDouble("MISSING_HELIX_PENALTY"))
        self.ui.doubleSpinBoxHelixMissingPenaltyScaled.setValue(self.constants.getConstantDouble("MISSING_HELIX_PENALTY_SCALED"))
        self.ui.doubleSpinBoxEndHelixMissingPenalty.setValue(self.constants.getConstantDouble("START_END_MISSING_HELIX_PENALTY"))
        
        self.ui.checkBoxIncludeSheets.setChecked(True)
        self.ui.doubleSpinBoxSheetImportance.setEnabled(True)
        self.ui.doubleSpinBoxSheetImportance.setValue(self.constants.getConstantDouble("SHEET_WEIGHT_COEFFICIENT"))
        self.ui.checkBoxMissingSheets.setEnabled(True)
        if (self.constants.getConstantInt("MISSING_SHEET_COUNT") == -1):
            self.ui.checkBoxMissingSheets.setChecked(False)
        else:
            self.ui.checkBoxMissingSheets.setChecked(True)
            self.ui.spinBoxMissingSheetCount.setValue(self.constants.getConstantInt("MISSING_SHEET_COUNT"))
        self.ui.doubleSpinBoxSheetMissingPenalty.setEnabled(True)
        self.ui.doubleSpinBoxSheetMissingPenalty.setValue(self.constants.getConstantDouble("MISSING_SHEET_PENALTY"))
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setEnabled(True)
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setValue(self.constants.getConstantDouble("MISSING_SHEET_PENALTY_SCALED"))

    def getConstraints(self):
        print "Reading constraints from c++ layer to python layer"

        corr = self.correspondenceLibrary.correspondenceList[0]
        numH = len(self.correspondenceLibrary.structureObservation.helixDict)

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
                    obsSheet = self.correspondenceEngine.getStrandConstraint(graphIx, 0)
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
                    obsHelixFwd = self.correspondenceEngine.getHelixConstraintFwd(graphIx)
                    obsHelixRev = self.correspondenceEngine.getHelixConstraintRev(graphIx)
                    obsHelixUnk = self.correspondenceEngine.getHelixConstraintUnk(graphIx)
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
        self.correspondenceEngine.clearAllConstraints()

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
            result = self.correspondenceEngine.getResult(i + 1)
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

        # put user-entered match parameters from UI into the correspondence object
        print "setting constants"
        self.setConstants()

        #Loading Predicted SSEs
        print "loading predicted SSEs"

        self.correspondenceEngine.loadSequenceGraph()
        print "before calling StructurePrediction.load"
        print "sequenceFileName is " + str(self.sequenceFileName)
        print "app is " + str(self.parent)
        includeStrands = self.constants.getConstantInt("INCLUDE_STRANDS")
        structPred = StructurePrediction.load(self.sequenceFileName, self.parent, includeStrands)
        print "after calling StructurePrediction.load"

        #Loading Observed SSEs
        print "loading observed SSEs"
        self.correspondenceEngine.loadSkeletonGraph()
        observedHelices = []
        observedSheets = []
        sseCount = self.correspondenceEngine.getSkeletonSSECount()

        print "adding helices to list of observed helices"
        for sseIx in range(sseCount):
            # call to c++ method QueryEngine::getSkeletonSSE(), which returns a c++ GeometricShape object

            cppSse = self.correspondenceEngine.getSkeletonSSE(sseIx)
            # create list of observed helices for this correspondence result
            if cppSse.isHelix():
                q1 = cppSse.getCornerCell3(1)
                q2 = cppSse.getCornerCell3(2)
            
                observedHelices.append(ObservedHelix(sseIx, q1, q2))

        #TODO: Mike this raises an error!;
        print "found " + str(len(observedHelices)) + " helices and " + str(len(observedSheets)) + " sheets"
        structObserv = StructureObservation(helixDict = observedHelices, sheetDict = observedSheets)
        print "writing to correspondenceLibrary"

        # create a new python CorrespondenceLibrary object
        self.correspondenceLibrary = CorrespondenceLibrary(sp = structPred, so = structObserv)

        print "finished creating basic correspondences"
        
    def create_all_helices(self):
        corrLib = self.correspondenceLibrary
        currCorrIndex = corrLib.getCurrentCorrespondenceIndex()
        matchList = corrLib.correspondenceList[currCorrIndex].matchList
        for match in matchList:
            if match.observed is not None:
                print match
                print match.predicted.startIndex
                print match.predicted.stopIndex
                create_helix(self.parent.calpha.renderer, self.parent.structPred.chain, match)

        self.parent.calpha.loaded = True
        self.parent.calpha.modelChanged()
        self.parent.mainCamera.updateGL()
        
    def accept(self):
        print "beginning search"

        # save the settings used to generate the last result, in case this search fails
        if self.executed:
            lastCorrespondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            self.lastCorrespondence = self.correspondenceLibrary.correspondenceList[lastCorrespondenceIndex]
        
        # read user parameters, read skeleton and sequence files, create correspondence library
        self.createBasicCorrespondence()
                
        # execute correspondence query and do cleanup
        self.resultCount = HelixCorrespondence.accept(self)

        print "found " + str(self.resultCount) + " results. cleaning up memory."

        if self.resultCount > 0:
            self.executed = True
            # populate the list of found correspondences
            print "populating result list"
            self.correspondenceLibrary.correspondenceList = self.populateResults(self.correspondenceLibrary)

        else:
            self.executed = False
            print "no results found. loading the most recent successful correspondence"

            self.correspondenceLibrary.correspondenceList = []
            self.correspondenceLibrary.correspondenceList.append(self.lastCorrespondence)

        self.populateComboBox(self.correspondenceLibrary)
        self.sse.modelChanged()
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
        print "currentIndexChanged: ", correspondenceIndex
        self.loadingCorrespondance = True
        self.ui.tableWidgetCorrespondenceList.clearContents()
        if(correspondenceIndex >= 0):
            corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
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
                    for corresp in self.correspondenceLibrary.correspondenceList:
                        if corresp.matchList[i].observed == match.observed:
                            matchPercentage += 1
                    matchPercentage /= len(self.correspondenceLibrary.correspondenceList)
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
                    if match.observed.sseType == 'helix':
                        # color is stored in two places: the renderer and the correspondence engine. update both.
                        self.sse.renderer.setHelixColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        self.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())

                    if match.observed.sseType == 'sheet':
                        self.sse.renderer.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        self.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())

                    notMissing[match.observed.label] = True
            
                checkBox = QtGui.QCheckBox()
                
                self.ui.tableWidgetCorrespondenceList.setCellWidget(sseRow, 2, checkBox)
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

            observedHelices = self.correspondenceLibrary.structureObservation.helixDict
            for i in range(len(observedHelices)):
                if(not notMissing.has_key(i)):
                    self.sse.renderer.setHelixColor(i, 0.5, 0.5, 0.5, 1.0)

        self.correspondenceEngine.setVisibleCorrespondence(correspondenceIndex)
        self.correspondenceLibrary.setCurrentCorrespondenceIndex(correspondenceIndex)
        self.sse.modelChanged()
        self.loadingCorrespondance = False

    def constrainPredictedHelix(self, predicted, observed, constrain):
        def constrainPredictedHelix_po():
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
                for j in range(len(corr.matchList)):
                    match = corr.matchList[j]
                    if(match and match.observed and (match.observed.label == observed)):
                        match.observed = None
                        match.constrained = False
                    if(match and match.predicted and match.predicted.type == 'helix' and match.predicted.serialNo == predicted):
                        newMatch = match
                match = newMatch
                match.constrained = constrain
                match.observed = self.correspondenceLibrary.structureObservation.helixDict[observed]
            self.selectCorrespondence(correspondenceIndex)
        return constrainPredictedHelix_po
    
    def constrainPredictedStrand(self, predicted, observed, constrain):
        def constrainPredictedStrand_po():
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
                match = corr.matchList[predicted]
                match.observed = self.correspondenceLibrary.structureObservation.sheetDict[observed]
                match.constrained = constrain # add or remove constraint
            self.selectCorrespondence(correspondenceIndex)
        return constrainPredictedStrand_po
    
    def sseClicked(self, hit0, hit1, hit2, hit3, hit4, hit5, event):
        if(self.isVisible() and self.dataLoaded and ((hit0 == 0) or (hit0 == 1) or (hit0 == 2)) and (hit1 >= 0)):
            observedType = hit0
            observedSSE = hit1
            constrained = {}
            match = None
            matchKey = 0
            matchKeys = []
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            numH = len(self.correspondenceLibrary.structureObservation.helixDict)
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
                for i in range(len(corr.matchList)):
                    m = corr.matchList[i]
                    if(m.constrained):
                        constrained[m.predicted.serialNo] = True
                    # find the index of the selected helix in the correspondence list
                    if observedType==0 and m.observed and m.observed.sseType == 'helix':
                        if(m.observed.label == observedSSE):
                            match = m
                            matchKey = i
                            matchKeys.append(i)
                    # find the index of the selected sheet in the correspondence list
                    if observedType==2 and m.observed and m.observed.sseType == 'sheet':
                        if(m.observed.label-numH+1 == observedSSE):
                            match = m
                            matchKey = i
                            matchKeys.append(i)

            self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(0, 0, self.ui.tableWidgetCorrespondenceList.rowCount()-1, 2), False)
            if(match):
                self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(0, 0, self.ui.tableWidgetCorrespondenceList.rowCount()-1, 2), False)
                self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(matchKey*2, 0, matchKey*2, 2),True)
                for matchItem in matchKeys:
                    self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(matchItem*2, 0, matchItem*2, 2),True)

            if(self.parent.mainCamera.mouseRightPressed):
                predictedHelices = self.correspondenceLibrary.structurePrediction.helixDict
                predictedStrands = self.correspondenceLibrary.structurePrediction.strandDict
                predictedSSEs = self.correspondenceLibrary.structurePrediction.secelDict
                menu = QtGui.QMenu(self.tr("Constrain observed SSE " + str(observedSSE+1)))
                i_h = 0
                i_s = 0
                for i in range(len(predictedSSEs)):
                    if observedType==0 and predictedSSEs[i].type == 'helix':
                        constrainAction = QtGui.QAction(self.tr("Sequence #" + str(i+1) + ": Predicted helix " + str(predictedHelices[i_h].serialNo)), self)

                        # bold if already selected
                        if corr.matchList[i].observed and corr.matchList[i].observed.label == observedSSE:
                            font=constrainAction.font()
                            font.setBold(True)
                            constrainAction.setFont(font)

                        # checked if already constrained
                        constrainAction.setCheckable(True)
                        if(match and match.observed):
                            constrainAction.setChecked(corr.matchList[i].constrained and corr.matchList[i].observed!=None and corr.matchList[i].observed.label == observedSSE)
                        else:
                            constrainAction.setChecked(False)

                        # checkable if not constrained to another helix
                        constrainAction.setEnabled( (not corr.matchList[i].constrained) or (corr.matchList[i].observed!=None and corr.matchList[i].observed.label == observedSSE) )
                        self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainPredictedHelix(predictedHelices[i_h].serialNo, observedSSE, not constrainAction.isChecked()))
                        menu.addAction(constrainAction)
                        i_h += 1

                    if observedType==2 and predictedSSEs[i].type == 'strand' and self.ui.checkBoxIncludeSheets.isChecked():
                        constrainAction = QtGui.QAction(self.tr("Sequence #" + str(i+1) + ": Predicted strand " + str(predictedStrands[i_s].serialNo)), self)

                        # bold if already selected
                        if corr.matchList[i].observed and corr.matchList[i].observed.label-numH == observedSSE-1:
                            font=constrainAction.font()
                            font.setBold(True)
                            constrainAction.setFont(font)

                        # checked if already constrained
                        constrainAction.setCheckable(True)
                        if(match and match.observed):
                            constrainAction.setChecked(corr.matchList[i].constrained and corr.matchList[i].observed!=None and corr.matchList[i].observed.label-numH == observedSSE-1)
                        else:
                            constrainAction.setChecked(False)

                        # checkable if not constrained to another sheet
                        constrainAction.setEnabled( (not corr.matchList[i].constrained) or (corr.matchList[i].observed!=None and corr.matchList[i].observed.label-numH == observedSSE-1) )
                        self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainPredictedStrand(i, observedSSE-1, not constrainAction.isChecked()))
                        menu.addAction(constrainAction)
                        i_s += 1

                menu.exec_(self.parent.mainCamera.mapToGlobal(self.parent.mainCamera.mouseDownPoint))
                self.parent.mainCamera.updateGL()

    def makeSheetSurfaces(self, offsetx, offsety, offsetz, scalex, scaley, scalez):
        # rebuild the set of sheets to render
        numHelicesSheets = self.correspondenceEngine.getSkeletonSSECount()
        self.sse.renderer.unloadGraphSSEs()
        for i in range(numHelicesSheets):
            if self.correspondenceEngine.getSkeletonSSE(i).isSheet():
                self.renderer.loadGraphSSE(i, self.correspondenceEngine.getSkeletonSSE(i), offsetx, offsety, offsetz, scalex, scaley, scalez)

    def extraDrawingRoutines(self):
        try:
            print termcolor.colored('correspondenceEngine.draw', 'yellow')
            self.correspondenceEngine.draw(0)
            self.correspondenceEngine.drawAllPaths(0,
                                                   self.ui.checkBoxShowAllPaths.isChecked(),
                                                   self.ui.checkBoxShowHelixCorners.isChecked(),
                                                   self.ui.checkBoxShowSheetCorners.isChecked(),
                                                   False)
        except:
            print "Problem in sseViewer::drawGL: correspondenceEngine.draw"
