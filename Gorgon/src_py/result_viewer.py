from PyQt4 import QtGui, QtCore
from result_renderer_gl import ResultRendererGL

class ResultViewer(QtGui.QWidget):
    def __init__(self, main, back_end, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.back_end = back_end
        self.app = main
        self.createUI()
        self.createActions()
        self.createMenus()
        
    def createActions(self):   
        self.resultsAct = QtGui.QAction(self.tr("&Results"), self)
        self.resultsAct.setStatusTip(self.tr("Show results widget"))
        self.resultsAct.setCheckable(True)
        self.resultsAct.setChecked(self.resultsDock.isVisible())
        self.connect(self.resultsAct, QtCore.SIGNAL("triggered()"), self.showResultsWidget)
       
        
    def createMenus(self):
        self.app.menuWindow().addAction(self.resultsAct)    
        

    def createUI(self):
        self.resultsDock = QtGui.QDockWidget(self.tr("Results"), self.app)
        self.resultsDock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        widget = ResultRendererGL(self.app, self.back_end, 1, self.resultsDock)
        self.resultsDock.setWidget(widget)
        self.resultsDock.close()
        self.connect(self.resultsDock, QtCore.SIGNAL("visibilityChanged (bool)"), self.resultsDockVisibilityChanged)
 
        
    def showResultsWidget(self):
        if(self.resultsAct.isChecked()) :
            self.app.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.resultsDock)
            self.resultsDock.show()
        else:
            self.app.removeDockWidget(self.resultsDock)
            
    def resultsDockVisibilityChanged(self, visible):
        self.resultsAct.setChecked(visible)
