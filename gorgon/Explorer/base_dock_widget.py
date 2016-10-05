from PyQt4 import QtCore, QtGui


class BaseDockWidget(QtGui.QWidget):

    def __init__(self, main, parent, title, allowedAreas, defaultArea):
        QtGui.QWidget.__init__(self)
        self.dock = QtGui.QDockWidget(title, main)
        self.dock.setAllowedAreas(allowedAreas)
        self.dock.setWidget(parent)
        main.addDockWidget(defaultArea, self.dock)
        if main.docks:
            main.tabifyDockWidget(main.docks[-1], self.dock)
        main.docks.append(self.dock)
        main.docks[0].raise_()
        
        dockVisible = self.toggleViewAction()

        main.docksMenu.addAction(dockVisible)

    def toggleViewAction(self):
        return self.dock.toggleViewAction()
