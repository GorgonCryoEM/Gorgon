class ActionManager(object):
    def __init__(self, main, parent=None):
        self.app = main
        self.actionList = {}

    def getAction(self, tag):
        if(tag in self.actionList) :
            return self.actionList[tag]
        else :
            raise Exception("Attempting to use non-existant action: " + tag)
        
    def addAction(self, tag, action):                   
        self.actionList[tag] = action    
