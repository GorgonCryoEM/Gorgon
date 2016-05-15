from libpytoolkit import SSEEngine
from sse_defaults import SSEDefaults


class SSEHelixCorrespondence(object):

    def __init__(self, skeleton, sequence, helix, output, auto=True):
        self.ui = SSEDefaults()
        self.skeleton = skeleton
        self.sequence = sequence
        self.helix    = helix
        self.output   = output
                
        '''
        SSEViewer
        '''
        self.correspondenceEngine = SSEEngine()
        
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

        #Matching Settings tab
        self.correspondenceEngine.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.ui.EuclideanToPDBRatio)
        if(self.ui.AbsoluteDifference):
            self.correspondenceEngine.setConstantInt("COST_FUNCTION", 1)
        elif (self.ui.NormalizedDifference.isChecked()):
            self.correspondenceEngine.setConstantInt("COST_FUNCTION", 2)
        else:
            self.correspondenceEngine.setConstantInt("COST_FUNCTION", 3)

        self.correspondenceEngine.setConstant("LOOP_WEIGHT_COEFFICIENT", self.ui.LoopImportance)

    def createBasicCorrespondence(self):
        self.setConstants()
        self.correspondenceEngine.loadSequenceGraph()
        self.correspondenceEngine.loadSkeletonGraph()

    def accept(self):
        self.createBasicCorrespondence()
        self.correspondenceEngine.executeQuery()
