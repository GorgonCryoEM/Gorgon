from PyQt4 import Qt, QtGui, QtCore
from .structure_editor.structure_editor import CAlphaStructureEditor
from .scrollable_view import CAlphaScrollableSequenceView
from .global_view import CAlphaGlobalSequenceView


class CAlphaSequenceWidget(QtGui.QWidget):

    def __init__(self, structurePrediction, currentChainModel, dock, parent=None):
        super(CAlphaSequenceWidget, self).__init__(parent)
        self.dock = dock
        self.currentChainModel = currentChainModel
        self.scrollable = CAlphaScrollableSequenceView(structurePrediction, currentChainModel, self)
        self.scrollable.setMinimumSize(300, 180)
        self.structureEditor = CAlphaStructureEditor(currentChainModel, dock, self)
        
        self.globalView = CAlphaGlobalSequenceView(structurePrediction, self)
        self.globalView.setLocalView(self.scrollable.seqView)

        self.connect(self.scrollable.seqView.scrollbar, QtCore.SIGNAL('actionTriggered(int)'), self.globalView.updateViewportRange)
        self.connect(self.scrollable.seqView.scrollbar, QtCore.SIGNAL('valueChanged(int)'), self.globalView.updateViewportRange)
        #self.connect(self.scrollable.seqView.scrollbar, QtCore.SIGNAL('rangeChanged(int)'), self.globalView.updateViewportRange)
        self.connect(self.scrollable.seqView, QtCore.SIGNAL('SequencePanelUpdate'), self.globalView.updateViewportRange)
        
        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.globalView)
        layout.addWidget(self.scrollable)
        layout.addWidget(self.structureEditor)
        layout.addStretch()
        self.setLayout(layout)
        self.setWindowTitle('Sequence Widget')
        self.setMinimumHeight(600)
        self.setMinimumWidth(310)
        # self.setMaximumWidth(self.globalView.width())
        
        self.globalView.updateViewportRange()
