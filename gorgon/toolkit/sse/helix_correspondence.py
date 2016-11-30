from ..libpytoolkit import SSEEngine, IBackEnd


class HelixCorrespondence(object):

    def __init__(self, args):
        self.skeleton = args.skeleton
        self.sequence = args.sequence
        self.helix    = args.helix
        if hasattr(args, 'output') and args.output:
            self.output   = args.output
                
        self.correspondenceEngine = SSEEngine()
        self.constants = IBackEnd()
        self.loadDefaultParams()
        
    def setConstants(self):
        #Data Sources tab
        #self.constants.setConstant("SSE_FILE_NAME", str(self.ui.lineEditHelixLengthFile.text()))
        self.constants.setConstantString("VRML_HELIX_FILE_NAME", self.helix)
#         self.constants.setConstant("VRML_SHEET_FILE_NAME", str(self.ui.lineEditSheetLocationFile.text()))
        self.constants.setConstantString("MRC_FILE_NAME", self.skeleton)
        self.sequenceFileName = self.sequence
        self.constants.setConstantString("SEQUENCE_FILE_NAME", self.sequenceFileName)
        if self.sequenceFileName.split('.')[-1].lower() == 'pdb':
            self.constants.setConstantString("SEQUENCE_FILE_TYPE", "PDB")
        elif self.sequenceFileName.split('.')[-1].lower() == 'seq':
            self.constants.setConstantString("SEQUENCE_FILE_TYPE", "SEQ")
        
        #Graph Settings tab
        self.constants.setConstantInt("BORDER_MARGIN_THRESHOLD", self.BorderMarginThreshold)
        self.constants.setConstantDouble("EUCLIDEAN_DISTANCE_THRESHOLD", self.EuclideanDistance)
        self.constants.setConstantDouble("MAXIMUM_DISTANCE_SHEET_SKELETON", self.MaxSheetDistance)
        self.constants.setConstantInt("MINIMUM_SHEET_SIZE", self.MinSheetSize)
        self.constants.setConstantDouble("SHEET_SELF_LOOP_LENGTH", self.SheetSelfLoopLength)
        self.constants.setConstantDouble("SHEET_MERGE_THRESHOLD", self.SheetMergeThreshold)
        self.constants.setConstantBool("INCLUDE_STRANDS", 0)

        #Matching Settings tab
        self.constants.setConstantDouble("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.EuclideanToPDBRatio)
        if(self.AbsoluteDifference):
            self.constants.setConstantInt("COST_FUNCTION", 1)
        elif (self.NormalizedDifference.isChecked()):
            self.constants.setConstantInt("COST_FUNCTION", 2)
        else:
            self.constants.setConstantInt("COST_FUNCTION", 3)

        self.constants.setConstantDouble("LOOP_WEIGHT_COEFFICIENT", self.LoopImportance)
        self.constants.setConstantDouble("EUCLIDEAN_LOOP_PENALTY", self.EuclideanLoopUsedPenalty)

        self.constants.setConstantDouble("HELIX_WEIGHT_COEFFICIENT", self.HelixImportance)
        self.constants.setConstantInt("MISSING_HELIX_COUNT", -1)
        
        self.constants.setConstantDouble("MISSING_HELIX_PENALTY", self.HelixMissingPenalty)
        self.constants.setConstantDouble("MISSING_HELIX_PENALTY_SCALED", self.HelixMissingPenaltyScaled)
        self.constants.setConstantDouble("START_END_MISSING_HELIX_PENALTY", self.EndHelixMissingPenalty)

        self.constants.setConstantDouble("SHEET_WEIGHT_COEFFICIENT", self.SheetImportance)
        self.constants.setConstantInt("MISSING_SHEET_COUNT", -1)
        
        self.constants.setConstantDouble("MISSING_SHEET_PENALTY", self.SheetMissingPenalty)
        self.constants.setConstantDouble("MISSING_SHEET_PENALTY_SCALED", self.SheetMissingPenaltyScaled)

        # no longer needed?
        self.constants.setConstantBool("NORMALIZE_GRAPHS", True)

    def loadDefaultParams(self):
        # Graph Settings tab
        self.BorderMarginThreshold = 5
        self.EuclideanDistance = 0.0
        self.IncludeSheets = True
    
        self.ShowHelixCorners = False
        self.ShowSheetCorners = False
        self.ShowAllPaths = False
    
        # Matching Settings tab
        self.EuclideanToPDBRatio = 10.0
        self.AbsoluteDifference = True
        self.NormalizedDifference = False
        self.QuadraticError = False
    
        self.LoopImportance = 0.2
        self.EuclideanLoopUsedPenalty = 5.0
    
        self.HelixImportance = 1.0
        self.MissingHelices = False
        self.MissingHelixCount = 0
        self.HelixMissingPenalty = 5.0
        self.HelixMissingPenaltyScaled = 0.0
        self.EndHelixMissingPenalty = 5.0
    
        self.SheetImportance = 1.0
        self.MissingSheets = False
        self.MissingSheetCount = 0
        self.SheetMissingPenalty = 5.0
        self.SheetMissingPenaltyScaled = 0.0
        # 
        # # Results tab
        # self.ui.tableWidgetCorrespondenceList.clearContents()
        # self.ui.tabWidget.setCurrentIndex(0)
        # self.ui.comboBoxCorrespondences.setCurrentIndex(-1)
        
        # Sheet related
        self.MinSheetSize        = 10
        self.MaxSheetDistance    = 5.0
        self.SheetSelfLoopLength = 5.0
        self.SheetMergeThreshold = 3.0

    def accept(self):
        self.setConstants()
        self.correspondenceEngine.loadSequenceGraph()
        self.correspondenceEngine.loadSkeletonGraph()
        return self.correspondenceEngine.executeQuery()
