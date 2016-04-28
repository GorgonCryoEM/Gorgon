from PyQt4 import QtCore, QtGui
import sys, os

from Explorer import Camera
from Explorer.volume_viewer import VolumeViewer


class MainWindowForm(QtGui.QMainWindow):

    def __init__(self, version, args):
        super(MainWindowForm, self).__init__()
        
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        
        vol = VolumeViewer(self)
#         vol.load(args.volume)
        self.mainCamera = Camera([vol], self)
        self.setCentralWidget(self.mainCamera)
        
        self.setWindowTitle(self.tr("Gorgon Toolkit - v" + version))
        pathname = os.path.abspath(os.path.dirname(sys.argv[0]))
        self.setWindowIcon(QtGui.QIcon(pathname + '/gorgon.ico'))
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
