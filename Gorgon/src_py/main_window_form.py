from PyQt4 import QtCore, QtGui
from menu_manager import MenuManager
from action_manager import ActionManager

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
        self.menus.addMenu("file-close", self.tr("&Close"), "file")
        self.menus.getMenu("file").addSeparator()
        self.menus.addAction("file-exit", self.actions.getAction("exit_Application"), "file")        
        self.menus.addMenu("options", self.tr("&Options"))    
        self.menus.addMenu("actions", self.tr("&Actions"))     
        self.menus.addMenu("window", self.tr("&Window"))
        self.menus.addMenu("help", self.tr("&Help"))
