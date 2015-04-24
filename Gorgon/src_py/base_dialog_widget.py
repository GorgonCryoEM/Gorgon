# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A base class for dialog boxes triggered through an action

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/base_dialog_widget.py,v $
#   $Revision: 1.2 $
#   $Date: 2009/12/24 07:25:07 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: base_dialog_widget.py,v $
#   Revision 1.2  2009/12/24 07:25:07  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior
#
#   Revision 1.1  2009/12/24 05:09:30  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior
#


from PyQt4 import QtCore, QtGui

class BaseDialogWidget(QtGui.QDialog):
    def __init__(self, main, title, hint, actionName, windowName, parentWindowName, isModal, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.app = main
        self.setModal(isModal)
        self.createDisplayAction(title, hint, actionName)
        self.createDisplayMenu(windowName, parentWindowName)
        
    def createDisplayAction(self, title, hint, actionName):
        self.displayAct = QtGui.QAction(self.tr(title + "..."), self)        
        self.displayAct.setStatusTip(self.tr(hint))
        self.displayAct.setCheckable(True)
        self.displayAct.setChecked(False)   
        self.connect(self.displayAct, QtCore.SIGNAL("triggered()"), self.toggleVisibility)
        self.app.actions.addAction(actionName, self.displayAct)
        
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
            
            