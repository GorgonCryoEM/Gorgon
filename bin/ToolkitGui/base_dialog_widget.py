from PyQt4 import QtCore, QtGui


class BaseDialogWidget(QtGui.QDialog):

    def __init__(self, main, title, hint, actionName, windowName, parentWindowName, isModal, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.app = main
        self.setModal(isModal)
        self.createDisplayAction(title, hint, actionName)
        
    def createDisplayAction(self, title, hint, actionName):
        self.displayAct = QtGui.QAction(self.tr(title + "..."), self)
        self.displayAct.setStatusTip(self.tr(hint))
        self.displayAct.setCheckable(True)
        self.displayAct.setChecked(False)
        self.connect(self.displayAct, QtCore.SIGNAL("triggered()"), self.toggleVisibility)
        
    def createDisplayMenu(self, windowName, parentWindowName):
        self.app.menus.addAction(windowName, self.displayAct, parentWindowName)
        
    def toggleVisibility(self):
        if(self.isVisible()):
            self.close()
        else:
            self.show()
        
    def show(self):
        QtGui.QDialog.show(self)
        self.displayAct.setChecked(True)
        
    def closeEvent(self, event):
        QtGui.QDialog.closeEvent(self, event)
        self.displayAct.setChecked(False)
            
            
