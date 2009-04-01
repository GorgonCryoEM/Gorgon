# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   Since a QDockWidget can contain only one widget, this is a composite 
#                widget of a global view of  a chain, a scrollable of its residues, and 
#                a local view that provides chain editing features. 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.1  2009/03/31 21:40:13  ssa1
#   Refactoring: Splitting seq_model\SequenceView.py into subclasses
#

from PyQt4 import Qt,QtGui,QtCore
from calpha_structure_editor import CAlphaStructureEditor
from calpha_scrollable_sequence_view import CAlphaScrollableSequenceView
from calpha_global_sequence_view import CAlphaGlobalSequenceView


class CAlphaSequenceWidget(QtGui.QWidget):
    def __init__(self, structurePrediction, currentChainModel, parent=None):
        super(CAlphaSequenceWidget, self).__init__(parent)
        self.currentChainModel = currentChainModel
        self.scrollable = CAlphaScrollableSequenceView(structurePrediction, currentChainModel, self)
        self.scrollable.setMinimumSize(300, 180)
        self.structureEditor = CAlphaStructureEditor(currentChainModel, self)
        
        self.globalView=CAlphaGlobalSequenceView(structurePrediction, self)
        self.globalView.setLocalView(self.scrollable.seqView)
        self.globalView.updateViewportRange()        

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
        self.setMinimumHeight(400)
        self.setMinimumWidth(self.globalView.width()+10)
        #self.setMaximumWidth(self.globalView.width())