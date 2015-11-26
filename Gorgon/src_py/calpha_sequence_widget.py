# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   Since a QDockWidget can contain only one widget, this is a composite 
#                widget of a global view of  a chain, a scrollable of its residues, and 
#                a local view that provides chain editing features. 


from PyQt4 import Qt,QtGui,QtCore
from calpha_structure_editor import CAlphaStructureEditor
from calpha_scrollable_sequence_view import CAlphaScrollableSequenceView
from calpha_global_sequence_view import CAlphaGlobalSequenceView


class CAlphaSequenceWidget(QtGui.QWidget):
    def __init__(self, structurePrediction, currentChainModel, dock, parent=None):
        super(CAlphaSequenceWidget, self).__init__(parent)
        self.dock = dock
        self.currentChainModel = currentChainModel
        self.scrollable = CAlphaScrollableSequenceView(structurePrediction, currentChainModel, self)
        self.scrollable.setMinimumSize(300, 180)
        self.structureEditor = CAlphaStructureEditor(currentChainModel, dock, self)
        
        self.globalView=CAlphaGlobalSequenceView(structurePrediction, self)
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
        #self.setMaximumWidth(self.globalView.width())
        
        self.globalView.updateViewportRange()      
        
        
        
