from PyQt4 import QtGui, QtCore
from about_form import AboutForm
from gl_widget import GLWidget
from gl_options import GLOptions
from skeleton_viewer import SkeletonViewer;

class WindowManager(QtGui.QWidget):
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.createUI()

    def createUI(self):
        self.createChildWindows()        
        self.createActions()
        self.createMenus()
        
    def createChildWindows(self):
        self.aboutForm = AboutForm(self.app)
        self.glOptions = GLOptions(self.app)
        self.skeletonViewer = SkeletonViewer(self.app)
        self.displayArea = GLWidget([self.skeletonViewer], self.glOptions)   
        self.app.setCentralWidget(self.displayArea)
        
    def createActions(self):
        pass
            
    def createMenus(self):
        pass
