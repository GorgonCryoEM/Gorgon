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
        self.HelixImportance           = 1.0
        self.HelixMissingPenalty       = 5.0
        self.HelixMissingPenaltyScaled = 0.0
        self.EndHelixMissingPenalty    = 8.0
        self.SheetImportance           = 1.0
        self.SheetMissingPenalty       = 5.0
        self.SheetMissingPenaltyScaled = 0.0
        self.BorderMarginThreshold           = 3
        self.EuclideanDistance         = 0.0
        self.MinSheetSize                    = 10
        self.MaxSheetDistance          = 5.0
        self.SheetSelfLoopLength       = 5.0
        self.SheetMergeThreshold       = 3.0
        self.EuclideanToPDBRatio       = 10.0
        self.AbsoluteDifference          = True
        self.LoopImportance            = 0.2
        self.EuclideanLoopUsedPenalty  = 5.0
        
        self.MissingHelices = bool()
        self.MissingSheets = bool()
        
        self.correspondences = ComboBox()
