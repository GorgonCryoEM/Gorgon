from PyQt4 import QtCore, QtGui


class BaseDockWidget(QtGui.QWidget):

    def __init__(self, main, title, allowedAreas, defaultArea):
        self.app = main
        self.defaultArea = defaultArea
                
        QtGui.QWidget.__init__(self)
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
