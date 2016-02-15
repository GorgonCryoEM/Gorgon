from libpytoolkit import SSEEngine, SSEResult, Vector3DFloat
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

    def __init__(self, skeleton, sequence, helix, output):
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
        
        self.run()
        
        self.correspondenceEngine.saveCorrespondenceToFile(self.output)
        
    def run(self):
        self.checkOk()
        self.accept()
        
    def checkOk(self):
        """
        This checks if all files necessary for the correspondence search have been loaded. If so, the
        correspondence search parameter tabs are enabled and a basic correspondence is created.   
        """
        print "begin checkOk"
        print "correspondence index at beginning is "
        print self.ui.correspondences.currentIndex()
        
        if(self.dataLoaded):
            self.executed = False
            self.createBasicCorrespondence()
            print "after creating basic correspondence (1), secelDict has length " + str(len(self.correspondenceLibrary.structurePrediction.secelDict))
            #self.createBasicCorrespondence()
            print "after creating basic correspondence (2), secelDict has length " + str(len(self.correspondenceLibrary.structurePrediction.secelDict))
            self.correspondenceLibrary.correspondenceList = self.populateEmptyResults(self.correspondenceLibrary)
            print "correspondenceList has length " + str(len(self.correspondenceLibrary.correspondenceList))
            self.populateComboBox(self.correspondenceLibrary)
            self.viewer.makeSheetSurfaces(self.app.viewers['skeleton'].renderer.getOriginX(), self.app.viewers['skeleton'].renderer.getOriginY(), self.app.viewers['skeleton'].renderer.getOriginZ(), self.app.viewers['skeleton'].renderer.getSpacingX(), self.app.viewers['skeleton'].renderer.getSpacingY(), self.app.viewers['skeleton'].renderer.getSpacingZ())
            if(allLoaded):
                self.ui.tabWidget.setCurrentIndex(1)
        else:
            print "data not loaded"
        print "correspondence index at end is " + str(self.ui.correspondences.currentIndex())
        print "end checkOk"
    
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

    def getConstants(self):
        
        #Graph Settings tab
        self.ui.BorderMarginThreshold.setValue(self.correspondenceEngine.getConstantInt("BORDER_MARGIN_THRESHOLD"))
        self.ui.EuclideanDistance.setValue(self.correspondenceEngine.getConstantDouble("EUCLIDEAN_DISTANCE_THRESHOLD"))
        self.ui.MinSheetSize.setValue(self.correspondenceEngine.getConstantInt("MINIMUM_SHEET_SIZE"))
        self.ui.MaxSheetDistance.setValue(self.correspondenceEngine.getConstantDouble("MAXIMUM_DISTANCE_SHEET_SKELETON"))
        self.ui.SheetSelfLoopLength.setValue(self.correspondenceEngine.getConstantDouble("SHEET_SELF_LOOP_LENGTH"))
        self.ui.SheetMergeThreshold.setValue(self.correspondenceEngine.getConstantDouble("SHEET_MERGE_THRESHOLD"))
        if(self.correspondenceEngine.getConstantInt("INCLUDE_STRANDS") == 1):
            self.ui.IncludeStrands.setChecked(True)
        else:
            self.ui.IncludeStrands.setChecked(False)

        #Matching settings tab
        self.ui.EuclideanToPDBRatio.setValue(self.correspondenceEngine.getConstantDouble("EUCLIDEAN_VOXEL_TO_PDB_RATIO"))
        if(self.correspondenceEngine.getConstantInt("COST_FUNCTION") == 1):
            self.ui.AbsoluteDifference.setChecked(True)
            self.ui.NormalizedDifference.setChecked(False)
            self.ui.QuadraticError.setChecked(False)
        elif (self.correspondenceEngine.getConstantInt("COST_FUNCTION") == 2):
            self.ui.AbsoluteDifference.setChecked(False)
            self.ui.NormalizedDifference.setChecked(True)
            self.ui.QuadraticError.setChecked(False)
        elif (self.correspondenceEngine.getConstantInt("COST_FUNCTION") == 3):
            self.ui.AbsoluteDifference.setChecked(False)
            self.ui.NormalizedDifference.setChecked(False)
            self.ui.QuadraticError.setChecked(True)

        self.ui.LoopImportance.setValue(self.correspondenceEngine.getConstantDouble("LOOP_WEIGHT_COEFFICIENT"))
        self.ui.EuclideanLoopUsedPenalty.setValue(self.correspondenceEngine.getConstantDouble("EUCLIDEAN_LOOP_PENALTY"))

        self.ui.HelixImportance.setValue(self.correspondenceEngine.getConstantDouble("HELIX_WEIGHT_COEFFICIENT"))
        if (self.correspondenceEngine.getConstantInt("MISSING_HELIX_COUNT") == -1):
            self.ui.MissingHelices = False
        else:
            self.ui.MissingHelices = True
            self.ui.spinBoxMissingHelixCount.setValue(self.correspondenceEngine.getConstantInt("MISSING_HELIX_COUNT"))
        self.ui.HelixMissingPenalty.setValue(self.correspondenceEngine.getConstantDouble("MISSING_HELIX_PENALTY"))
        self.ui.HelixMissingPenaltyScaled.setValue(self.correspondenceEngine.getConstantDouble("MISSING_HELIX_PENALTY_SCALED"))
        self.ui.EndHelixMissingPenalty.setValue(self.correspondenceEngine.getConstantDouble("START_END_MISSING_HELIX_PENALTY"))
        
        self.ui.IncludeSheets.setChecked(True)
        self.ui.SheetImportance.setEnabled(True)
        self.ui.SheetImportance.setValue(self.correspondenceEngine.getConstantDouble("SHEET_WEIGHT_COEFFICIENT"))
        self.ui.MissingSheets.setEnabled(True)
        if (self.correspondenceEngine.getConstantInt("MISSING_SHEET_COUNT") == -1):
            self.ui.MissingSheets.setChecked(False)
        else:
            self.ui.MissingSheets.setChecked(True)
            self.ui.spinBoxMissingSheetCount.setValue(self.correspondenceEngine.getConstantInt("MISSING_SHEET_COUNT"))
        self.ui.SheetMissingPenalty.setEnabled(True)
        self.ui.SheetMissingPenalty.setValue(self.correspondenceEngine.getConstantDouble("MISSING_SHEET_PENALTY"))
        self.ui.SheetMissingPenaltyScaled.setEnabled(True)
        self.ui.SheetMissingPenaltyScaled.setValue(self.correspondenceEngine.getConstantDouble("MISSING_SHEET_PENALTY_SCALED"))

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
                    obsSheet = self.correspondenceEngine.getStrandConstraint(graphIx,0)
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

    def selectCorrespondence(self, correspondenceIndex):
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
                        self.viewer.renderer.setHelixColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        self.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())

                    if match.observed.sseType == 'sheet':
                        self.viewer.renderer.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        self.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())

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
            
            observedHelices = self.correspondenceLibrary.structureObservation.helixDict
            for i in range(len(observedHelices)):
                if(not notMissing.has_key(i)):
                    self.viewer.renderer.setHelixColor(i, 0.5, 0.5, 0.5, 1.0)
                
        self.correspondenceEngine.setVisibleCorrespondence(correspondenceIndex)
        self.correspondenceLibrary.setCurrentCorrespondenceIndex(correspondenceIndex)
        self.viewer.emitModelChanged()
        self.loadingCorrespondance = False
        
    def rebuildGraph(self):
        print "correspondence index before rebuilding is "
        print self.ui.correspondences.currentIndex()
        self.ui.correspondences.setCurrentIndex(-1)
        print "correspondence index after setting to -1 is "
        print self.ui.correspondences.currentIndex()
        self.setConstants()
        self.checkOk()
        self.viewer.makeSheetSurfaces(self.app.viewers['skeleton'].renderer.getOriginX(), self.app.viewers['skeleton'].renderer.getOriginY(), self.app.viewers['skeleton'].renderer.getOriginZ(), self.app.viewers['skeleton'].renderer.getSpacingX(), self.app.viewers['skeleton'].renderer.getSpacingY(), self.app.viewers['skeleton'].renderer.getSpacingZ())
        self.viewer.emitModelChanged()
        print "correspondence index after rebuilding is "
        print self.ui.correspondences.currentIndex()
        
    def constraintAdded(self, state):
        if(not self.loadingCorrespondance):
            correspondenceIndex = self.ui.correspondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
                for i in range(len(corr.matchList)):
                    match = corr.matchList[i]
                    match.constrained = (self.ui.tableWidgetCorrespondenceList.cellWidget(2*i, 2).checkState() == QtCore.Qt.Checked)
                    
    def createActionsForCell(self, row, col):
        self.selectedRow = row/2
        for act in self.ui.tableWidgetCorrespondenceList.actions()[:]:
            self.ui.tableWidgetCorrespondenceList.removeAction(act)
        if(col == 1 and row%2==0):
            observedHelices = self.correspondenceLibrary.structureObservation.helixDict
            observedSheets = self.correspondenceLibrary.structureObservation.sheetDict
            constrained = {}
            
            correspondenceIndex = self.ui.correspondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
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
            if match.predicted.type == 'strand' and self.ui.IncludeSheets.isChecked():
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

    def constrainSSE(self, pred, obs, dir):
        correspondenceIndex = self.ui.correspondences.currentIndex()
        if(correspondenceIndex >= 0):
            corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
            match = corr.matchList[pred]
            match.constrained = True
            if(obs == -1):
                match.observed = None
            else:
                if match.predicted.type == 'helix':
                    match.observed = self.correspondenceLibrary.structureObservation.helixDict[obs-1]
                    if (dir == 1):
                        match.directionConstrained = True
                        match.direction = Match.FORWARD
                    elif (dir == -1):
                        match.directionConstrained = True
                        match.direction = Match.REVERSE
                    else:
                        match.directionConstrained = False
                            
                if match.predicted.type == 'strand':
                    match.observed = self.correspondenceLibrary.structureObservation.sheetDict[obs-1]
                
        self.selectCorrespondence(correspondenceIndex)

    def constrainObservedHelix(self, i):
        def constrainObservedHelix_i():
            correspondenceIndex = self.ui.correspondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]

                # clear old constraints involving helix i
                for j in range(len(corr.matchList)):
                    match = corr.matchList[j]
                    if(match and match.observed and (match.observed.label == i)):
                        match.observed = None
                
                match = corr.matchList[self.selectedRow]
                match.constrained = True
                if(i == -1):
                    match.observed = None
                else:
                    match.observed = self.correspondenceLibrary.structureObservation.helixDict[i]
            self.selectCorrespondence(correspondenceIndex)
        return constrainObservedHelix_i

    def constrainObservedSheet(self, i):
        def constrainObservedSheet_i():
            numH = len(self.correspondenceLibrary.structureObservation.helixDict)
            correspondenceIndex = self.ui.correspondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
                match = corr.matchList[self.selectedRow]
                match.constrained = True
                if(i == -1):
                    match.observed = None
                else:
                    match.observed = self.correspondenceLibrary.structureObservation.sheetDict[i]
            self.selectCorrespondence(correspondenceIndex)
        return constrainObservedSheet_i

    def constrainPredictedHelix(self, predicted, observed, constrain):
        def constrainPredictedHelix_po():
            correspondenceIndex = self.ui.correspondences.currentIndex()
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
            correspondenceIndex = self.ui.correspondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.correspondenceLibrary.correspondenceList[correspondenceIndex]
                match = corr.matchList[predicted]
                match.observed = self.correspondenceLibrary.structureObservation.sheetDict[observed]
                match.constrained = constrain # add or remove constraint
            self.selectCorrespondence(correspondenceIndex)
        return constrainPredictedStrand_po
    
    def exportToRosettaFile(self, fileName):
        
        library = self.viewer.correspondenceLibrary
        engine = self.viewer.correspondenceEngine
        skeletonViewer = self.app.viewers["skeleton"]
        calphaViewer = self.app.viewers["calpha"]

        def tupleToVector3DFloat(pt):
            return Vector3DFloat(pt[0], pt[1], pt[2])

        def vector3DFloatToTuple(pt):
            return [pt.x(), pt.y(), pt.z()]

        def cAlphaToSkeleton(pt):
            return skeletonViewer.worldToObjectCoordinates(calphaViewer.objectToWorldCoordinates(pt))
        
        def skeletonToCAlpha(pt):
            return calphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates(pt))
            
        doc = xml.dom.minidom.Document()
        gorgonModelListElement = doc.createElement("GORGON_MODEL_LIST")
        
        sequenceElement = doc.createElement("SEQUENCE")
        sequenceElement.setAttribute("FIRST_RESIDUE_NO", str(library.structurePrediction.chain.getFirstResidueIndex()))
        residueList = doc.createTextNode(library.structurePrediction.chain.getSequence())
        sequenceElement.appendChild(residueList)
        gorgonModelListElement.appendChild(sequenceElement)
        
        correspondenceListElement = doc.createElement("CORRESPONDENCE_LIST")
        
        for i in range(len(library.correspondenceList)):
            corr = library.correspondenceList[i]
            
            correspondenceElement = doc.createElement("CORRESPONDENCE")
            correspondenceElement.setAttribute("RANK", str(i+1))
            correspondenceElement.setIdAttribute("RANK")
            correspondenceElement.setAttribute("GORGON_SCORE", str(corr.score))

            engine.initializePathFinder(self.app.viewers["skeleton"].renderer.getMesh())
            
            helixRadius = vectorDistance(cAlphaToSkeleton([2.5, 0,0]), cAlphaToSkeleton([0,0,0])) # Finding the size of 2.5 Angstroms in skeleton space
            
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]
                if(match.observed):
                    if(match.observed.sseType == 'helix'):
                        sseElement = doc.createElement("SECONDARY_STRUCTURE")
                        
                        startTag = "START_COORDINATE"
                        endTag = "END_COORDINATE"
                        if(match.direction != Match.FORWARD):
                            startTag, endTag = endTag, startTag
                        
                        startCoordinateElement = doc.createElement(startTag)
                        startCoordinateElement.setAttribute("X", str(match.observed.beginningCoord[0]))
                        startCoordinateElement.setAttribute("Y", str(match.observed.beginningCoord[1]))
                        startCoordinateElement.setAttribute("Z", str(match.observed.beginningCoord[2]))
                        sseElement.appendChild(startCoordinateElement)
                        
                        endCoordinateElement = doc.createElement(endTag)
                        endCoordinateElement.setAttribute("X", str(match.observed.endCoord[0]))
                        endCoordinateElement.setAttribute("Y", str(match.observed.endCoord[1]))
                        endCoordinateElement.setAttribute("Z", str(match.observed.endCoord[2]))
                        sseElement.appendChild(endCoordinateElement)
                        
                        sseElement.setAttribute("SSE_TYPE", "ALPHA_HELIX")
                        sseElement.setAttribute("START_RESIDUE", str(match.predicted.startIndex))
                        sseElement.setAttribute("END_RESIDUE", str(match.predicted.stopIndex))
                                                                                                    
                        correspondenceElement.appendChild(sseElement)
                        if(match.direction == Match.FORWARD):
                            engine.initializePathHelix(i, tupleToVector3DFloat(cAlphaToSkeleton(match.observed.endCoord)), tupleToVector3DFloat(cAlphaToSkeleton(match.observed.beginningCoord)), helixRadius)
                        else:
                            engine.initializePathHelix(i, tupleToVector3DFloat(cAlphaToSkeleton(match.observed.beginningCoord)), tupleToVector3DFloat(cAlphaToSkeleton(match.observed.endCoord)), helixRadius)
                                
                    elif (match.observed.sseType == 'sheet'):
                        pass;
         
            for i in range(1, len(corr.matchList)):
                if ((corr.matchList[i-1].observed) and
                    (corr.matchList[i].observed) and
                    (corr.matchList[i-1].predicted.stopIndex+1 <= corr.matchList[i].predicted.startIndex-1)):
                    sseElement = doc.createElement("SECONDARY_STRUCTURE")
                    sseElement.setAttribute("SSE_TYPE", "LOOP")
                    sseElement.setAttribute("START_RESIDUE", str(corr.matchList[i-1].predicted.stopIndex+1))
                    sseElement.setAttribute("END_RESIDUE", str(corr.matchList[i].predicted.startIndex-1))
                    
                    engine.getPathSpace(i-1, False, i, True)
                    vertexListElement = doc.createElement("VERTEX_LIST")
                    
                    for j in range(engine.getPathVertexCount()):
                        vertex = skeletonToCAlpha(vector3DFloatToTuple(engine.getPathVertex(j)))
                        vertexElement = doc.createElement("VERTEX")
                        vertexElement.setAttribute("ID", str(j))
                        vertexElement.setIdAttribute("ID")
                        vertexElement.setAttribute("X", str(vertex[0]))
                        vertexElement.setAttribute("Y", str(vertex[1]))
                        vertexElement.setAttribute("Z", str(vertex[2]))
                        vertexListElement.appendChild(vertexElement)
                                        
                    sseElement.appendChild(vertexListElement)
                    
                    edgeListElement = doc.createElement("EDGE_LIST")
                    for j in range(engine.getPathEdgeCount()):
                        ix0 = engine.getEdgeVertexIndex(j, 0)
                        ix1 = engine.getEdgeVertexIndex(j, 1)
                        edgeElement = doc.createElement("EDGE")
                        edgeElement.setAttribute("VERTEX_1_INDEX", str(ix0))
                        edgeElement.setAttribute("VERTEX_2_INDEX", str(ix1))
                        edgeListElement.appendChild(edgeElement)
                    
                    engine.clearPathSpace()
                    sseElement.appendChild(edgeListElement)
                    
                    correspondenceElement.appendChild(sseElement)
         
            engine.clearPathFinder()
           
            correspondenceListElement.appendChild(correspondenceElement)
        
        gorgonModelListElement.appendChild(correspondenceListElement)
        doc.appendChild(gorgonModelListElement)
                
        outFile = open(fileName, "w")
        outFile.writelines(doc.toprettyxml())
        outFile.close()
