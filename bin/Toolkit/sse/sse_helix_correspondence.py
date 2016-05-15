from PyQt4 import QtCore, QtGui
from libpytoolkit import SSEEngine, SSEResult, Vec3F
from sse_defaults import SSEDefaults
from .correspondence.CorrespondenceLibrary import CorrespondenceLibrary
from .correspondence.Correspondence import Correspondence
from .correspondence.Match import Match
from .correspondence.ObservedHelix import ObservedHelix
# from .correspondence.ObservedSheet import ObservedSheet
from .correspondence.StructureObservation import StructureObservation
from .correspondence.StructurePrediction import StructurePrediction
# from .seq_model.Helix import Helix
# from vector_lib import *

import math


class SSEHelixCorrespondence(object):

    def __init__(self, skeleton, sequence, helix, output, auto=True):
        self.ui = SSEDefaults()
        self.skeleton = skeleton
        self.sequence = sequence
        self.helix    = helix
        self.output   = output
        
        self.executed = False
        self.loadingCorrespondance = False
        self.userConstraints = {}
        self.constraintActions = {}
        self.selectedRow = 0
        self.dataLoaded = False
        
        '''
        SSEViewer
        '''
        self.correspondenceEngine = SSEEngine()
        self.correspondenceLibrary = CorrespondenceLibrary()
        
        if auto:
            self.accept()
            self.correspondenceEngine.saveCorrespondenceToFile(self.output)
        
    def setConstants(self):
        #Data Sources tab
        #self.correspondenceEngine.setConstant("SSE_FILE_NAME", str(self.ui.lineEditHelixLengthFile.text()))
        self.correspondenceEngine.setConstant("VRML_HELIX_FILE_NAME", self.helix)
#         self.correspondenceEngine.setConstant("VRML_SHEET_FILE_NAME", str(self.ui.lineEditSheetLocationFile.text()))
        self.correspondenceEngine.setConstant("MRC_FILE_NAME", self.skeleton)
        self.sequenceFileName = self.sequence
        self.correspondenceEngine.setConstant("SEQUENCE_FILE_NAME", self.sequenceFileName)
        if self.sequenceFileName.split('.')[-1].lower() == 'pdb':
            self.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "PDB")
        elif self.sequenceFileName.split('.')[-1].lower() == 'seq':
            self.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "SEQ")
        
        #Graph Settings tab
        self.correspondenceEngine.setConstantInt("BORDER_MARGIN_THRESHOLD", self.ui.BorderMarginThreshold)
        self.correspondenceEngine.setConstant("EUCLIDEAN_DISTANCE_THRESHOLD", self.ui.EuclideanDistance)
        self.correspondenceEngine.setConstant("MAXIMUM_DISTANCE_SHEET_SKELETON", self.ui.MaxSheetDistance)
        self.correspondenceEngine.setConstantInt("MINIMUM_SHEET_SIZE", self.ui.MinSheetSize)
        self.correspondenceEngine.setConstant("SHEET_SELF_LOOP_LENGTH", self.ui.SheetSelfLoopLength)
        self.correspondenceEngine.setConstant("SHEET_MERGE_THRESHOLD", self.ui.SheetMergeThreshold)
#         if (self.ui.IncludeStrands.isChecked()):
#             self.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 1)
#         else:
#             self.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 0)
        self.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 0)

        #Matching Settings tab
        self.correspondenceEngine.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.ui.EuclideanToPDBRatio)
        if(self.ui.AbsoluteDifference):
            self.correspondenceEngine.setConstantInt("COST_FUNCTION", 1)
        elif (self.ui.NormalizedDifference.isChecked()):
            self.correspondenceEngine.setConstantInt("COST_FUNCTION", 2)
        else:
            self.correspondenceEngine.setConstantInt("COST_FUNCTION", 3)

        self.correspondenceEngine.setConstant("LOOP_WEIGHT_COEFFICIENT", self.ui.LoopImportance)
        self.correspondenceEngine.setConstant("EUCLIDEAN_LOOP_PENALTY", self.ui.EuclideanLoopUsedPenalty)

        self.correspondenceEngine.setConstant("HELIX_WEIGHT_COEFFICIENT", self.ui.HelixImportance)
        if(self.ui.MissingHelices):
            self.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", self.ui.spinBoxMissingHelixCount)
        else:
            self.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", -1)
        self.correspondenceEngine.setConstant("MISSING_HELIX_PENALTY", self.ui.HelixMissingPenalty)
        self.correspondenceEngine.setConstant("MISSING_HELIX_PENALTY_SCALED", self.ui.HelixMissingPenaltyScaled)
        self.correspondenceEngine.setConstant("START_END_MISSING_HELIX_PENALTY", self.ui.EndHelixMissingPenalty)
        
        self.correspondenceEngine.setConstant("SHEET_WEIGHT_COEFFICIENT", self.ui.SheetImportance)
        if(self.ui.MissingSheets):
            self.correspondenceEngine.setConstantInt("MISSING_SHEET_COUNT", self.ui.spinBoxMissingSheetCount)
        else:
            self.correspondenceEngine.setConstantInt("MISSING_SHEET_COUNT", -1)
        self.correspondenceEngine.setConstant("MISSING_SHEET_PENALTY", self.ui.SheetMissingPenalty)
        self.correspondenceEngine.setConstant("MISSING_SHEET_PENALTY_SCALED", self.ui.SheetMissingPenaltyScaled)
        
        # no longer needed?
        self.correspondenceEngine.setConstantBool("NORMALIZE_GRAPHS", True)

        #Tab 4 User Constraints
        # comment out the constraint clearing so that constraints can be loaded from settings files
        #self.correspondenceEngine.clearAllConstraints()
        correspondenceIndex = self.ui.correspondences.currentIndex()
        if(correspondenceIndex >= 0):
            corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
            predictedGraphNode = 1
            nObservedHelices = len(self.correspondenceLibrary.structureObservation.helixDict)
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]
                self.userConstraints[i] = match.constrained
                if match.predicted.type == 'helix':
                    if match.constrained:
                        if(match.observed):
                            self.correspondenceEngine.setHelixConstraint(predictedGraphNode, 2*match.observed.label + 1)
                        else:
                            self.correspondenceEngine.setHelixConstraint(predictedGraphNode, -1)
                if match.predicted.type == 'strand':
                    if(not self.ui.IncludeSheets.isChecked()):
                        self.userConstraints[i]=False # clear all strand constraints
                        match.constrained = False     # clear all strand constraints
                        self.correspondenceEngine.setNodeConstraint(predictedGraphNode, -1)
                    elif(match.constrained):
                        if(match.observed):
                            self.correspondenceEngine.setNodeConstraint(predictedGraphNode, match.observed.label + nObservedHelices + 1)
                        else:
                            self.correspondenceEngine.setNodeConstraint(predictedGraphNode, -1)
                if (match.predicted.type) == 'strand':
                    predictedGraphNode += 1
                if (match.predicted.type) == 'helix':
                    predictedGraphNode += 2

    def populateResults(self, library):

        print "Starting to populate results. Found " + str(self.resultCount) + " results."

        # clear the correspondence list
        corrList = []
        
        # iterate over all results from correspondence algorithm
        #print "Iterating over results."
        for i in range(self.resultCount):
            # create a Correspondence object and add it to the list
            # start from correspondenceEngine result
            result = self.correspondenceEngine.getResult(i+1)
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
        self.ui.correspondences.clear()
        # add all correspondence to pulldown menu
        for i in range(len(library.correspondenceList)):
            corr = library.correspondenceList[i]
            self.ui.correspondences.addItem("Correspondence " + str(i+1) + " - [Cost: " + str(corr.score) + "]")
                   
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
        includeStrands = self.correspondenceEngine.getConstantInt("INCLUDE_STRANDS")
        self.structPred = StructurePrediction.load(self.sequenceFileName, None, includeStrands)
        print "after calling StructurePrediction.load"
        cAlphaViewer   = None
        sseViewer      = None
        skeletonViewer = None

        def vector3DFloatToTuple(v3df):
            return (v3df.x(), v3df.y(), v3df.z())
        
        #Loading Observed SSEs
        print "loading observed SSEs"
        self.correspondenceEngine.loadSkeletonGraph()
        observedHelices = {}
        helixCount = 0
        observedSheets = {}
        sheetCount = 0
        sseCount = self.correspondenceEngine.getSkeletonSSECount()

        print "adding helices to list of observed helices"
        for sseIx in range(sseCount):
            # call to c++ method QueryEngine::getSkeletonSSE(), which returns a c++ GeometricShape object
            cppSse = self.correspondenceEngine.getSkeletonSSE(sseIx)
            
            # create list of observed helices for this correspondence result
            if cppSse.isHelix():
                q1 = None
                q2 = None
            
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
        self.correspondenceLibrary = CorrespondenceLibrary(sp = self.structPred, so = structObserv)
               
        print "finished creating basic correspondences"
        
    def accept(self):
        print "beginning search"

        # save the settings used to generate the last result, in case this search fails
        if self.executed:
            lastCorrespondenceIndex = self.ui.correspondences.currentIndex()
            self.lastCorrespondence = self.correspondenceLibrary.correspondenceList[lastCorrespondenceIndex]
        
        # read user parameters, read skeleton and sequence files, create correspondence library
        self.createBasicCorrespondence()
                
        # execute correspondence query and do cleanup
        memErr = False
        try:
            self.resultCount = self.correspondenceEngine.executeQuery()
        except MemoryError:
            print "memory error"
            self.resultCount=0
            memErr = True
        self.correspondenceEngine.cleanupMemory()
        self.correspondenceEngine.clearAllConstraints()

        print "found " + str(self.resultCount) + " results. cleaning up memory."

        if self.resultCount > 0:
            self.executed = True
            # populate the list of found correspondences
            print "populating result list"
            self.correspondenceLibrary.correspondenceList = self.populateResults(self.correspondenceLibrary)

        else:
            self.executed = False
            if memErr:
                QtGui.QMessageBox.warning(self.app, "Insufficient Memory", "Insufficient memory to complete the search. Try adding or removing constraints.")
            else:
                QtGui.QMessageBox.warning(self.app, "No results found", "The correspondence search returned no results. Try removing constraints.")
            print "no results found. loading the most recent successful correspondence"

            self.correspondenceLibrary.correspondenceList = []
            self.correspondenceLibrary.correspondenceList.append(self.lastCorrespondence)

        self.populateComboBox(self.correspondenceLibrary)
        print "done with search"
