import cPickle as pickle


class CorrespondenceLibrary:
    def __init__(self, sp=None, so=None, algo=None, params=None, correspondenceList=None, comments=None, filename=None):
        if (filename):
            self.load(filename)
        else:
            self.structurePrediction = sp
            self.structureObservation = so
            self.algorithm = algo
            self.params = params
            self.correspondenceList = correspondenceList
            self.currentCorrespondenceIndex = None
            self.otherAttribs = {}
            self.otherAttribs['comments'] = comments
            # sort correspondences by score

    def getCurrentCorrespondenceIndex(self):
        return self.currentCorrespondenceIndex

    def setCurrentCorrespondenceIndex(self, corrIndex):
        self.currentCorrespondenceIndex = corrIndex
