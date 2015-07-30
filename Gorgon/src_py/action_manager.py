# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   Maintains a repository of all the actions in the system. 



from PyQt4 import QtGui

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
