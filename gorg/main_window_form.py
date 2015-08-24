from PyQt4 import QtCore, QtGui
from camera import Camera
from volume_viewer import VolumeViewer

import sys, os

class MainWindowForm(QtGui.QMainWindow):
    def __init__(self, version):
        super(MainWindowForm, self).__init__()
        self.version = version

        self.viewers = {}
        
        self.menubar = self.menuBar()
        self.explorerMenu = self.menubar.addMenu(self.tr('&Explorer'))
        
        exitAction = QtGui.QAction('&Exit', self)
        exitAction.setShortcut(self.tr('Ctrl+Q'))
#         exitAction.setStatusTip('Exit application')
        exitAction.triggered.connect(QtGui.qApp.quit)
        self.explorerMenu.addAction(exitAction)
        
        self.dockWidgets = []
        
        self.windowManager = QtGui.QWidget(self)

        self.volumeViewer = VolumeViewer(self)
        self.mainCamera = Camera([self.volumeViewer], self)
        self.setCentralWidget(self.mainCamera)
                
        self.statusBar().showMessage(self.tr("Gorgon: Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon Explorer - v" + self.version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
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
