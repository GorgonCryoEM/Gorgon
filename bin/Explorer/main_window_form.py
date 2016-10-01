from PyQt4 import QtCore, QtGui
from camera import Camera
from scene import Scene
from .widget3 import Widget3

import sys, os


class MainWindowForm(QtGui.QMainWindow):

    def __init__(self, version):
        super(MainWindowForm, self).__init__()
        
        self.form = Widget3(self)
        self.form.show()
        
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')

        self.scene = Scene(self)
        scenes = self.scene.getShapes()

        self.mainCamera = Camera(scenes, self)
        self.setCentralWidget(self.mainCamera)
        
#         self.form.raise_()
        
#         self.dockWidgets = []
                
        self.statusBar().showMessage(self.tr("Gorgon: Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon Explorer - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def load(self):
        self.scene.load()

#         dockwidget.dockLocationChanged.connect(self.dockLocationChanged(dockwidget))
#
#     def removeDockWidget (self, dockwidget):
#         QtGui.QMainWindow.removeDockWidget(self, dockwidget)
#         if(dockwidget in self.dockWidgets):
#             self.dockWidgets.remove(dockwidget)
#             dockwidget.dockLocationChanged.disconnect()
    
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
            
#     def closeEvent(self, event):
#         exitText = "This will close Gorgon, you will lose all unsaved data.\nAre you sure?"
#
#         if (QtGui.QMessageBox.warning (self, self.tr("Exit Gorgon?"), self.tr(exitText), QtGui.QMessageBox.Yes | QtGui.QMessageBox.Cancel) == QtGui.QMessageBox.Yes):
#             event.accept()
#         else:
#             event.ignore()

#     def dockLocationChanged(self, widget):
#         def dockLocationChanged_widget(area):
#             widget.area = area
#         return dockLocationChanged_widget
