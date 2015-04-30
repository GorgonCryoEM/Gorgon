# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   This QWidget gives residues as one letter abbreviations for residues 
#                and the index below. Most chains will be too big to fit on the screen 
#                on this class. Thus, a CAlphaScrollableSequenceView contains this class.


from PyQt4 import Qt,QtGui,QtCore

class CAlphaSequenceView(QtGui.QWidget):
  def __init__(self, structurePrediction, currentChainModel, parent=None):
    super(CAlphaSequenceView,self).__init__(parent)
    # Initialize font
    self.fontName='Arial'
    self.fontSize=20
    self.font=QtGui.QFont(self.fontName,self.fontSize)
    


    # Initialize sequence
    #self.structurePrediction = structurePrediction
    self.currentChainModel = currentChainModel
    self.setStructurePrediction(structurePrediction)
    

    #self.connect(self, QtCore.SIGNAL('SequencePanelUpdate'), self.repaint)
    self.setWindowModality(QtCore.Qt.NonModal)
    self.setMouseTracking(True) #necessary to listen to mouse with no buttons pressed

    metrics=QtGui.QFontMetrics(self.font)
    self.height = int(3.5*metrics.lineSpacing())
    self.width = int(40*metrics.maxWidth())
    self.resize(self.width, self.height)
    self.setMinimumSize(self.width, self.height)


  def setStructurePrediction(self, newStructurePrediction):
    '''
This function changes what structure prediction is used for sequence
and predicted residue indices of helices and strands.
    '''
    self.structurePrediction = newStructurePrediction
    self.structurePrediction.chain.fillGaps()
    self.updatePanelHeight()
    self.residueRange=self.structurePrediction.chain.residueRange()
    self.connect(self.structurePrediction.chain, QtCore.SIGNAL("selection updated"), self.__selectionUpdated)
    self.connect(self.currentChainModel, QtCore.SIGNAL("selection updated"), self.__selectionUpdated)
    self.repaint()    

  def __selectionUpdated(self):
    self.repaint()

  def __paintIndices(self, painter, metrics, cellWidth, cellHeight):
    x=0
    y=3*cellHeight

    paint=False
    for index in self.residueRange:
      # Thousands place
      if (index+3)%10==0:
        if index+3 >= 1000:
          thousandsPlace=((index+3)/1000)%10
          nextChar=str(thousandsPlace)
          paint=True
      # Hundreds place
      elif (index+2)%10==0:
        if index+2 >= 100:
          hundredsPlace=((index+2)/100)%10
          nextChar=str(hundredsPlace)
          paint=True
      # Tens place
      elif (index+1)%10==0:
        tensPlace= ((index+1)/10)%10
        nextChar=str(tensPlace)
        paint=True
      # Ones place
      elif index%10==0:
        nextChar='0'
        paint=True
      else:
        paint=False

      if paint:
        charWidth=metrics.width(QtCore.QChar(nextChar))
        painter.setPen(QtCore.Qt.gray)
        xOffset=int((cellWidth-charWidth)/2)
        painter.drawText(x+xOffset, y, QtCore.QString(nextChar))
      x = x + cellWidth

  def __paintSecels(self, painter):

    cellWidth=self.cellWidth()
    cellHeight=self.cellHeight()

    x=0
    y=0*cellHeight

    x0=x

    for index in self.residueRange:

      secel=self.structurePrediction.getSecelByIndex(index)
      color=secel.getColor()
      color.setAlpha(255)

      if secel.type=='strand':
        painter.setPen(QtCore.Qt.blue)
        brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)
        # strand end
        if index==secel.stopIndex:
          p0 = QtCore.QPointF(float(x0+0.4*float(cellWidth)),0.15*float(cellHeight))
          p1 = QtCore.QPointF(float(x0+cellWidth),float(cellHeight)/2.0)
          p2 = QtCore.QPointF(float(x0+0.4*float(cellWidth)),0.85*float(cellHeight))
          #painter.drawPolygon(p0,p1,p2,brush=brush)
          path=QtGui.QPainterPath(p0)
          path.lineTo(p1)
          path.lineTo(p2)
          painter.fillPath(path,brush)

          y0=int(cellHeight*.32)
          height=int(cellHeight*.36)
          rect=QtCore.QRectF ( float(x0), float(y0), 0.4*float(cellWidth), float(height))
          painter.fillRect(rect,brush)
        else:
          y0=int(cellHeight*.32)
          height=int(cellHeight*.36)
          rect=QtCore.QRectF ( float(x0), float(y0), float(cellWidth), float(height))
          painter.fillRect(rect,brush)

      elif secel.type=='helix':
        painter.setPen(QtCore.Qt.darkGreen)
        brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)
        # helix end
        if index==secel.stopIndex:
          y0=int(cellHeight*.20)
          height=int(cellHeight*.60)
          rect=QtCore.QRectF ( float(x0), float(y0), float(cellWidth), float(height))
          painter.fillRect(rect,brush)
        # helix start
        #elif index==secel.startIndex: pass
        # helix middle
        else:
          y0=int(cellHeight*.20)
          height=int(cellHeight*.60)
          rect=QtCore.QRectF ( float(x0), float(y0), float(cellWidth), float(height))
          painter.fillRect(rect,brush)

      elif secel.type=='loop':
        painter.setPen(QtCore.Qt.gray)
        brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)
        y0=int(cellHeight*.45)
        height=int(cellHeight*.10)
        rect=QtCore.QRectF ( float(x0), float(y0), float(cellWidth), float(height))
        painter.fillRect(rect,brush)
      x = x + cellWidth
      x0 = x0 + cellWidth

  def __paintResidues(self, painter):
    brush=QtGui.QBrush(QtCore.Qt.black, QtCore.Qt.SolidPattern)
    metrics=QtGui.QFontMetrics(self.font)
    cellWidth=self.cellWidth()
    cellHeight=self.cellHeight()
    y=2*cellHeight

    # initialize loop conditions and iterate for each residue
    x=0
    for index in self.residueRange:

      # residues without coordinates are NOT BOLD and GRAY
      if len(self.currentChainModel[index].getAtomNames())==0:
        self.font.setBold(False)
        painter.setPen(QtCore.Qt.gray)
        painter.setFont(self.font)
        
      # residues which are hidden are NOT BOLD and GRAY
      elif (self.currentChainModel[index].getAtom("CA") and not self.currentChainModel[index].getAtom("CA").getVisible()) :
        self.font.setBold(False)
        painter.setPen(QtCore.Qt.gray)
        painter.setFont(self.font)        

      # residues with coordinates are BOLD and BLACK
      else:
        self.font.setBold(True)
        painter.setPen(QtCore.Qt.black)
        painter.setFont(self.font)

      # selected residues are YELLOW with a BLACK BACKGROUND
      if index in self.structurePrediction.chain.getSelection():
        yOffset= 0.75*(cellHeight-metrics.ascent()) 
        rect=QtCore.QRectF ( float(x), yOffset+float(y), float(cellWidth), float(-cellHeight))
        painter.fillRect(rect,brush)
        painter.setPen(QtCore.Qt.yellow)

      # draw the glyph 
      nextChar=self.structurePrediction.chain[index].__repr__()
      charWidth=metrics.width(QtCore.QChar(nextChar))
      xOffset=int((cellWidth-charWidth)/2)
      painter.drawText(x+xOffset, y, QtCore.QString(nextChar))
      x = x + cellWidth

    # Restore standard properties
    self.font.setBold(False)
    painter.setFont(self.font)

  def mouseReleaseEvent(self, mouseEvent):
    self.updateCursor(mouseEvent)

  def getResidueIndexByMousePosition(self,x,y):
    '''
Given the x, y coordinates of a user mouse click on the CAlphaSequenceView, 
this returns the residue index of that residue.
    '''
    metrics=QtGui.QFontMetrics(self.font)
    cellHeight=metrics.lineSpacing()
    maxCharWidth=QtGui.QFontMetrics(self.font).maxWidth()
    index= int(x/maxCharWidth)
    if y > 1*cellHeight and y < 2*cellHeight:
      return self.residueRange[index]
    else:
      return None

  def mousePressEvent(self, mouseEvent):
    '''
If the user left clicks on a box representing a predicted helix or an 
arrow representing a predicted strand, its residues are selected.  If 
the control key is also pressed, it adds to or removes from the current 
selection.

If the user left clicks on a residue, it becomes the new selection.  If
the control key is also pressed, it is added to or removed from the 
selection.  If the shift key is pressed, a continuous sequence of
residues is selected.
    '''

    if mouseEvent.button() == QtCore.Qt.LeftButton and mouseEvent.y() < self.cellHeight():
      residue=self.getResidueIndexByMousePosition(mouseEvent.x(),self.cellHeight() +1)
      secel = self.structurePrediction.getSecelByIndex(residue)
      self.structurePrediction.setSecelSelection(secel)
      newSelection=range(secel.startIndex, secel.stopIndex+1)
      #  CTRL key pressed
      if mouseEvent.modifiers() & QtCore.Qt.CTRL:
        self.setSequenceSelection(addRange=newSelection)
      else:
        self.setSequenceSelection(newSelection)
      
      self.parentWidget().parentWidget().parentWidget().structureEditor.setResidues(newSelection)
      self.repaint()
      return

    # LEFT MOUSE PRESS
    if mouseEvent.button() == QtCore.Qt.LeftButton and mouseEvent.y() < 2 * self.cellHeight():
      additionalResidue=self.getResidueIndexByMousePosition(mouseEvent.x(),mouseEvent.y())
      self.parentWidget().parentWidget().parentWidget().structureEditor.setResidues([additionalResidue])
      
      #  CTRL key pressed
      if mouseEvent.modifiers() & QtCore.Qt.CTRL:
        if additionalResidue not in self.structurePrediction.chain.getSelection():
          self.setSequenceSelection(addOne=additionalResidue)
        else:
          self.setSequenceSelection(removeOne=additionalResidue)

      #  SHIFT key pressed
      elif mouseEvent.modifiers() & QtCore.Qt.SHIFT:
        additionalResidue=self.getResidueIndexByMousePosition(mouseEvent.x(),mouseEvent.y())
        if additionalResidue > sorted(self.structurePrediction.chain.getSelection())[-1]:
          addedRange=range( 1+sorted(self.structurePrediction.chain.getSelection())[-1],1+additionalResidue)
          self.setSequenceSelection(addRange=addedRange)
        elif additionalResidue < sorted(self.structurePrediction.chain.getSelection())[0]:
          addedRange=range( additionalResidue, sorted(self.structurePrediction.chain.getSelection())[0])
          self.setSequenceSelection(addRange=addedRange)

      #  No key pressed
      elif mouseEvent.modifiers() == QtCore.Qt.NoModifier:
        self.setSequenceSelection(newSelection=[additionalResidue])
      
    if mouseEvent.buttons() & QtCore.Qt.MidButton:
      self.setCursor(QtCore.Qt.ClosedHandCursor)
      self.mouseXInitial=mouseEvent.globalX()
      self.scrollbarInitialX=self.scrollbar.value()

    self.repaint()

  def updateCursor(self, mouseEvent):
    metrics=QtGui.QFontMetrics(self.font)
    # cursor is Arrow when positioned over secel row or index row
    if mouseEvent.y() > metrics.lineSpacing() and mouseEvent.y() < 2*metrics.lineSpacing():
      self.setCursor(QtCore.Qt.IBeamCursor)
    # cursor is TextSelector when positioned over residue row
    else:
      self.setCursor(QtCore.Qt.ArrowCursor)

  def mouseMoveEvent(self, mouseEvent):
    if mouseEvent.buttons() & QtCore.Qt.LeftButton:
      addedResidue=self.getResidueIndexByMousePosition(mouseEvent.x(), mouseEvent.y())
      self.setSequenceSelection(addOne=addedResidue)
      self.repaint()

    if mouseEvent.buttons() & QtCore.Qt.MidButton:
      dx=mouseEvent.globalX()-self.mouseXInitial
      self.scrollbar.setValue(self.scrollbarInitialX-dx)
    elif mouseEvent.buttons() == QtCore.Qt.NoButton:
      self.updateCursor(mouseEvent)

  def incrementPointSize(self):
    '''
This increases font size.
    '''
    self.setFontSize(self.fontSize + 2)

  def decrementPointSize(self):
    '''
This decreases font size.
    '''
    self.setFontSize(self.fontSize - 2)

  def updatePanelHeight(self):
    '''
This updates the height and width of the CAlphaSequenceView.
    '''
    metrics=QtGui.QFontMetrics(self.font)
    height=int(3.5*metrics.lineSpacing())
    width=metrics.maxWidth()*len(self.structurePrediction.chain.residueRange())
    self.resize(QtCore.QSize(width,height))


  def setSequenceSelection(self, newSelection=None, removeOne=None, addOne=None, addRange=None):
    '''
This changes what residues are selected in both the structure 
prediction and the chain model by replacing the selection, removing a 
residue from the selection, adding a residue to the selection, or 
adding a list of residues to the selection.
    '''
    dock = self.parentWidget().parentWidget().parentWidget().parentWidget()
    viewer = dock.viewer
    renderer = viewer.renderer
    app = dock.app
    
    selectionToClear = self.currentChainModel.getSelection()
    for i in selectionToClear:
        try: 
            self.structurePrediction.chain[i]
        except KeyError: 
            continue
        atom = self.currentChainModel[i].getAtom('CA')
        if atom:
            atom.setSelected(False)
    self.structurePrediction.chain.setSelection(newSelection,removeOne,addOne,addRange)
    self.currentChainModel.setSelection(newSelection,removeOne,addOne,addRange)
    
    for i in self.currentChainModel.getSelection():
        try:
            selectedAtom = self.currentChainModel[ i ].getAtom('CA')
        except KeyError:
            continue
        if not selectedAtom:
            continue    
        selectedAtom.setSelected(True)
        
    viewer.centerOnSelectedAtoms()    
    viewer.emitModelChanged()
    
  def updateSequenceSelection(self):
    '''
Based on the selection in the current chain model, this updates the 
selection in the structure prediction and displays the changes in the
CAlphaSequenceView.
    '''
    selection = self.currentChainModel.getSelection()
    self.structurePrediction.chain.setSelection(selection)
    dock = self.parentWidget().parentWidget().parentWidget().parentWidget()
    dock.viewer.emitModelChanged()
    
  def setFont(self, newFont):
    self.fontName=newFont
    self.font=QtGui.QFont(self.fontName,self.fontSize)
    self.fontMetrics=QtGui.QFontMetrics(self.font)
    self.emit( QtCore.SIGNAL('SequencePanelUpdate'))

  def setFontSize(self, newFontSize):
    oldValue=self.scrollbar.value()
    oldCellWidth=QtGui.QFontMetrics(self.font).maxWidth()

    self.fontSize=newFontSize
    self.font=QtGui.QFont(self.fontName,self.fontSize)
    self.fontMetrics=QtGui.QFontMetrics(self.font)  #How do I do this correctly so I don't duplicate code? 
    self.updatePanelHeight()

    newValue=self.scrollbar.value()
    newCellWidth=QtGui.QFontMetrics(self.font).maxWidth()
    ratio=float(newCellWidth/oldCellWidth)

    self.emit( QtCore.SIGNAL('SequencePanelUpdate'))
    self.scrollbar.setValue(int(oldValue*ratio))

  def height(self):
    return 4*self.cellHeight()

  def cellHeight(self):
    metrics=QtGui.QFontMetrics(self.font)
    cellHeight=metrics.lineSpacing()
    return cellHeight

  def cellWidth(self):
    metrics=QtGui.QFontMetrics(self.font)
    cellWidth=metrics.maxWidth()
    return cellWidth

  def paintEvent(self, event):
    painter=QtGui.QPainter(self)
    painter.setFont(self.font)

    # Establish widget geometry
    metrics=QtGui.QFontMetrics(self.font)
    cellWidth=self.cellWidth()
    cellHeight=self.cellHeight()

    self.__paintSecels(painter)
    self.__paintResidues(painter)
    self.__paintIndices(painter, metrics, cellWidth, cellHeight)


  def calculateViewportRange(self, j):
    metrics=QtGui.QFontMetrics(self.font)
    cellWidth=metrics.maxWidth()
    viewportStart=self.scrollbar.value()/cellWidth
    viewportWidth=self.parentWidget().parentWidget().width()/cellWidth 
    #self.parentWidget() => QWidget object, self.parentWidget().parentWidget() => CAlphaScrollableSequenceView object
    try: 
        start=self.residueRange[viewportStart]
    except:
        start = 1
    return range(start,start+viewportWidth+1)
