import os
import sys

from PyQt4 import QtGui

from .explorer import Camera


class Window(QtGui.QMainWindow):

    def __init__(self, args, form):
        super(Window, self).__init__()
        
        self.args = args
        self.menubar = self.menuBar()
        self.docksMenu = self.menubar.addMenu('&Docks')
        self.docks = []
        
        self.scene = []
        
        self.toolbar = self.addToolBar("Toolbar")
        self.mainCamera = Camera(self.scene, self)
        
        self.setCentralWidget(self.mainCamera)
        
        if form:
            self.form = form(self)
            if(hasattr(self.form, 'show')):
                self.form.show()
        
        self.setWindowTitle(self.tr("Gorgon GUI"))
        pathname = os.path.join(sys.modules[__name__.split('.')[0]].__path__[0],
                                os.pardir,
                                'design',
                                'gorgon.ico')
        self.setWindowIcon(QtGui.QIcon(pathname))
        
    @classmethod
    def set_parser(cls, parser):
        pass

    def load(self):
        if hasattr(self, 'form'):
            self.form.modelLoaded()
        
    def exitApplication(self):
        QtGui.qApp.closeAllWindows()
