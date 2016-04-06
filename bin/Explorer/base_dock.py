from PyQt4 import QtCore, QtGui


class BaseDock(QtGui.QDockWidget):

    def __init__(self, title, parent=None):
        QtGui.QDockWidget.__init__(self, title, parent)
    
    def closeEvent(self, event):
        QtGui.QDockWidget.closeEvent(self, event)
    
    def show(self):
        QtGui.QDockWidget.show(self)
