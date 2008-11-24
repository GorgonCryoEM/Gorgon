#!/usr/bin/python
import sys
from PyQt4 import Qt,QtGui,QtCore

from seq_model.Helix import Helix
from seq_model.Strand import Strand
from seq_model.Chain import Chain
from seq_model.Residue import Residue
from seq_model.structure_editor import StructureEditor
from libpyGORGON import CAlphaRenderer, PDBAtom,  PDBBond

class SequenceDock(QtGui.QDockWidget):
    __dock = None
    
    def __init__(self, main, viewer, structurePrediction, currentChainModel, parent=None):
        super(SequenceDock, self).__init__(parent)
        self.app = main
        self.currentChainModel = currentChainModel
        self.structurePrediction = structurePrediction
        self.viewer=viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.seqWidget = SequenceWidget( structurePrediction, currentChainModel, self)
        self.setWidget(self.seqWidget)
        self.createActions()
        SequenceDock.__dock = self
        self.connect(self.seqWidget.structureEditor.mockSidechainsCheckBox,  QtCore.SIGNAL('stateChanged(int)'),  self.toggleMockSideChains)
        if main:
            self.connect(self.app.viewers["calpha"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.updateFromViewerSelection)    
    
    @classmethod
    def changeDockVisibility(cls, main, viewer, structurePrediction, currentChainModel):
        if not structurePrediction.chain.residueRange():
            return
        if not currentChainModel.residueRange():
            currentChainModel = Chain('', main)
            for i in structurePrediction.chain.residueRange():
                #This will not copy PDBAtom objects, but it will copy (not reference) the residue objects and place them in the new chain
                #It will not copy any Secel objects (Helix, Strand, Sheet, Coil, Secel)
                #Thus, it it prepares a clean chain object to begin building a model
                resSymbol3 = structurePrediction.chain[i].symbol3
                res = Residue(resSymbol3, currentChainModel)
                currentChainModel[i] = res
                
        if cls.__dock:
            if cls.__dock.app.actions.getAction("seqDock").isChecked():
                cls.__dock.app.addDockWidget(QtCore.Qt.LeftDockWidgetArea,  cls.__dock)
                cls.__dock.changeCurrentChainModel(currentChainModel)
                cls.__dock.show()
            else:
                cls.__dock.app.removeDockWidget(cls.__dock)
        else:
            if main and viewer:
                dock = SequenceDock(main, viewer, structurePrediction, currentChainModel)
                main.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)
                dock.show()
            else:
                if not main: print 'Sequence Dock Error: no main app'
                if not viewer: print 'Sequence Dock Error: no viewer'
                if not currentChainModel: print 'Sequence Dock: no chain to load'
                            
    def createActions(self):
        seqDockAct = QtGui.QAction(self.tr("Partly &Automated Atom Placement"), self)
        self.seqDockAct = seqDockAct
        seqDockAct.setStatusTip(self.tr("Place atoms based on predicted SSE's"))
        seqDockAct.setCheckable(True)
        seqDockAct.setChecked(False)
        self.connect(seqDockAct, QtCore.SIGNAL("triggered()"), SequenceDock.changeDockVisibility)
        self.app.actions.addAction("perform_autoAtomPlacement", seqDockAct)
    
    def changeCurrentChainModel(self, currentChainModel):
        self.currentChainModel = currentChainModel
        self.seqWidget.currentChainModel = currentChainModel
        seqView = self.seqWidget.scrollable.seqView
        seqView.currentChainModel = currentChainModel
        seqView.repaint()
        structureEditor = self.seqWidget.structureEditor
        structureEditor.currentChainModel = currentChainModel        
        for i in structureEditor.atomicResNumbers.keys():
            structureEditor.atomicResNumbers[i].setText('?')
            structureEditor.atomicResNames[i].setText('?')
    
    def changeSequenceSSE(self, structurePrediction):
        self.structurePrediction = structurePrediction
    
    def closeEvent(self, event):
        self.app.actions.getAction("seqDock").setChecked(False)
        
    def updateFromViewerSelection(self, *argv):
        #hits = argv[:-1]
        #event = argv[-1]
        #print "SequenceDock.updateFromViewerSelection()"
        #TODO: I don't understand the purpose of the boolean variable in the Hit Stack!
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
        self.seqWidget.structureEditor.setResidues([resNum])
        selectedChain = Chain.getChain((pdbID, chainID))
        selectedChain.setSelection([resNum])
    
    def toggleMockSideChains(self):
        viewer = self.viewer
        
        if self.seqWidget.structureEditor.mockSidechainsCheckBox.isChecked():
            self.seqWidget.structureEditor.renderMockSidechains(self.currentChainModel)
        else:
            self.seqWidget.structureEditor.clearMockSidechains(self.currentChainModel)         
                
        viewer.emitModelChanged()
            
class SequenceWidget(QtGui.QWidget):
    def __init__(self, structurePrediction, currentChainModel, parent=None):
        super(SequenceWidget, self).__init__(parent)
        self.currentChainModel = currentChainModel
        self.setMinimumSize(400,600)
        self.scrollable = ScrollableSequenceView(structurePrediction, currentChainModel, self)
        self.scrollable.setMinimumSize(300, 180)
        self.structureEditor = StructureEditor(currentChainModel, self)
        
        self.globalView=GlobalSequenceView(structurePrediction, self)
        self.globalView.setLocalView(self.scrollable.seqView)
        self.globalView.updateViewportRange()
        self.setMaximumWidth(self.globalView.width())

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


class SequenceView(QtGui.QWidget):
  """
  This QWidget gives residues as one letter abbreviations for residues and the index below.  
  Most chains will be too big to fit on the screen on this class. Thus, a ScrollableSequenceView
  contains this class.
  """
  def __init__(self, structurePrediction, currentChainModel, parent=None):
    super(SequenceView,self).__init__(parent)
    # Initialize font
    self.fontName='Arial'
    self.fontSize=30
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
    self.structurePrediction = newStructurePrediction
    self.structurePrediction.chain.fillGaps()
    self.updatePanelHeight()
    self.residueRange=self.structurePrediction.chain.residueRange()
    self.connect(self.structurePrediction.chain, QtCore.SIGNAL("selection updated"), self.__selectionUpdated)
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
    if mouseEvent.button() == QtCore.Qt.LeftButton:
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
    self.setFontSize(self.fontSize + 2)

  def decrementPointSize(self):
    self.setFontSize(self.fontSize - 2)

  def updatePanelHeight(self):
    metrics=QtGui.QFontMetrics(self.font)
    height=int(3.5*metrics.lineSpacing())
    width=metrics.maxWidth()*len(self.structurePrediction.chain.residueRange())
    self.resize(QtCore.QSize(width,height))


  def setSequenceSelection(self, newSelection=None, removeOne=None, addOne=None, addRange=None):
    dock = self.parentWidget().parentWidget().parentWidget().parentWidget()
    #self.parentWidget() => QWidget, self.parentWidget().parentWidget() => ScrollableSequenceView
    #self.parentWidget().parentWidget().parentWidget() => SequenceDock
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
    
    try:
        selectedAtom = self.currentChainModel[ self.currentChainModel.getSelection()[-1] ].getAtom('CA')
    except KeyError:
        return
    if not selectedAtom:
        return
    
    selectedAtom.setSelected(True)    
    dock.app.mainCamera.centerOnSelectedAtom()    
    viewer.emitModelChanged()

          
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
    #self.parentWidget() => QWidget object, self.parentWidget().parentWidget() => ScrollableSequenceView object
    try: 
        start=self.residueRange[viewportStart]
    except:
        start = 1
    return range(start,start+viewportWidth+1)

class ScrollableSequenceView(QtGui.QScrollArea):
  """
  This QWidget contains a SequenceView object but in a scrollable view.
  """
  def __init__(self,structurePrediction, currentChainModel, parent=None):
    super(ScrollableSequenceView,self).__init__()
    self.seqView=SequenceView(structurePrediction, currentChainModel,parent=self)
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
  def __init__(self, structurePrediction, parent=None):
    super(GlobalSequenceView, self).__init__(parent)

    # Set up Geometry
    self.nCols=100
    self.widgetWidth=600
    self.cellHeight=40
    self.cellWidth=self.widgetWidth/self.nCols

    self.setStructurePrediction(structurePrediction)    

    # Set up QWidget Behavior
    self.connect(self, QtCore.SIGNAL('SequencePanelUpdate'), self.repaint)
    self.setWindowModality(QtCore.Qt.NonModal)
    self.setWindowTitle(QtCore.QString('Global View'))

  def setStructurePrediction(self, newStructurePrediction):
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


def renderCAlphas(chain):
  for index in chain.residueRange():
    res=chain[index]
    if 'CA' in res.getAtomNames():
        thisAtom = Chain.getViewer().renderer.addAtom(res.getAtom('CA'))
        res.addAtomObject(thisAtom)

        if index-1 in chain.residueList:
            previousRes=chain[index-1]
            if 'CA' in previousRes.getAtomNames():
              print 'index=%s' %index
              previousAtom=previousRes.getAtom('CA')
              bond=PDBBond()
              bond.setAtom0Ix(previousAtom.getSerial())
              bond.setAtom0Ix(thisAtom.getSerial())
              Chain.getViewer().renderer.addBond(bond)

def renderMockSidechains(chain):
    obj = StructureEditor(chain)
    obj.renderMockSidechains(chain)
def clearMockSidechains(chain):
    obj = StructureEditor(chain)
    obj.clearMockSidechains(chain)

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
    
    #threeRes = StructureEditor(groel)
    #threeRes.show()
    
    #seqScroll = ScrollableSequenceView(groel)
    #seqScroll.show()
    
    #seqVw = SequenceView(groel)
    #seqVw.show()
    
    #dialog=tempZoomDialog(seqWidget.scrollable.getSeqview(),seqWidget.scrollable)
    #dialog.show()
    #dialog.raise_()

    sys.exit(app.exec_())
