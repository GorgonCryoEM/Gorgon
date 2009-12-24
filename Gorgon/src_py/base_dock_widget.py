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

from PyQt4 import QtCore, QtGui

class BaseDockWidget(QtGui.QWidget):
    def __init__(self, main, title, hint, actionName, windowName, parentWindowName, allowedAreas, defaultArea, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.defaultArea = defaultArea        
        self.createDock(title, allowedAreas)
        self.createDisplayAction(title, hint, actionName)
        self.createDisplayMenu(windowName, parentWindowName)

    def createDock(self, title, allowedAreas):
        self.dock = QtGui.QDockWidget(self.tr(title), self.app)
        self.dock.setAllowedAreas(allowedAreas)
        self.dock.setWidget(self)
        self.dock.close()  
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
    
    def createDisplayAction(self, title, hint, actionName):               
        self.displayAct = QtGui.QAction(self.tr(title), self)
        self.displayAct.setStatusTip(self.tr(hint))
        self.displayAct.setCheckable(True)
        self.displayAct.setChecked(False)
        self.connect(self.displayAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction(actionName,  self.displayAct)
  
    def createDisplayMenu(self, windowName, parentWindowName):
        self.app.menus.addAction(windowName, self.displayAct, parentWindowName)                                   
                
    def loadWidget(self):
        if(self.displayAct.isChecked()) :
            self.showWidget(True)
        else:
            self.showWidget(False)

    def showWidget(self, show):
        if(show):
            self.displayAct.setChecked(True)
            self.app.addDockWidget(self.defaultArea, self.dock)
            self.dock.show()
            self.dock.raise_()
        else:
            self.displayAct.setChecked(False)
            self.app.removeDockWidget(self.dock)  

    def dockVisibilityChanged(self, visible):
        self.displayAct.setChecked(visible)
        
    
    
    
    
    
