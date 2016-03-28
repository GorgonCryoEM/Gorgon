from PyQt4 import QtCore, QtGui


class BaseDockWidget(QtGui.QWidget):

    def __init__(self, main, parent, title, allowedAreas, defaultArea):
        self.app = main
        self.defaultArea = defaultArea
                
        QtGui.QWidget.__init__(self)
        self.dock = QtGui.QDockWidget(title, self.app)
        self.dock.setAllowedAreas(allowedAreas)
        self.dock.setWidget(parent)
   
    def show(self):
        self.app.addDockWidget(self.defaultArea, self.dock)
        self.dock.show()
        self.dock.raise_()
