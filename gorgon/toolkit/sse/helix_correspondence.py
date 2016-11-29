from ..libpytoolkit import SSEEngine, IBackEnd
from .defaults import SSEDefaults


class HelixCorrespondence(object):

    def __init__(self, args):
        self.defaults = SSEDefaults()
        
        self.skeleton = args.skeleton
        self.sequence = args.sequence
        self.helix    = args.helix
        if hasattr(args, 'output') and args.output:
            self.output   = args.output
                
        '''
        SSEViewer
        '''
        self.correspondenceEngine = SSEEngine()
        self.constants = IBackEnd()
        
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
        self.constants.setConstantInt("BORDER_MARGIN_THRESHOLD", self.defaults.BorderMarginThreshold)
        self.constants.setConstantDouble("EUCLIDEAN_DISTANCE_THRESHOLD", self.defaults.EuclideanDistance)
        self.constants.setConstantDouble("MAXIMUM_DISTANCE_SHEET_SKELETON", self.defaults.MaxSheetDistance)
        self.constants.setConstantInt("MINIMUM_SHEET_SIZE", self.defaults.MinSheetSize)
        self.constants.setConstantDouble("SHEET_SELF_LOOP_LENGTH", self.defaults.SheetSelfLoopLength)
        self.constants.setConstantDouble("SHEET_MERGE_THRESHOLD", self.defaults.SheetMergeThreshold)
        # if (self.ui.checkBoxIncludeStrands.isChecked()):
        #     self.constants.setConstant("INCLUDE_STRANDS", 1)
        # else:
        #     self.constants.setConstant("INCLUDE_STRANDS", 0)

        #Matching Settings tab
        self.constants.setConstantDouble("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.defaults.EuclideanToPDBRatio)
        if(self.defaults.AbsoluteDifference):
            self.constants.setConstantInt("COST_FUNCTION", 1)
        elif (self.defaults.NormalizedDifference.isChecked()):
            self.constants.setConstantInt("COST_FUNCTION", 2)
        else:
            self.constants.setConstantInt("COST_FUNCTION", 3)

        self.constants.setConstantDouble("LOOP_WEIGHT_COEFFICIENT", self.defaults.LoopImportance)
        self.constants.setConstantDouble("EUCLIDEAN_LOOP_PENALTY", self.defaults.EuclideanLoopUsedPenalty)

        self.constants.setConstantDouble("HELIX_WEIGHT_COEFFICIENT", self.defaults.HelixImportance)
        # if(self.ui.checkBoxMissingHelices.isChecked()):
        #     self.constants.setConstant("MISSING_HELIX_COUNT", self.defaults.MissingHelixCount)
        # else:
        #     self.constants.setConstant("MISSING_HELIX_COUNT", -1)
        self.constants.setConstantInt("MISSING_HELIX_COUNT", -1)
        
        self.constants.setConstantDouble("MISSING_HELIX_PENALTY", self.defaults.HelixMissingPenalty)
        self.constants.setConstantDouble("MISSING_HELIX_PENALTY_SCALED", self.defaults.HelixMissingPenaltyScaled)
        self.constants.setConstantDouble("START_END_MISSING_HELIX_PENALTY", self.defaults.EndHelixMissingPenalty)

        self.constants.setConstantDouble("SHEET_WEIGHT_COEFFICIENT", self.defaults.SheetImportance)
        # if(self.ui.checkBoxMissingSheets.isChecked()):
        #     self.constants.setConstant("MISSING_SHEET_COUNT", self.defaults.MissingSheetCount)
        # else:
        #     self.constants.setConstant("MISSING_SHEET_COUNT", -1)
        self.constants.setConstantInt("MISSING_SHEET_COUNT", -1)
        
        self.constants.setConstantDouble("MISSING_SHEET_PENALTY", self.defaults.SheetMissingPenalty)
        self.constants.setConstantDouble("MISSING_SHEET_PENALTY_SCALED", self.defaults.SheetMissingPenaltyScaled)

        # no longer needed?
        self.constants.setConstantBool("NORMALIZE_GRAPHS", True)

    def loadDefaultParams(self):
        # Graph Settings tab
        self.defaults.BorderMarginThreshold = 5
        self.defaults.EuclideanDistance = 0.0
        self.defaults.IncludeSheets = True
    
        self.defaults.ShowHelixCorners = False
        self.defaults.ShowSheetCorners = False
        self.defaults.ShowAllPaths = False
    
        # Matching Settings tab
        self.defaults.EuclideanToPDBRatio = 10.0
        self.defaults.AbsoluteDifference = True
        self.defaults.NormalizedDifference = False
        self.defaults.QuadraticError = False
    
        self.defaults.LoopImportance = 0.2
        self.defaults.EuclideanLoopUsedPenalty = 5.0
    
        self.defaults.HelixImportance = 1.0
        self.defaults.MissingHelices = False
        self.defaults.MissingHelixCount = 0
        self.defaults.HelixMissingPenalty = 5.0
        self.defaults.HelixMissingPenaltyScaled = 0.0
        self.defaults.EndHelixMissingPenalty = 5.0
    
        self.defaults.SheetImportance = 1.0
        self.defaults.MissingSheets = False
        self.defaults.MissingSheetCount = 0
        self.defaults.SheetMissingPenalty = 5.0
        self.defaults.SheetMissingPenaltyScaled = 0.0
        # 
        # # Results tab
        # self.ui.tableWidgetCorrespondenceList.clearContents()
        # self.ui.tabWidget.setCurrentIndex(0)
        # self.ui.comboBoxCorrespondences.setCurrentIndex(-1)
        
        # SSEDefaults
        self.defaults.HelixImportance           = 1.0
        self.defaults.HelixMissingPenalty       = 5.0
        self.defaults.HelixMissingPenaltyScaled = 0.0
        self.defaults.EndHelixMissingPenalty    = 8.0
        self.defaults.SheetImportance           = 1.0
        self.defaults.SheetMissingPenalty       = 5.0
        self.defaults.SheetMissingPenaltyScaled = 0.0
        self.defaults.BorderMarginThreshold           = 3
        self.defaults.EuclideanDistance         = 0.0
        self.defaults.MinSheetSize              = 10
        self.defaults.MaxSheetDistance          = 5.0
        self.defaults.SheetSelfLoopLength       = 5.0
        self.defaults.SheetMergeThreshold       = 3.0
        self.defaults.EuclideanToPDBRatio       = 10.0
        self.defaults.AbsoluteDifference          = True
        self.defaults.LoopImportance            = 0.2
        self.defaults.EuclideanLoopUsedPenalty  = 5.0

        self.defaults.MissingHelices = bool()
        self.defaults.MissingSheets = bool()

    def accept(self):
        self.loadDefaultParams()
        self.setConstants()
        self.correspondenceEngine.loadSequenceGraph()
        self.correspondenceEngine.loadSkeletonGraph()
        return self.correspondenceEngine.executeQuery()
