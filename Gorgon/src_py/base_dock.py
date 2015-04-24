# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A base class for dock widgets

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/base_dock.py,v $
#   $Revision: 1.2 $
#   $Date: 2009/12/24 21:53:49 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: base_dock.py,v $
#   Revision 1.2  2009/12/24 21:53:49  ssa1
#   Giving back color control to the SSE Visualization options form when SSE Correspondence engine is not running (Bug ID: 58)
#
#   Revision 1.1  2009/12/24 19:37:36  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior



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