# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A base class for dock widgets




from PyQt4 import QtCore, QtGui

class BaseDock(QtGui.QDockWidget):
    def __init__(self, title, displayAct, parent=None):
        QtGui.QDockWidget.__init__(self, title, parent)
        self.displayAct = displayAct
    
    def closeEvent(self, event):
        QtGui.QDockWidget.closeEvent(self, event)
        self.displayAct.setChecked(False)
    
    def show(self):
        QtGui.QDockWidget.show(self)
        self.displayAct.setChecked(True) 
