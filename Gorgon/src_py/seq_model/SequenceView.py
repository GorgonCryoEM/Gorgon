#!/usr/bin/python
import sys
from PyQt4 import Qt,QtGui,QtCore

from seq_model.Helix import Helix
from seq_model.Strand import Strand
from seq_model.Chain import Chain
#from seq_model.Residue import Residue
from ui_threeResidues import Ui_threeResidues
from libpyGORGON import CAlphaRenderer, PDBAtom

class SequenceDock(QtGui.QDockWidget):
    __dock = None
    
    def __init__(self, main, viewer, chainObj, parent=None):
        super(SequenceDock, self).__init__(parent)
        self.app = main
        self.viewer=viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.seqWidget = SequenceWidget(chainObj, self)
        self.setWidget(self.seqWidget)
        self.createActions()
        SequenceDock.__dock = self
        if main:
            self.connect(self.app.viewers["calpha"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.updateFromViewerSelection)    
    @classmethod
    def changeDockVisibility(cls, main, viewer):
        #### To do: hide the dock if unchecked & handle close button on the dock correctly
        chainObj = Chain.getChain( Chain.getSelectedChainKey() )
        if not chainObj: chainObj = Chain('', main)
        if cls.__dock:
            cls.seqWidget.setChain(chainObj)
            cls.__dock.show()
        else:
            if main and viewer:
                dock = SequenceDock(main, viewer, chainObj)
                main.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)
                dock.show()
            else:
                if not main: print 'Sequence Dock Error: no main app'
                if not viewer: print 'Sequence Dock Error: no viewer'
                if not chainObj: print 'Sequence Dock: no chain to load'
                            
    def createActions(self):
        seqDockAct = QtGui.QAction(self.tr("Partly &Automated Atom Placement"), self)
        self.seqDockAct = seqDockAct
        seqDockAct.setStatusTip(self.tr("Place atoms based on predicted SSE's"))
        seqDockAct.setCheckable(True)
        seqDockAct.setChecked(False)
        self.connect(seqDockAct, QtCore.SIGNAL("triggered()"), SequenceDock.changeDockVisibility)
        self.app.actions.addAction("perform_autoAtomPlacement", seqDockAct)
    def updateFromViewerSelection(self, *argv):
        #hits = argv[:-1]
        #event = argv[-1]
        #print "SequenceDock.updateFromViewerSelection()"
        ####I don't understand the purpose of the boolean variable in the Hit Stack!
        try: 
            atom = CAlphaRenderer.getAtomFromHitStack(self.app.viewers['calpha'].renderer, argv[0], True, *argv[1:-1])
        except:
            print "Not an atom."
            return
        pdbID = atom.getPDBId()
        chainID = atom.getChainId()
        resNum = atom.getResSeq()
        print pdbID, chainID, resNum
        self.seqWidget.scrollable.seqView.setSequenceSelection([resNum])
        selectedChain = Chain.getChain((pdbID, chainID))
        selectedChain.setSelection([resNum])
#        self.emit( QtCore.SIGNAL("selection updated") )
#        self.emit( QtCore.SIGNAL('SequencePanelUpdate'))

class SequenceWidget(QtGui.QWidget):
    def __init__(self, chainObj, parent=None):
        super(SequenceWidget, self).__init__(parent)
        self.setMinimumSize(400,600)
        self.scrollable = ScrollableSequenceView(chainObj, self)
        self.scrollable.setMinimumSize(300, 180)
        self.threeResidues = ThreeResidues(chainObj, self)
        self.chainObj = chainObj
        
        self.globalView=GlobalSequenceView(chainObj)
        self.globalView.setLocalView(self.scrollable.seqView)
        self.globalView.updateViewportRange()

        self.connect(self.scrollable.seqView.scrollbar, QtCore.SIGNAL('actionTriggered(int)'), self.globalView.updateViewportRange)
        self.connect(self.scrollable.seqView.scrollbar, QtCore.SIGNAL('valueChanged(int)'), self.globalView.updateViewportRange)
        #self.connect(self.scrollable.seqView.scrollbar, QtCore.SIGNAL('rangeChanged(int)'), self.globalView.updateViewportRange)
        self.connect(self.scrollable.seqView, QtCore.SIGNAL('SequencePanelUpdate'), self.globalView.updateViewportRange)

        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.globalView)
        layout.addWidget(self.scrollable)
        layout.addWidget(self.threeResidues)
        self.setLayout(layout)
        self.setWindowTitle('Sequence Widget')

class ThreeResidues(QtGui.QWidget, Ui_threeResidues):
    def __init__(self, chainObj, parent=None):
        super(ThreeResidues, self).__init__(parent)
        self.setupUi(self)
        self.setMinimumSize(400,280)
        self.chainObj = chainObj
        self.connect(self.back1resButton, QtCore.SIGNAL('clicked()'), self.prevButtonPress)
        self.connect(self.forward1resButton, QtCore.SIGNAL('clicked()'), self.nextButtonPress)
    def setResidues(self, newSelection):
        #newSelection is a list of Residue indeces that are selected
        prevResNum = newSelection[-1]
        self.prevNum.setText(unicode(prevResNum))
        self.prevName.setText(unicode(self.chainObj[prevResNum]))
        self.curNum.setText(unicode(prevResNum+1))
        self.curName.setText(unicode(self.chainObj[prevResNum+1]))
        self.nextNum.setText(unicode(prevResNum+2))
        self.nextName.setText(unicode(self.chainObj[prevResNum+2]))
    def prevButtonPress(self):
        newSelection = [ self.parent().chainObj.getSelection()[-1] - 1 ]
        self.parent().scrollable.seqView.setSequenceSelection(newSelection)
        self.setResidues(newSelection)
        print 'prevButton'
        
    def nextButtonPress(self):
        newSelection = [ self.parent().chainObj.getSelection()[-1] + 1 ]
        self.parent().scrollable.seqView.setSequenceSelection(newSelection)
        self.setResidues(newSelection)        
        print 'nextButton'

class SequenceView(QtGui.QWidget):
  """
  This QWidget gives residues as one letter abbreviations for residues and the index below.  
  Most chains will be too big to fit on the screen on this class. Thus, a ScrollableSequenceView
  contains this class.
  """
  def __init__(self,sequence,parent=None):
    super(SequenceView,self).__init__(parent)
    self.parent = parent
    # Initialize font
    self.fontName='Arial'
    self.fontSize=30
    self.font=QtGui.QFont(self.fontName,self.fontSize)

    # Initialize sequence
    self.setSequence(sequence)

    #self.connect(self, QtCore.SIGNAL('SequencePanelUpdate'), self.repaint)
    self.setWindowModality(QtCore.Qt.NonModal)
    self.setMouseTracking(True) #necessary to listen to mouse with no buttons pressed

    metrics=QtGui.QFontMetrics(self.font)
    self.height = int(3.5*metrics.lineSpacing())
    self.width = int(40*metrics.maxWidth())
    self.resize(self.width, self.height)
    self.setMinimumSize(self.width, self.height)


  def setSequence(self, newSequence):
    self.sequence = newSequence
    self.sequence.fillGaps()
    self.residueRange=self.sequence.residueRange()
    self.connect(self.sequence, QtCore.SIGNAL("selection updated"), self.__selectionUpdated)
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
      if len(self.sequence[index].getAtomNames())==0:
        self.font.setBold(False)
        painter.setPen(QtCore.Qt.gray)
        painter.setFont(self.font)

      # residues with coordinates are BOLD and BLACK
      else:
        self.font.setBold(True)
        painter.setPen(QtCore.Qt.black)
        painter.setFont(self.font)

      # selected residues are YELLOW with a BLACK BACKGROUND
      if index in self.sequence.getSelection():
        yOffset= 0.75*(cellHeight-metrics.ascent()) 
        rect=QtCore.QRectF ( float(x), yOffset+float(y), float(cellWidth), float(-cellHeight))
        painter.fillRect(rect,brush)
        painter.setPen(QtCore.Qt.yellow)

      # draw the glyph 
      nextChar=self.sequence[index].__repr__()
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
    metrics=QtGui.QFontMetrics(self.font)
    cellHeight=metrics.lineSpacing()
    maxCharWidth=QtGui.QFontMetrics(self.font).maxWidth()
    index= int(x/maxCharWidth)
    if y > 1*cellHeight and y < 2*cellHeight:
      return self.residueRange[index]
    else:
      return None

  def mousePressEvent(self, mouseEvent):

    if mouseEvent.button() == QtCore.Qt.LeftButton and mouseEvent.y() < self.cellHeight():
      residue=self.getResidueIndexByMousePosition(mouseEvent.x(),self.cellHeight() +1)
      secel= self.sequence.getSecelByIndex(residue)
      newSelection=range(secel.startIndex, secel.stopIndex+1)
      #  CTRL key pressed
      if mouseEvent.modifiers() & QtCore.Qt.CTRL:
        self.setSequenceSelection(addRange=newSelection)
      else:
        self.setSequenceSelection(newSelection)

      self.repaint()
      return

    # LEFT MOUSE PRESS
    if mouseEvent.button() == QtCore.Qt.LeftButton:
      additionalResidue=self.getResidueIndexByMousePosition(mouseEvent.x(),mouseEvent.y())

      #  CTRL key pressed
      if mouseEvent.modifiers() & QtCore.Qt.CTRL:
        if additionalResidue not in self.sequence.getSelection():
          self.setSequenceSelection(addOne=additionalResidue)
        else:
          self.setSequenceSelection(removeOne=additionalResidue)

      #  SHIFT key pressed
      elif mouseEvent.modifiers() & QtCore.Qt.SHIFT:
        additionalResidue=self.getResidueIndexByMousePosition(mouseEvent.x(),mouseEvent.y())
        if additionalResidue > sorted(self.sequence.getSelection())[-1]:
          addedRange=range( 1+sorted(self.sequence.getSelection())[-1],1+additionalResidue)
          self.setSequenceSelection(addRange=addedRange)
        elif additionalResidue < sorted(self.sequence.getSelection())[0]:
          addedRange=range( additionalResidue, sorted(self.sequence.getSelection())[0])
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
    self.setFontSize(self.fontSize + 2)

  def decrementPointSize(self):
    self.setFontSize(self.fontSize - 2)

  def updatePanelHeight(self):
    metrics=QtGui.QFontMetrics(self.font)
    height=int(3.5*metrics.lineSpacing())
    width=metrics.maxWidth()*len(self.sequence.residueRange())
    self.resize(QtCore.QSize(width,height))


  def setSequenceSelection(self, newSelection=None, removeOne=None, addOne=None, addRange=None):
    print 'In SequenceView.setSequenceSelection'
    self.sequence.setSelection(newSelection,removeOne,addOne,addRange)
    #print newSelection
    #print self.parent.parent.threeResidues
    self.parent.parent.threeResidues.setResidues(newSelection)
    
    ####To do: Need to select residue in SequenceView if an atom is clicked in the renderer.
    viewer = self.parent.parent.parent().viewer
    atom = self.sequence[ self.sequence.getSelection()[-1] ].getAtom('CA')
    atom.setSelected(True)
    viewer.emitModelChanged()
    #viewer.loaded = True
    #viewer.emitModelLoaded()
          
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
    viewportWidth=self.parent.width()/cellWidth
    try: 
        start=self.residueRange[viewportStart]
    except:
        start = 1
    return range(start,start+viewportWidth+1)

class ScrollableSequenceView(QtGui.QScrollArea):
  """
  This QWidget contains a SequenceView object but in a scrollable view.
  """
  def __init__(self,sequence, parent=None):
    super(ScrollableSequenceView,self).__init__()
    self.parent = parent
    self.seqView=SequenceView(sequence,parent=self)
    seqView = self.seqView
    seqView.updatePanelHeight() ####This is needed to get all residues to show up in this widget.
    ####Note: updatePanelHeight also adjusts width - I'm guessing that is the part that fixes things.
    seqView.scrollbar=self.horizontalScrollBar()
    self.seqView=seqView
    self.setWidget(self.seqView)
    
    self.setWidgetResizable(False)
    self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
    self.setWindowTitle(QtCore.QString('Scrollable Local View'))
    self.updateHeight()

  def setSequence(self, newSequence):
    #self.globalView.setSequence(newSequence)
    self.seqView.setSequence(newSequence)


  def updateHeight(self):
    scrollbar=self.horizontalScrollBar()
    scrollbarHeight=scrollbar.height()
    widgetHeight=self.widget().height#()
    #self.resize(QtCore.QSize(self.width(), scrollbarHeight+widgetHeight))
    self.resize(QtCore.QSize(self.width(), widgetHeight))

  def getSeqview(self):
    return self.seqView

class GlobalSequenceView(QtGui.QWidget):
  """
  This QWidget shows a pictographic representation of a chain, with blocks
  for helices and arrows for strands.  It contains a SequenceView
  QWidget Object, and updates the SequenceView to show the residues corresponding
  to the selection on it, and vice versa.
  """
  def __init__(self,sequence,parent=None):
    super(GlobalSequenceView,self).__init__(parent)

    # Set up Geometry
    self.nCols=100
    self.widgetWidth=600
    self.cellHeight=40
    self.cellWidth=self.widgetWidth/self.nCols

    self.setSequence(sequence)

    # Set up QWidget Behavior
    self.connect(self, QtCore.SIGNAL('SequencePanelUpdate'), self.repaint)
    self.setWindowModality(QtCore.Qt.NonModal)
    self.setWindowTitle(QtCore.QString('Global View'))

  def setSequence(self, newSequence):
    self.sequence=newSequence
    self.connect(self.sequence, QtCore.SIGNAL("selection updated"), self.__selectionUpdated)
    seqLength=len(self.sequence.residueRange())
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
    self.__paintSecels(painter, self.cellWidth, self.cellHeight/2)

  def __paintSecels(self, painter, cellWidth, cellHeight):

    indexRange=self.sequence.residueRange()
    markerRange=range( len(indexRange) )
    for index,marker in zip(indexRange,markerRange):

      if len(self.sequence[index].getAtomNames())==0:
        alpha=100
      elif len(self.sequence[index].getAtomNames())==1:
        alpha=200
      else:
        alpha=255

      xOffset=(marker%self.nCols)*self.cellWidth
      yOffset=0.5*cellHeight+(marker/(self.nCols))*self.cellHeight

      secel=self.sequence.getSecelByIndex(index)
      color=secel.getColor()

      # Paint Selection Blocks
      if index in self.localView.sequence.getSelection():
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


def tempZoomDialog(seqView, scrollArea):
  plusButton=QtGui.QPushButton('+')
  plusButton.setMaximumWidth(50)
  plusButton.connect(plusButton, QtCore.SIGNAL("clicked()"), seqView.incrementPointSize)
  plusButton.connect(plusButton, QtCore.SIGNAL("clicked()"), scrollArea.updateHeight)

  minusButton=QtGui.QPushButton('-')
  minusButton.setMaximumWidth(50)
  minusButton.connect(minusButton, QtCore.SIGNAL("clicked()"), seqView.decrementPointSize)
  minusButton.connect(minusButton, QtCore.SIGNAL("clicked()"), scrollArea.updateHeight)
  minusButton.emit(QtCore.SIGNAL("clicked()"))
  # There is some bug with the scroll widget.  If I don't use the minusButton or plusButton once (e.g. previous line), then the scrollbar is wrong size.
  # Ross: I traced down the fix to SequenceView.updatePanelHeight().  That is what must be executed to get all residues to display.
  # Ross: Thus, I added a line to ScrollableSequenceWidget.__init__()

  layout=QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight)
  layout.addWidget(plusButton)
  layout.addWidget(minusButton)

  dialog=QtGui.QDialog()
  dialog.resize(QtCore.QSize(150,40))
  dialog.setModal(False)
  dialog.setLayout(layout)
  dialog.setWindowTitle(QtCore.QString('Zoom Dialog'))

  return dialog

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    groel = Chain.load('1KPO.pdb',app)
    for residue in groel.residueRange()[80:-80]:
        groel[residue].clearAtoms()

    seqWidget = SequenceWidget(groel)
    seqWidget.show()
    
    #seqScroll = ScrollableSequenceView(groel)
    #seqScroll.show()
    
    #seqVw = SequenceView(groel)
    #seqVw.show()
    
    dialog=tempZoomDialog(seqWidget.scrollable.getSeqview(),seqWidget.scrollable)
    dialog.show()
    dialog.raise_()

    sys.exit(app.exec_())
