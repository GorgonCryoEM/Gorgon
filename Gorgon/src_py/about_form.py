# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: This form displays information about Gorgon.


from PyQt4 import QtCore, QtGui
from ui_dialog_about import Ui_DialogAbout

class AboutForm(QtGui.QDialog):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.ui = Ui_DialogAbout()
        self.ui.setupUi(self)
        self.createActions()
        self.createMenus()

    def createActions(self):               
        aboutAct = QtGui.QAction(self.tr("&About"), self)
        aboutAct.setStatusTip(self.tr("About Gorgon"))
        self.connect(aboutAct, QtCore.SIGNAL("triggered()"), self.show)
        self.app.actions.addAction("show_AboutForm", aboutAct)
  
    def createMenus(self):
        self.app.menus.addAction("help-about", self.app.actions.getAction("show_AboutForm"), "help")        