from PyQt4 import QtGui, QtCore
from gorgon_cpp_wrapper import BackEndInterface
from sequence_viewer import SequenceViewer
from skeleton_renderer import SkeletonRenderer
from result_viewer import ResultViewer
from iso_surface import IsoSurface
from gl_options import GLOptions
from gl_widget import GLWidget


class TopologyHunter(QtGui.QWidget):
    """Class for TopologyHunter Module"""
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.back_end = BackEndInterface()       
        self.skeleton = SkeletonRenderer(self.app, self.back_end)
        
        # Load MRC visualization module
        self.mrc_engine = IsoSurface(main)
        self.gl_options = GLOptions(main)
        #self.gl_main = GLWidget(self.mrc_engine, self.gl_options)
        self.gl_main = GLWidget([self.skeleton, self.mrc_engine], self.gl_options)
        #self.gl_main.connect(self.mrc_engine, QtCore.SIGNAL("meshChanged()"), self.gl_main.updateGL)
        
        # create UI and add its components to the MainWindow
        self.createUI()

    def createUI(self):
        self.createChildWindows()        
        self.createActions()
        self.createMenus()

        
    def createChildWindows(self):
        self.app.setCentralWidget(self.gl_main)
        self.results = ResultViewer(self.app, self.back_end)
        self.sequence = SequenceViewer(self.app, self.back_end)
        
    def createActions(self):
        self.executeAct = QtGui.QAction(self.tr("&Execute"), self)
        self.executeAct.setStatusTip(self.tr("Execute Topology Hunter"))
        self.connect(self.executeAct, QtCore.SIGNAL("triggered()"), self.execute)

        self.openAct = QtGui.QAction(self.tr("&Open Data..."), self)
        self.openAct.setShortcut(self.tr("Ctrl+D"))
        self.openAct.setStatusTip(self.tr("Open an existing data file"))
        self.connect(self.openAct, QtCore.SIGNAL("triggered()"), self.openDialog)
            
    def createMenus(self):
        self.app.menuOpen().addAction(self.openAct)
        
        self.thMenu = self.app.menuBar().addMenu(self.tr("Topology Hunter"))
        self.thMenu.addAction(self.executeAct)       

    def openDialog(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self)
        if not fileName.isEmpty():
            self.load(fileName)

    def load(self, fileName):
        self.back_end.SetConstantsFromFile(str(fileName))
        self.back_end.LoadSequenceGraph()
        self.back_end.LoadSkeletonGraph()

        self.sequence.updateGL()
        
    def execute(self):
        self.back_end.ExecuteQuery()
    
