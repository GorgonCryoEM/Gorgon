# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   The main window of the Gorgon application 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.6  2008/10/15 19:41:32  ssa1
#   Esc to cancel path, Clear Button and Tracking of start seed point
#
#   Revision 1.5  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from menu_manager import MenuManager
from action_manager import ActionManager
from theme_manager import ThemeManager

class MainWindowForm(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.modules = []
        self.viewers = {}
        self.menus = MenuManager(self)       
        self.actions = ActionManager(self) 
        self.createUI()
        self.createActions()
        self.createMenus()
        self.themes = ThemeManager(self)
                
        self.statusBar().showMessage(self.tr("Gorgon: Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon - v0.1 beta"))

    def addModule(self, module):
        self.modules.append(module)
            
    def createUI(self):
        pass
        
    def createActions(self):
        exitAct = QtGui.QAction(self.tr("E&xit"), self)
        exitAct.setShortcut(self.tr("Ctrl+Q"))
        exitAct.setStatusTip(self.tr("Exit the application"))        
        self.connect(exitAct, QtCore.SIGNAL("triggered()"), QtGui.qApp.closeAllWindows)
        self.actions.addAction("exit_Application", exitAct)
                       
    def createMenus(self):
        self.menus.addMenu("file", self.tr("&File"))
        self.menus.addMenu("file-open", self.tr("&Open"), "file")
        self.menus.addMenu("file-save", self.tr("&Save"), "file")
        self.menus.addMenu("file-close", self.tr("&Close"), "file")
        self.menus.getMenu("file").addSeparator()
        self.menus.addAction("file-exit", self.actions.getAction("exit_Application"), "file")        
        self.menus.addMenu("options", self.tr("&Options"))    
        self.menus.addMenu("actions", self.tr("&Actions"))     
        self.menus.addMenu("window", self.tr("&Window"))
        self.menus.addMenu("themes", self.tr("&Themes"))
        self.menus.addMenu("help", self.tr("&Help"))
        
    def keyPressEvent(self, event):
        self.emitKeyPressed(event)
        
    def keyReleaseEvent(self, event):
        self.emitKeyReleased(event)
        
    def emitKeyPressed(self, event):
        self.emit(QtCore.SIGNAL("keyPressed(QKeyEvent)"), event);        

    def emitKeyReleased(self, event):
        self.emit(QtCore.SIGNAL("keyReleased(QKeyEvent)"), event);        
