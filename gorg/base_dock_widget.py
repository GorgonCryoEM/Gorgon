from PyQt4 import QtCore, QtGui
from base_dock import BaseDock

class BaseDockWidget(QtGui.QWidget):
    def __init__(self, main, title, hint, actionName, menuName, parentMenuName, allowedAreas, defaultArea, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.defaultArea = defaultArea        
        
#         :TODO Maybe keep action and/or menu manager ????
        self.menuParent = dict((str(m.text()), m) for m in self.app.menubar.actions()).get('Window')

        if not self.menuParent:
            self.menuParent = self.app.menubar.addMenu('Window')
        else:
            self.menuParent = self.menuParent.menu()
        
        self.createDisplayAction(title, hint, actionName)

        self.createDock(title, allowedAreas)        

    def createDock(self, title, allowedAreas):
        self.dock = BaseDock(self.tr(title), self.displayAct, self.app)
        self.dock.setAllowedAreas(allowedAreas)
        self.dock.setWidget(self)
        self.dock.close()  
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
   
    def createDisplayAction(self, title, hint, actionName):               
        self.displayAct = QtGui.QAction(self.tr(title + "..."), self)
        self.displayAct.setStatusTip(self.tr(hint))
        self.displayAct.setCheckable(True)
        self.displayAct.setChecked(False)
        self.displayAct.triggered.connect(self.loadWidget)
        
        self.menuParent.addAction(self.displayAct)        
  

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
            self.bringToFront()
            
        else:
            self.displayAct.setChecked(False)
            self.app.removeDockWidget(self.dock)  
    
    def dockVisibilityChanged(self, visible):
        pass
        
    def bringToFront(self):
        self.dock.raise_()
    
    def closeEvent(self, event):
        QtGui.QWidget.closeEvent(self, event)
        self.displayAct.setChecked(False)
        
    def show(self):
        QtGui.QWidget.show(self)
        self.displayAct.setChecked(True)
        
    
    
    
    
