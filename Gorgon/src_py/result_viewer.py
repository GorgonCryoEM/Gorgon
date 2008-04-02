from PyQt4 import QtGui, QtCore
from result_renderer_gl import ResultRendererGL

class ResultViewer(QtGui.QWidget):
    def __init__(self, main, back_end, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.back_end = back_end
        self.app = main
        self.createUI()

    def createUI(self):
        dock = QtGui.QDockWidget(self.tr("Results"), self.app)
        dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea)
        widget = ResultRendererGL(self.app, self.back_end, 1, dock)
        
        dock.setWidget(widget)
        self.app.addDockWidget(QtCore.Qt.LeftDockWidgetArea,dock)
