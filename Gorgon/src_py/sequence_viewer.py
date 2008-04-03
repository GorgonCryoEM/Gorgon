from PyQt4 import QtGui, QtCore
from sequence_renderer_gl import SequenceRendererGL

class SequenceViewer(QtGui.QWidget):
    def __init__(self, main, back_end, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.back_end = back_end
        self.app = main
        self.createUI()
        self.createActions()
        self.createMenus()
        
    def createActions(self):   
        self.sequenceAct = QtGui.QAction(self.tr("&Protein Sequence"), self)
        self.sequenceAct.setStatusTip(self.tr("Show protein sequence widget"))
        self.sequenceAct.setCheckable(True)
        self.sequenceAct.setChecked(self.sequenceDock.isVisible())
        self.connect(self.sequenceAct, QtCore.SIGNAL("triggered()"), self.showSequenceWidget)
       
        
    def createMenus(self):
        self.app.menuWindow().addAction(self.sequenceAct)
        

    def createUI(self):
        sequence = SequenceRendererGL(self.app, self.back_end)
        self.sequenceDock = QtGui.QDockWidget(self.tr("Sequence"), self.app)
        self.sequenceDock.setAllowedAreas(QtCore.Qt.TopDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.sequenceDock.setWidget(sequence)
        self.sequenceDock.close()
        self.connect(self.sequenceDock, QtCore.SIGNAL("visibilityChanged (bool)"), self.sequenceDockVisibilityChanged)
 
        
    def showSequenceWidget(self):
        if(self.sequenceAct.isChecked()) :
            self.app.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.sequenceDock)
            self.sequenceDock.show()
        else:
            self.app.removeDockWidget(self.sequenceDock)
            
    def sequenceDockVisibilityChanged(self, visible):
        self.sequenceAct.setChecked(visible)