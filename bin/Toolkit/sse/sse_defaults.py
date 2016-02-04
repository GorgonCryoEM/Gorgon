class ComboBox(object):

    def __init__(self):
        self.ind = -1
        self.arr = []
        
    def currentIndex(self):
        return self.ind

    def clear(self):
        self.ind = -1
        self.arr = []
        
    def addItem(self, item):
        self.arr.append(item)


class SSEDefaults(object):

    def __init__(self):
        self.doubleSpinBoxHelixImportance           = 1.0
        self.doubleSpinBoxHelixMissingPenalty       = 5.0
        self.doubleSpinBoxHelixMissingPenaltyScaled = 0.0
        self.doubleSpinBoxEndHelixMissingPenalty    = 8.0
        self.doubleSpinBoxSheetImportance           = 1.0
        self.doubleSpinBoxSheetMissingPenalty       = 5.0
        self.doubleSpinBoxSheetMissingPenaltyScaled = 0.0
        self.spinBoxBorderMarginThreshold           = 3
        self.doubleSpinBoxEuclideanDistance         = 0.0
        self.spinBoxMinSheetSize                    = 10
        self.doubleSpinBoxMaxSheetDistance          = 5.0
        self.doubleSpinBoxSheetSelfLoopLength       = 5.0
        self.doubleSpinBoxSheetMergeThreshold       = 3.0
        self.doubleSpinBoxEuclideanToPDBRatio       = 10.0
        self.radioButtonAbsoluteDifference          = True
        self.doubleSpinBoxLoopImportance            = 0.2
        self.doubleSpinBoxEuclideanLoopUsedPenalty  = 5.0
        
        self.checkBoxMissingHelices = bool()
        self.checkBoxMissingSheets = bool()
        
        self.comboBoxCorrespondences = ComboBox()
