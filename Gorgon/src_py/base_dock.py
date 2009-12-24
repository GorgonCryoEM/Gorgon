# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A base class for dock widgets

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.2  2009/12/24 07:25:07  ssa1


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