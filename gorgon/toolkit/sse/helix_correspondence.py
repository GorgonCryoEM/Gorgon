from ..libpytoolkit import SSEEngine, IBackEnd
from .defaults import SSEDefaults
import json


class HelixCorrespondence(object):

    def __init__(self, args):
        self.defaults = SSEDefaults()
        if args.json:
            with open(args.json) as data_file:
                json_data = json.load(data_file)
            for key, val in json_data.iteritems():
                setattr(self.defaults, key, val)
        
        self.skeleton = args.skeleton
        self.sequence = args.sequence
        self.helix    = args.helix
        self.output   = args.output
                
        '''
        SSEViewer
        '''
        self.correspondenceEngine = SSEEngine()
        self.constants = IBackEnd()
        
    def setConstants(self):
        #Data Sources tab
        #self.constants.setConstant("SSE_FILE_NAME", str(self.ui.lineEditHelixLengthFile.text()))
        self.constants.setConstant("VRML_HELIX_FILE_NAME", self.helix)
#         self.constants.setConstant("VRML_SHEET_FILE_NAME", str(self.ui.lineEditSheetLocationFile.text()))
        self.constants.setConstant("MRC_FILE_NAME", self.skeleton)
        self.sequenceFileName = self.sequence
        self.constants.setConstant("SEQUENCE_FILE_NAME", self.sequenceFileName)
        if self.sequenceFileName.split('.')[-1].lower() == 'pdb':
            self.constants.setConstant("SEQUENCE_FILE_TYPE", "PDB")
        elif self.sequenceFileName.split('.')[-1].lower() == 'seq':
            self.constants.setConstant("SEQUENCE_FILE_TYPE", "SEQ")
        
        #Graph Settings tab
        self.constants.setConstant("BORDER_MARGIN_THRESHOLD", self.defaults.BorderMarginThreshold)
        self.constants.setConstant("EUCLIDEAN_DISTANCE_THRESHOLD", self.defaults.EuclideanDistance)
        self.constants.setConstant("MAXIMUM_DISTANCE_SHEET_SKELETON", self.defaults.MaxSheetDistance)
        self.constants.setConstant("MINIMUM_SHEET_SIZE", self.defaults.MinSheetSize)
        self.constants.setConstant("SHEET_SELF_LOOP_LENGTH", self.defaults.SheetSelfLoopLength)
        self.constants.setConstant("SHEET_MERGE_THRESHOLD", self.defaults.SheetMergeThreshold)
        # if (self.ui.checkBoxIncludeStrands.isChecked()):
        #     self.constants.setConstant("INCLUDE_STRANDS", 1)
        # else:
        #     self.constants.setConstant("INCLUDE_STRANDS", 0)

        #Matching Settings tab
        self.constants.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.defaults.EuclideanToPDBRatio)
        if(self.defaults.AbsoluteDifference):
            self.constants.setConstant("COST_FUNCTION", 1)
        elif (self.defaults.NormalizedDifference.isChecked()):
            self.constants.setConstant("COST_FUNCTION", 2)
        else:
            self.constants.setConstant("COST_FUNCTION", 3)

        self.constants.setConstant("LOOP_WEIGHT_COEFFICIENT", self.defaults.LoopImportance)
        self.constants.setConstant("EUCLIDEAN_LOOP_PENALTY", self.defaults.EuclideanLoopUsedPenalty)

        self.constants.setConstant("HELIX_WEIGHT_COEFFICIENT", self.defaults.HelixImportance)
        # if(self.ui.checkBoxMissingHelices.isChecked()):
        #     self.constants.setConstant("MISSING_HELIX_COUNT", self.defaults.MissingHelixCount)
        # else:
        #     self.constants.setConstant("MISSING_HELIX_COUNT", -1)
        self.constants.setConstant("MISSING_HELIX_COUNT", -1)
        
        self.constants.setConstant("MISSING_HELIX_PENALTY", self.defaults.HelixMissingPenalty)
        self.constants.setConstant("MISSING_HELIX_PENALTY_SCALED", self.defaults.HelixMissingPenaltyScaled)
        self.constants.setConstant("START_END_MISSING_HELIX_PENALTY", self.defaults.EndHelixMissingPenalty)

        self.constants.setConstant("SHEET_WEIGHT_COEFFICIENT", self.defaults.SheetImportance)
        # if(self.ui.checkBoxMissingSheets.isChecked()):
        #     self.constants.setConstant("MISSING_SHEET_COUNT", self.defaults.MissingSheetCount)
        # else:
        #     self.constants.setConstant("MISSING_SHEET_COUNT", -1)
        self.constants.setConstant("MISSING_SHEET_COUNT", -1)
        
        self.constants.setConstant("MISSING_SHEET_PENALTY", self.defaults.SheetMissingPenalty)
        self.constants.setConstant("MISSING_SHEET_PENALTY_SCALED", self.defaults.SheetMissingPenaltyScaled)

        # no longer needed?
        self.constants.setConstant("NORMALIZE_GRAPHS", True)


    def accept(self):
        self.setConstants()
        self.correspondenceEngine.loadSequenceGraph()
        self.correspondenceEngine.loadSkeletonGraph()
        self.correspondenceEngine.executeQuery()
