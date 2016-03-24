from PyQt4 import QtCore, QtGui


class BaseDockWidget(QtGui.QWidget):

    def __init__(self, main, title, hint, actionName, menuName, parentMenuName, allowedAreas, defaultArea, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.defaultArea = defaultArea
                
        self.createDock(title, allowedAreas)

    def createDock(self, title, allowedAreas):
        self.dock = QtGui.QDockWidget(self.tr(title), self.app)
        self.dock.setAllowedAreas(allowedAreas)
        self.dock.setWidget(self)
        self.dock.close()
   
    def loadWidget(self):
        self.showWidget(True)

    def showWidget(self, show):
        if(show):
            self.app.addDockWidget(self.defaultArea, self.dock)
            self.dock.show()
            self.bringToFront()
    
    def bringToFront(self):
        self.dock.raise_()
    
    def show(self):
        QtGui.QWidget.show(self)
