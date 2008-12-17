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
#   Revision 1.13  2008/12/03 20:27:05  ssa1
#   adding in bug tracker
#
#   Revision 1.12  2008/12/02 21:10:06  colemanr
#   Added a file-export menu.
#
#   Revision 1.11  2008/12/01 01:25:54  ssa1
#   Docking widgets as tabs to preserve screen real-estate
#
#   Revision 1.10  2008/11/30 00:25:44  ssa1
#   Splash screen for public beta 1 release
#
#   Revision 1.9  2008/11/29 04:48:28  ssa1
#   Icon support and Redirecting help to website.
#
#   Revision 1.8  2008/11/25 21:03:40  ssa1
#   User constraints on finding correspondences (v3)
#
#   Revision 1.7  2008/11/06 05:29:04  ssa1
#   CGI submission milestone for Interactive Skeletonization, and theme support, and fixing (hopefully) mac-os flicker bug
#
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
import sys, os

class MainWindowForm(QtGui.QMainWindow):
    def __init__(self, version, parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.version = version
        self.modules = []
        self.viewers = {}
        self.menus = MenuManager(self)       
        self.actions = ActionManager(self) 
        self.createUI()
        self.createActions()
        self.createMenus()
        self.themes = ThemeManager(self)
        self.setContextMenuPolicy(QtCore.Qt.NoContextMenu)
        self.dockWidgets = []
                
        self.statusBar().showMessage(self.tr("Gorgon: Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon - v" + self.version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))

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
        self.menus.addMenu("file-export", self.tr("&Export"), "file")
        self.menus.addMenu("file-close", self.tr("&Close"), "file")
        self.menus.getMenu("file").addSeparator()
        self.menus.addAction("file-exit", self.actions.getAction("exit_Application"), "file")        
        #self.menus.addMenu("options", self.tr("&Options"))    
        self.menus.addMenu("actions", self.tr("&Actions"))     
        self.menus.addMenu("window", self.tr("&Window"))
        self.menus.addMenu("themes", self.tr("&Themes"))
        self.menus.addMenu("help", self.tr("&Help"))
        
    def addDockWidget (self, area, dockwidget):
        QtGui.QMainWindow.addDockWidget(self, area, dockwidget)
        dockwidget.area = area
        otherwidget = None
        for widget in self.dockWidgets:
            if (widget.area == area) and (widget != dockwidget):
                otherwidget = widget
        if(otherwidget):
            self.tabifyDockWidget(otherwidget, dockwidget)
        self.dockWidgets.append(dockwidget)
        self.connect(dockwidget, QtCore.SIGNAL("dockLocationChanged ( Qt::DockWidgetArea )"), self.dockLocationChanged(dockwidget))
        
    def removeDockWidget (self, dockwidget):
        QtGui.QMainWindow.removeDockWidget(self, dockwidget)
        if(dockwidget in self.dockWidgets):
            self.dockWidgets.remove(dockwidget)
            self.disconnect(dockwidget, QtCore.SIGNAL("dockLocationChanged ( Qt::DockWidgetArea )"), self.dockLocationChanged(dockwidget))
    
    def dockLocationChanged(self, widget):
        def dockLocationChanged_widget(area):
            widget.area = area
        return dockLocationChanged_widget
    
        
    def keyPressEvent(self, event):
        self.emitKeyPressed(event)
        
    def keyReleaseEvent(self, event):
        self.emitKeyReleased(event)
        
    def emitKeyPressed(self, event):
        self.emit(QtCore.SIGNAL("keyPressed(QKeyEvent)"), event);        

    def emitKeyReleased(self, event):
        self.emit(QtCore.SIGNAL("keyReleased(QKeyEvent)"), event);        
