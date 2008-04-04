from PyQt4 import QtGui

class MenuManager(object):
    def __init__(self, main, parent=None):
        self.app = main
        self.menuList = {}

    def getMenu(self, tag):
        if(tag in self.menuList) :
            return self.menuList[tag]
        else :
            QtGui.QMessageBox.critical(None, "Invalid Menu", "Attempting to use non-existant menu item: " + tag, QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default, QtGui.QMessageBox.NoButton)
            sys.exit(1)
        
    def addMenu(self, tag, text, parentTag=""):        
        if (parentTag == "") :
            parentMenu = self.app.menuBar()
        else :
            parentMenu = self.getMenu(parentTag) 
            
        self.menuList[tag] = parentMenu.addMenu(text)
        return self.menuList[tag]
    
    def addAction(self, tag, action, parentTag):
        self.menuList[parentTag].addAction(action)