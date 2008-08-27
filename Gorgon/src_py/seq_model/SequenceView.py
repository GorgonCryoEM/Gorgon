#!/usr/bin/python
import sys
from PyQt4 import Qt,QtGui,QtCore

from Helix import Helix
from Strand import Strand
from Chain import Chain
from Residue import Residue
import random

def fillGaps(inSequence):
  for i in inSequence.residueRange():
    while i+1 not in inSequence.residueRange():
      if i+1>inSequence.residueRange()[-1]:
	break
      inSequence[i+1]=Residue('X')
      i=i+1

class SequenceView(QtGui.QWidget):
  def __init__(self,sequence,parent=None):
    super(SequenceView,self).__init__(parent)

    self.parent=parent

    # Initialize font
    self.fontName='Arial'
    self.fontSize=30
    self.font=QtGui.QFont(self.fontName,self.fontSize)

    # Initialize sequence
    self.sequence = sequence
    fillGaps(self.sequence)
    self.residueRange=self.sequence.residueRange()

    # Initialize residue selection
    self.selectedResidues=[]

    # Set up QWidget Behavior
    #self.connect(self, QtCore.SIGNAL('SequencePanelUpdate'), self.repaint)
    self.setWindowModality(QtCore.Qt.NonModal)
    self.setMouseTracking(True) #necessary to listen to mouse with no buttons pressed

    metrics=QtGui.QFontMetrics(self.font)
    self.cellWidth=metrics.maxWidth()
    height=int(3.5*metrics.lineSpacing())
    self.resize(int(40*metrics.maxWidth()),height)


  def __paintIndices(self, painter, metrics, cellWidth, cellHeight):
    x=0
    y=3*cellHeight

    paint=False
    for index in self.residueRange:
      # Hundreds place
      if (index+2)%10==0:
	if index+2 > 100:
	  hundredsPlace=((index+2)/100)%100
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

  def __paintSecels(self, painter, cellWidth, cellHeight):

    x=0
    y=0*cellHeight

    x0=x

    for index in self.residueRange:

      secel=self.sequence.getSecelByIndex(index)
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




  def __paintResidues(self, painter, metrics, cellWidth, cellHeight):
    brush=QtGui.QBrush(QtCore.Qt.black, QtCore.Qt.SolidPattern)


    x=0
    y=2*cellHeight

    for index in self.residueRange:
      if len(sequence[index].atoms)==0:
	self.font.setBold(False)
	painter.setFont(self.font)
      else:
	self.font.setBold(True)
	painter.setFont(self.font)

      nextChar=self.sequence[index].__repr__()
      charWidth=metrics.width(QtCore.QChar(nextChar))
      if index in self.selectedResidues:
	yOffset= 0.75*(cellHeight-metrics.ascent()) 
	rect=QtCore.QRectF ( float(x), yOffset+float(y), float(cellWidth), float(-cellHeight))
	painter.fillRect(rect,brush)
	painter.setPen(QtCore.Qt.yellow)
      else:
	painter.setPen(QtCore.Qt.black)
      xOffset=int((cellWidth-charWidth)/2)
      painter.drawText(x+xOffset, y, QtCore.QString(nextChar))
      x = x + cellWidth

    self.font.setBold(False)
    painter.setFont(self.font)


  def mouseReleaseEvent(self, mouseEvent):
    self.updateCursor(mouseEvent)

  def getResidueIndexByMousePosition(self,x,y):
    metrics=QtGui.QFontMetrics(self.font)
    cellHeight=metrics.lineSpacing()
    maxCharWidth=QtGui.QFontMetrics(self.font).maxWidth()
    index= int(x/maxCharWidth)
    if y > 1*cellHeight and y < 2*cellHeight:
      return self.residueRange[index]
    else:
      print y/float(cellHeight)
      return None

  def mousePressEvent(self, mouseEvent):

    # LEFT MOUSE PRESS
    if mouseEvent.button() == QtCore.Qt.LeftButton:
      additionalResidue=self.getResidueIndexByMousePosition(mouseEvent.x(),mouseEvent.y())

      #  CTRL key pressed
      if mouseEvent.modifiers() & QtCore.Qt.CTRL:
	if additionalResidue not in self.selectedResidues:
	  self.updateSelectedResidues(addOne=additionalResidue)
	else:
	  self.updateSelectedResidues(removeOne=additionalResidue)
	self.repaint()

      #  SHIFT key pressed
      elif mouseEvent.modifiers() & QtCore.Qt.SHIFT:
	additionalResidue=self.getResidueIndexByMousePosition(mouseEvent.x(),mouseEvent.y())
	if additionalResidue > sorted(self.selectedResidues)[-1]:
	  addedRange=range( 1+sorted(self.selectedResidues)[-1],1+additionalResidue)
	  self.updateSelectedResidues(addRange=addedRange)
	elif additionalResidue < sorted(self.selectedResidues)[0]:
	  addedRange=range( additionalResidue, sorted(self.selectedResidues)[0])
	  self.updateSelectedResidues(addRange=addedRange)
	self.repaint()

      #  No key pressed
      elif mouseEvent.modifiers() == QtCore.Qt.NoModifier:
	self.updateSelectedResidues(newSelection=[additionalResidue])
      
    if mouseEvent.buttons() & QtCore.Qt.MidButton:
      self.setCursor(QtCore.Qt.ClosedHandCursor)
      self.mouseXInitial=mouseEvent.globalX()
      self.scrollbarInitialX=self.scrollbar.value()

  def updateCursor(self, mouseEvent):
    metrics=QtGui.QFontMetrics(self.font)
    if mouseEvent.y() > metrics.lineSpacing() and mouseEvent.y() < 2*metrics.lineSpacing():
      self.setCursor(QtCore.Qt.IBeamCursor)
    else:
      self.setCursor(QtCore.Qt.ArrowCursor)

  def mouseMoveEvent(self, mouseEvent):
    if mouseEvent.buttons() & QtCore.Qt.LeftButton:
      addedResidue=self.getResidueIndexByMousePosition(mouseEvent.x(), mouseEvent.y())
      self.updateSelectedResidues(addOne=addedResidue)
      self.repaint()

    if mouseEvent.buttons() & QtCore.Qt.MidButton:
      dx=mouseEvent.globalX()-self.mouseXInitial
      self.scrollbar.setValue(self.scrollbarInitialX-dx)
    elif mouseEvent.buttons() == QtCore.Qt.NoButton:
      self.updateCursor(mouseEvent)

  def incrementPointSize(self):
    self.setFontSize(self.fontSize + 2)

  def decrementPointSize(self):
    self.setFontSize(self.fontSize - 2)

  def updatePanelHeight(self):
    metrics=QtGui.QFontMetrics(self.font)
    height=int(3.5*metrics.lineSpacing())
    width=metrics.maxWidth()*len(self.sequence.residueRange())
    self.resize(QtCore.QSize(width,height))


  def updateSelectedResidues(self, newSelection=None, removeOne=None, addOne=None, addRange=None):
    if newSelection is not None:
      self.selectedResidues=newSelection

    elif removeOne is not None:
      if removeOne in self.selectedResidues:
	self.selectedResidues.remove(removeOne)

    elif addOne is not None:
      if addOne not in self.selectedResidues:
	self.selectedResidues.append(addOne)

    elif addRange is not None:
      self.selectedResidues.extend(addRange)

    self.emit( QtCore.SIGNAL('SequencePanelUpdate'))


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
    self.cellWidth=self.fontMetrics.maxWidth()
    self.updatePanelHeight()

    newValue=self.scrollbar.value()
    newCellWidth=QtGui.QFontMetrics(self.font).maxWidth()
    ratio=float(newCellWidth/oldCellWidth)

    self.emit( QtCore.SIGNAL('SequencePanelUpdate'))
    self.scrollbar.setValue(int(oldValue*ratio))

  def height(self):
    metrics=QtGui.QFontMetrics(self.font)
    cellHeight=4*metrics.lineSpacing()
    return cellHeight

  def paintEvent(self, event):
    painter=QtGui.QPainter(self)
    painter.setFont(self.font)

    # Establish widget geometry
    metrics=QtGui.QFontMetrics(self.font)
    cellWidth=metrics.maxWidth()
    cellHeight=metrics.lineSpacing()

    self.__paintSecels(painter, cellWidth, cellHeight)
    self.__paintResidues(painter, metrics, cellWidth, cellHeight)
    self.__paintIndices(painter, metrics, cellWidth, cellHeight)


  def calculateViewportRange(self, j):
    metrics=QtGui.QFontMetrics(self.font)
    cellWidth=metrics.maxWidth()
    viewportStart=self.scrollbar.value()/cellWidth
    viewportWidth=self.parent.width()/cellWidth
    start=self.residueRange[viewportStart]
    return range(start,start+viewportWidth+1)

class ScrollableSequenceView(QtGui.QScrollArea):
  def __init__(self,sequence):
    super(ScrollableSequenceView,self).__init__()

    seqView=SequenceView(sequence,parent=self)
    seqView.scrollbar=self.horizontalScrollBar()
    self.seqView=seqView
    self.setWidget(self.seqView)

    globalView=GlobalSequenceView(sequence)
    globalView.setLocalView(seqView)
    globalView.updateViewportRange()
    globalView.show()
    globalView.raise_()

    self.connect(seqView.scrollbar, QtCore.SIGNAL('actionTriggered(int)'), globalView.updateViewportRange)
    self.connect(seqView.scrollbar, QtCore.SIGNAL('valueChanged(int)'), globalView.updateViewportRange)
    #self.connect(seqView.scrollbar, QtCore.SIGNAL('rangeChanged(int)'), globalView.updateViewportRange)
    self.connect(seqView, QtCore.SIGNAL('SequencePanelUpdate'), globalView.updateViewportRange)

    self.setWidgetResizable(False)
    self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
    self.setWindowTitle(QtCore.QString('Local View'))

  def updateHeight(self):
    scrollbar=self.horizontalScrollBar()
    scrollbarHeight=scrollbar.height()
    widgetHeight=self.widget().height()
    #self.resize(QtCore.QSize(self.width(), scrollbarHeight+widgetHeight))
    self.resize(QtCore.QSize(self.width(), widgetHeight))

  def getSeqview(self):
    return self.seqView

class GlobalSequenceView(QtGui.QWidget):
  def __init__(self,sequence,parent=None):
    super(GlobalSequenceView,self).__init__(parent)

    self.sequence=sequence
    seqLength=len(self.sequence.residueRange())

    self.nCols=100
    self.nRows=1+seqLength/self.nCols

    widgetWidth=600
    self.cellHeight=40
    self.cellWidth=widgetWidth/self.nCols

    # Set up QWidget Behavior
    self.connect(self, QtCore.SIGNAL('SequencePanelUpdate'), self.repaint)
    self.setWindowModality(QtCore.Qt.NonModal)
    self.resize(QtCore.QSize(widgetWidth,self.nRows*self.cellHeight))

    self.setWindowTitle(QtCore.QString('Global View'))

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
    windowOffset = - 0.5 * self.localView.cellWidth * len(viewportRange)
    self.localView.scrollbar.setValue(windowOffset + position*self.localView.cellWidth)

  def mouseReleaseEvent(self, mouseEvent):
    self.setCursor(QtCore.Qt.ArrowCursor)

  def mousePressEvent(self, mouseEvent):
    self.__updatePosition(mouseEvent.x(),mouseEvent.y())

  def paintEvent(self, event):
    painter=QtGui.QPainter(self)
    self.__paintSecels(painter, self.cellWidth, self.cellHeight/2)

  def __paintSecels(self, painter, cellWidth, cellHeight):

    indexRange=self.sequence.residueRange()
    markerRange=range( len(indexRange) )
    for index,marker in zip(indexRange,markerRange):

      if len(sequence[index].atoms)==0:
	alpha=100
      elif len(sequence[index].atoms)==1:
	alpha=200
      else:
	alpha=255

      xOffset=(marker%self.nCols)*self.cellWidth
      yOffset=0.5*cellHeight+(marker/(self.nCols))*self.cellHeight

      secel=self.sequence.getSecelByIndex(index)
      color=secel.getColor()

      if index in self.viewportRange:
	viewportColor=QtGui.QColor(0,0,0)
	brush=QtGui.QBrush(QtCore.Qt.gray, QtCore.Qt.SolidPattern)
	brush=QtGui.QBrush(viewportColor, QtCore.Qt.SolidPattern)
	rect=QtCore.QRectF ( float(xOffset), float(yOffset)-0.5*cellHeight, float(cellWidth), 2.0*float(cellHeight))
	painter.fillRect(rect,brush)


      if secel.type=='strand':
	color.setAlpha(alpha)
	brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)
	# strand end
	if index==secel.stopIndex:
	  p0 = QtCore.QPointF(float(xOffset+0.4*float(cellWidth)),yOffset+0.15*float(cellHeight))
	  p1 = QtCore.QPointF(float(xOffset+cellWidth),yOffset+float(cellHeight)/2.0)
	  p2 = QtCore.QPointF(float(xOffset+0.4*float(cellWidth)),yOffset+0.85*float(cellHeight))
	  #painter.drawPolygon(p0,p1,p2,brush=brush)
	  path=QtGui.QPainterPath(p0)
	  path.lineTo(p1)
	  path.lineTo(p2)
	  painter.fillPath(path,brush)

	  y0=int(cellHeight*.32)
	  height=int(cellHeight*.36)
	  rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), 0.4*float(cellWidth), float(height))
	  painter.fillRect(rect,brush)
	else:
	  y0=int(cellHeight*.32)
	  height=int(cellHeight*.36)
	  rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), float(cellWidth), float(height))
	  painter.fillRect(rect,brush)

      elif secel.type=='helix':
	color.setAlpha(alpha)
	brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)
	# helix end
	if index==secel.stopIndex:
	  y0=int(cellHeight*.20)
	  height=int(cellHeight*.60)
	  rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), float(cellWidth), float(height))
	  painter.fillRect(rect,brush)
	# helix start
	#elif index==secel.startIndex: pass
	# helix middle
	else:
	  y0=int(cellHeight*.20)
	  height=int(cellHeight*.60)
	  rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), float(cellWidth), float(height))
	  painter.fillRect(rect,brush)

      elif secel.type=='loop':
	#painter.setPen(QtCore.Qt.gray)
	if index in self.viewportRange:
	  (r,g,b,a)=color.getRgb()
	  color=QtGui.QColor(255-r,255-g,255-b)
	#else:
	  #color=QtGui.QColor(0,0,0)

	color.setAlpha(alpha)
	brush=QtGui.QBrush(color, QtCore.Qt.SolidPattern)

	y0=int(cellHeight*.45)
	height=int(cellHeight*.10)
	rect=QtCore.QRectF ( float(xOffset), yOffset+float(y0), float(cellWidth), float(height))
	painter.fillRect(rect,brush)
      xOffset = xOffset + cellWidth

def makeToolbar():
  toolbar=QtGui.QToolBar()
  toolbar.setToolButtonStyle(QtCore.Qt.ToolButtonIconOnly)


  zoomInIcon=QtGui.QIcon('zoom-in.png')
  zoomInAction=QtGui.QAction(zoomInIcon,'LargerFont',None)
  toolbar.addAction(zoomInAction)

  zoomOutIcon=QtGui.QIcon('zoom-out.png')
  zoomOutAction=QtGui.QAction(zoomOutIcon,'LargerFont',None)
  toolbar.addAction(zoomOutAction)

  return toolbar


def tempZoomDialog(seqView, scrollArea):
  plusButton=QtGui.QPushButton('+')
  plusButton.setMaximumWidth(50)
  plusButton.connect(plusButton, QtCore.SIGNAL("clicked()"), seqView.incrementPointSize)
  plusButton.connect(plusButton, QtCore.SIGNAL("clicked()"), scrollArea.updateHeight)

  minusButton=QtGui.QPushButton('-')
  minusButton.setMaximumWidth(50)
  minusButton.connect(minusButton, QtCore.SIGNAL("clicked()"), seqView.decrementPointSize)
  minusButton.connect(minusButton, QtCore.SIGNAL("clicked()"), scrollArea.updateHeight)

  layout=QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight)
  layout.addWidget(plusButton)
  layout.addWidget(minusButton)

  dialog=QtGui.QDialog()
  dialog.resize(QtCore.QSize(150,40))
  dialog.setModal(False)
  dialog.setLayout(layout)
  dialog.setWindowTitle(QtCore.QString('Zoom Dialog'))
  #dialog.show() ;dialog.raise_()

  return dialog

app = QtGui.QApplication(sys.argv)

sequence = Chain.load('1KPO.pdb')

lastRandom = random.gauss (100.0, 1.0)
lastCleared=True
for residue in sequence.residueRange():
  nextRandom = random.gauss(lastRandom, 1.0)

  #invert
  if abs(nextRandom - lastRandom) > 1.8:

    if lastCleared==True:
      lastCleared=False
    else:
      sequence[residue].clearAtoms()
      lastCleared=True

  #don't invert
  else:

    if lastCleared==True:
      sequence[residue].clearAtoms()



seqView=ScrollableSequenceView(sequence)
seqView.show()
seqView.updateHeight()
seqView.raise_()

dialog=tempZoomDialog(seqView.getSeqview(),seqView)
dialog.show()
dialog.raise_()

sys.exit(app.exec_())
