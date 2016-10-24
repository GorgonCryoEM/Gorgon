import json


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
        with open('sse_defaults.json') as data_file:
            json_data = json.load(data_file)
        
        self.HelixImportance           = json_data["HelixImportance"]          
        self.HelixMissingPenalty       = json_data["HelixMissingPenalty"]      
        self.HelixMissingPenaltyScaled = json_data["HelixMissingPenaltyScaled"]
        self.EndHelixMissingPenalty    = json_data["EndHelixMissingPenalty"]   
        self.SheetImportance           = json_data["SheetImportance"]          
        self.SheetMissingPenalty       = json_data["SheetMissingPenalty"]      
        self.SheetMissingPenaltyScaled = json_data["SheetMissingPenaltyScaled"]
        self.BorderMarginThreshold     = json_data["BorderMarginThreshold"]    
        self.EuclideanDistance         = json_data["EuclideanDistance"]        
        self.MinSheetSize              = json_data["MinSheetSize"]             
        self.MaxSheetDistance          = json_data["MaxSheetDistance"]         
        self.SheetSelfLoopLength       = json_data["SheetSelfLoopLength"]      
        self.SheetMergeThreshold       = json_data["SheetMergeThreshold"]      
        self.EuclideanToPDBRatio       = json_data["EuclideanToPDBRatio"]      
        self.AbsoluteDifference        = json_data["AbsoluteDifference"]       
        self.LoopImportance            = json_data["LoopImportance"]           
        self.EuclideanLoopUsedPenalty  = json_data["EuclideanLoopUsedPenalty"] 
        
        self.MissingHelices = json_data["MissingHelices"]
        self.MissingSheets  = json_data["MissingSheets"]
        
        self.correspondences = ComboBox()
