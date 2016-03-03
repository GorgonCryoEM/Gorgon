from PyQt4 import QtCore, QtGui
from camera import Camera
from volume_viewer import VolumeViewer
from skeleton_viewer import SkeletonViewer

import sys, os


class MainWindowForm(QtGui.QMainWindow):

    def __init__(self, version):
        super(MainWindowForm, self).__init__()

        self.volumeViewer = VolumeViewer(self)
        self.skeletonViewer = VolumeViewer(self)

        self.mainCamera = Camera([self.volumeViewer, self.skeletonViewer], self)
        self.setCentralWidget(self.mainCamera)
                
        self.statusBar().showMessage(self.tr("Gorgon: Protein Visualization Suite"))
        self.setWindowTitle(self.tr("Gorgon Explorer - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def addDockWidget (self, area, dockwidget):
        QtGui.QMainWindow.addDockWidget(self, area, dockwidget)
        dockwidget.area = area
        dockwidget.dockLocationChanged.connect(self.dockLocationChanged(dockwidget))
        
    def removeDockWidget (self, dockwidget):
        QtGui.QMainWindow.removeDockWidget(self, dockwidget)
        if(dockwidget in self.dockWidgets):
            self.dockWidgets.remove(dockwidget)
            dockwidget.dockLocationChanged.disconnect()
    
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
            
#     def closeEvent(self, event):
#         exitText = "This will close Gorgon, you will lose all unsaved data.\nAre you sure?"
#
#         if (QtGui.QMessageBox.warning (self, self.tr("Exit Gorgon?"), self.tr(exitText), QtGui.QMessageBox.Yes | QtGui.QMessageBox.Cancel) == QtGui.QMessageBox.Yes):
#             event.accept()
#         else:
#             event.ignore()

    def dockLocationChanged(self, widget):
        def dockLocationChanged_widget(area):
            widget.area = area
        return dockLocationChanged_widget

