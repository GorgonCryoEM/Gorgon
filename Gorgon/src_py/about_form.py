from PyQt4 import QtCore, QtGui

from ui_dialog_about import Ui_DialogAbout


class AboutForm(QtGui.QWidget):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.ui = Ui_DialogAbout()
        self.ui.setupUi(self)       
        self.createActions()
        self.createMenus()

    def createActions(self):               
        self.aboutAct = QtGui.QAction(self.tr("&About"), self)
        self.aboutAct.setStatusTip(self.tr("About Gorgon"))
        self.connect(self.aboutAct, QtCore.SIGNAL("triggered()"), self.showUI )
  
    def createMenus(self):
        self.app.menuHelp().addAction(self.aboutAct)
        
    def showUI(self):
        self.show()