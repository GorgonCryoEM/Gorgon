from PyQt4 import QtCore, QtGui

import sys, os


class MainWindowForm(QtGui.QMainWindow):

    def __init__(self, version):
        super(MainWindowForm, self).__init__()
        
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
