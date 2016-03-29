from PyQt4 import QtCore, QtGui


class BaseDockWidget(QtGui.QWidget):

    def __init__(self, main, parent, title, allowedAreas, defaultArea):
        QtGui.QWidget.__init__(self)
        self.dock = QtGui.QDockWidget(title, main)
        self.dock.setAllowedAreas(allowedAreas)
        self.dock.setWidget(parent)
        main.addDockWidget(defaultArea, self.dock)
