# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   This QWidget shows a pictographic representation of a chain, with 
#                blocks for helices and arrows for strands. It contains a CAlphaSequenceView
#                QWidget Object, and updates the CAlphaSequenceView to show the residues 
#                corresponding to the selection on it, and vice versa.



from PyQt4 import Qt,QtGui,QtCore

class CAlphaGlobalSequenceView(QtGui.QWidget):
  def __init__(self, structurePrediction, parent=None):
    super(CAlphaGlobalSequenceView, self).__init__(parent)

    # Set up Geometry
    self.nCols= min(200, len(structurePrediction.chain.residueRange()))
    self.widgetWidth = self.width()
    self.cellHeight= 40
    self.cellWidth=float(self.widgetWidth)/float(self.nCols)

    self.setStructurePrediction(structurePrediction)    

    # Set up QWidget Behavior
    self.connect(self, QtCore.SIGNAL('SequencePanelUpdate'), self.repaint)
    self.setWindowModality(QtCore.Qt.NonModal)
    self.setWindowTitle(QtCore.QString('Global View'))
    
  def setStructurePrediction(self, newStructurePrediction):
    """
This sets the structure prediction used by the CAlphaGlobalSequenceView, and
updates its size based on the length of the sequence.
    """
    self.structurePrediction = newStructurePrediction
    self.connect(self.structurePrediction.chain, QtCore.SIGNAL("selection updated"), self.__selectionUpdated)
    seqLength=len(self.structurePrediction.chain.residueRange())
    self.nRows=1+seqLength/self.nCols
    self.resize(QtCore.QSize(self.widgetWidth,self.nRows*self.cellHeight))
    self.setMinimumSize(QtCore.QSize(self.widgetWidth,self.nRows*self.cellHeight)) #Neccessary when adding as a widget.
    self.repaint()


  def __selectionUpdated(self):
    self.repaint()

  def updateViewportRange(self):
    self.viewportRange=self.localView.calculateViewportRange(0)
    self.repaint()

  def setLocalView(self, seqView):
    self.localView=seqView

  def mouseMoveEvent(self, mouseEvent):
    self.setCursor(QtCore.Qt.ClosedHandCursor)
    self.__updatePosition(mouseEvent.x(),mouseEvent.y())

  def __updatePosition(self,x,y):
    position=(y/self.cellHeight)*self.nCols + (x/self.cellWidth)

    viewportRange=self.localView.calculateViewportRange(0)
    windowOffset = - 0.5 * self.localView.cellWidth() * len(viewportRange)
    self.localView.scrollbar.setValue(windowOffset + position*self.localView.cellWidth())

  def mouseReleaseEvent(self, mouseEvent):
    self.setCursor(QtCore.Qt.ArrowCursor)

  def mousePressEvent(self, mouseEvent):
    self.__updatePosition(mouseEvent.x(),mouseEvent.y())

  def paintEvent(self, event):
    painter=QtGui.QPainter(self)
    self.widgetWidth = self.width()
    self.cellWidth=float(self.widgetWidth)/float(self.nCols)     
    self.__paintSecels(painter, self.cellWidth, self.cellHeight/2)

  def __paintSecels(self, painter, cellWidth, cellHeight):     
    indexRange=self.structurePrediction.chain.residueRange()
    markerRange=range( len(indexRange) )
    for index,marker in zip(indexRange,markerRange):

      if len(self.structurePrediction.chain[index].getAtomNames())==0:
        alpha=100
      elif len(self.structurePrediction.chain[index].getAtomNames())==1:
        alpha=200
      else:
        alpha=255

      xOffset=(marker%self.nCols)*self.cellWidth
      yOffset=0.5*cellHeight+(marker/(self.nCols))*self.cellHeight

      secel=self.structurePrediction.getSecelByIndex(index)
      color=secel.getColor()

      # Paint Selection Blocks
      if index in self.localView.structurePrediction.chain.getSelection():
        viewportColor=QtCore.Qt.yellow
        brush=QtGui.QBrush(viewportColor, QtCore.Qt.SolidPattern)
        rect=QtCore.QRectF ( float(xOffset), float(yOffset)-0.25*cellHeight, float(cellWidth), 1.5*float(cellHeight))
        painter.fillRect(rect,brush)

      # Paint Strand
      if secel.type=='strand':
        color.setAlpha(alpha)
        brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)

        # strand end (triangle)
        if index==secel.stopIndex:
          p0 = QtCore.QPointF(float(xOffset+0.4*float(cellWidth)),yOffset+0.15*float(cellHeight))
          p1 = QtCore.QPointF(float(xOffset+cellWidth),yOffset+float(cellHeight)/2.0)
          p2 = QtCore.QPointF(float(xOffset+0.4*float(cellWidth)),yOffset+0.85*float(cellHeight))
          path=QtGui.QPainterPath(p0)
          path.lineTo(p1)
          path.lineTo(p2)
          painter.fillPath(path,brush)

          y0=int(cellHeight*.32)
          height=int(cellHeight*.36)
          rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), 0.4*float(cellWidth), float(height))
          painter.fillRect(rect,brush)
        # strand main (wide bar)
        else:
          y0=int(cellHeight*.32)
          height=int(cellHeight*.36)
          rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), float(cellWidth), float(height))
          painter.fillRect(rect,brush)

      # Paint Helix
      elif secel.type=='helix':
        color.setAlpha(alpha)
        brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)
        y0=int(cellHeight*.20)
        height=int(cellHeight*.60)
        rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), float(cellWidth), float(height))
        painter.fillRect(rect,brush)

      # Paint Coil
      elif secel.type=='loop':
        if index in self.viewportRange:
          (r,g,b,a)=color.getRgb()
          color=QtGui.QColor(255-r,255-g,255-b)

        #color.setAlpha(alpha)
        brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)

        y0=int(cellHeight*.45)
        height=int(cellHeight*.10)
        rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), float(cellWidth), float(height))
        painter.fillRect(rect,brush)

      # Paint Viewport
      if index in self.viewportRange:
        viewportColor=QtCore.Qt.gray
        viewportColor=QtGui.QColor(40,40,40)
        viewportColor.setAlpha(100)
        brush=QtGui.QBrush(viewportColor, QtCore.Qt.SolidPattern)
        rect=QtCore.QRectF ( float(xOffset), float(yOffset)-0.5*cellHeight, float(cellWidth), 2.0*float(cellHeight))
        painter.fillRect(rect,brush)

      xOffset = xOffset + cellWidth
