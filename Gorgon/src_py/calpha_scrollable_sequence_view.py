# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:    This QWidget contains a CAlphaSequenceView object but in a scrollable view.

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/calpha_scrollable_sequence_view.py,v $
#   $Revision: 1.1 $
#   $Date: 2009/03/31 21:40:13 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: calpha_scrollable_sequence_view.py,v $
#   Revision 1.1  2009/03/31 21:40:13  ssa1
#   Refactoring: Splitting seq_model\SequenceView.py into subclasses
#

from PyQt4 import Qt,QtGui,QtCore
from calpha_sequence_view import CAlphaSequenceView

class CAlphaScrollableSequenceView(QtGui.QScrollArea):
  def __init__(self,structurePrediction, currentChainModel, parent=None):
    super(CAlphaScrollableSequenceView,self).__init__()
    self.seqView=CAlphaSequenceView(structurePrediction, currentChainModel,parent=self)
    seqView = self.seqView
    seqView.updatePanelHeight() #This is needed to get all residues to show up in this widget.
    #Note: updatePanelHeight also adjusts width - I'm guessing that is the part that fixes things.
    seqView.scrollbar=self.horizontalScrollBar()
    self.seqView=seqView
    self.setWidget(self.seqView)
    
    self.setWidgetResizable(False)
    self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
    self.setWindowTitle(QtCore.QString('Scrollable Local View'))
    self.updateHeight()

  def updateHeight(self):
    """
This resizes the CAlphaScrollableSequenceView's height based on the size of 
the CAlphaSequenceView.
    """
    scrollbar=self.horizontalScrollBar()
    scrollbarHeight=scrollbar.height()
    widgetHeight=self.widget().height#()
    #self.resize(QtCore.QSize(self.width(), scrollbarHeight+widgetHeight))
    self.resize(QtCore.QSize(self.width(), widgetHeight))

  def getSeqview(self):
    return self.seqView