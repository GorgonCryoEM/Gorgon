from PyQt4 import QtGui

class ActionManager(object):
    def __init__(self, main, parent=None):
        self.app = main
        self.actionList = {}

    def getAction(self, tag):
        if(tag in self.actionList) :
            return self.actionList[tag]
        else :
            QtGui.QMessageBox.critical(None, "Invalid Action", "Attempting to use non-existant action: " + tag, QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
            sys.exit(1)
        
    def addAction(self, tag, action):                   
        self.actionList[tag] = action    