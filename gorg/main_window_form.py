from PyQt4 import QtCore, QtGui
from menu_manager import MenuManager
from action_manager import ActionManager
from window_manager import WindowManager

import sys, os

class MainWindowForm(QtGui.QMainWindow):
    def __init__(self, version):
        super(MainWindowForm, self).__init__()
        self.version = version

        self.viewers = {}
        self.menus = MenuManager(self)       
        self.actions = ActionManager(self) 

        self.createActions()
        self.createMenus()
        self.setContextMenuPolicy(QtCore.Qt.NoContextMenu)
        self.dockWidgets = []
        self.windowManager = WindowManager(self)
                
        self.statusBar().showMessage(self.tr("Gorgon: Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon Explorer - v" + self.version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def createActions(self):
        exitAct = QtGui.QAction(self.tr("E&xit"), self)
        exitAct.setShortcut(self.tr("Ctrl+Q"))
        exitAct.setStatusTip(self.tr("Exit the application"))        
        self.connect(exitAct, QtCore.SIGNAL("triggered()"), self.exitApplication)
        self.actions.addAction("exit_Application", exitAct)
                       
    def createMenus(self):
        self.menus.addMenu("file", self.tr("&File"))
        self.menus.addMenu("file-open", self.tr("&Open"), "file")
        self.menus.addMenu("file-save", self.tr("&Save"), "file")
        self.menus.addMenu("file-export", self.tr("&Export"), "file")
        self.menus.addMenu("file-close", self.tr("&Close"), "file")
        self.menus.getMenu("file").addSeparator()
        self.menus.addAction("file-exit", self.actions.getAction("exit_Application"), "file")        
        self.menus.addMenu("window", self.tr("&Window"))
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
    
    def isDockWidget(self, dockWidget):
        isWidget = False
        for widget in self.dockWidgets:
            isWidget = isWidget or (widget == dockWidget)
        return isWidget
    
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
            
    def closeEvent(self, event):
        exitText = "This will close Gorgon, you will lose all unsaved data.\nAre you sure?"
        
        if (QtGui.QMessageBox.warning (self, self.tr("Exit Gorgon?"), self.tr(exitText), QtGui.QMessageBox.Yes | QtGui.QMessageBox.Cancel) == QtGui.QMessageBox.Yes) :
            event.accept()
        else :
            event.ignore()


    
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
