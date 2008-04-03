import sys
from topology_hunter import TopologyHunter
from about_window import Ui_DialogAbout
from PyQt4 import QtCore, QtGui

class MainWindow(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self, parent)

        self.modules = []

        self.createActions()
        self.createMenus()
        
        self.statusBar().showMessage(self.tr("Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon Prototype"))

    def addModule(self, module):
        self.modules.append(module)
        
    def createActions(self):
        self.exitAct = QtGui.QAction(self.tr("E&xit"), self)
        self.exitAct.setShortcut(self.tr("Ctrl+Q"))
        self.exitAct.setStatusTip(self.tr("Exit the application"))
                
        self.aboutAct = QtGui.QAction(self.tr("&About"), self)
        self.aboutAct.setStatusTip(self.tr("About Gorgon"))
        self.aboutWindow = Ui_DialogAbout()
        self.connect(self.aboutAct, QtCore.SIGNAL("show()"), self.aboutWindow )
        
        self.connect(self.exitAct, QtCore.SIGNAL("triggered()"), QtGui.qApp.closeAllWindows)
        
    def createMenus(self):
        self.fileMenu = self.menuBar().addMenu(self.tr("&File"))
        self.fileMenu.addSeparator()
        self.fileMenu.addAction(self.exitAct)

        self.openMenu = self.fileMenu.addMenu(self.tr("&Open"))
        
        self.optionsMenu = self.menuBar().addMenu(self.tr("&Options"))
                
        self.helpMenu = self.menuBar().addMenu(self.tr("&Help"))
        self.helpMenu.addAction(self.aboutAct)


    def menuOpen(self):
        return self.openMenu

    def menuOptions(self):
        return self.optionsMenu
