from ..libpytoolkit import SSEEngine, IBackEnd
from .defaults import SSEDefaults


class SSEHelixCorrespondence(object):

    def __init__(self, args, auto=True):
        self.defaults = SSEDefaults()
        self.skeleton = args.skeleton
        self.sequence = args.sequence
        self.helix    = args.helix
        self.output   = args.output
                
        '''
        SSEViewer
        '''
        self.correspondenceEngine = SSEEngine()
        self.constants = IBackEnd()
        
        if auto:
            self.accept()
            self.correspondenceEngine.saveCorrespondenceToFile(self.output)
        
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

        #Matching Settings tab
        self.constants.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.defaults.EuclideanToPDBRatio)
        if(self.defaults.AbsoluteDifference):
            self.constants.setConstant("COST_FUNCTION", 1)
        elif (self.defaults.NormalizedDifference.isChecked()):
            self.constants.setConstant("COST_FUNCTION", 2)
        else:
            self.constants.setConstant("COST_FUNCTION", 3)

        self.constants.setConstant("LOOP_WEIGHT_COEFFICIENT", self.defaults.LoopImportance)

    def accept(self):
        self.setConstants()
        self.correspondenceEngine.loadSequenceGraph()
        self.correspondenceEngine.loadSkeletonGraph()
        self.correspondenceEngine.executeQuery()
