#!/usr/bin/python
import sys
from PyQt4 import Qt,QtGui,QtCore

from seq_model.Helix import Helix
from seq_model.Strand import Strand
from seq_model.Chain import Chain
from seq_model.Residue import Residue
from libpyGORGON import CAlphaRenderer, PDBAtom,  PDBBond

class SequenceDock(QtGui.QDockWidget):
    __dock = None
    
    def __init__(self, main, viewer, chainObj, parent=None):
        super(SequenceDock, self).__init__(parent)
        self.app = main
        self.chainObj = chainObj
        self.viewer=viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.seqWidget = SequenceWidget(chainObj, self)
        self.setWidget(self.seqWidget)
        self.createActions()
        SequenceDock.__dock = self
        self.connect(self.seqWidget.structureEditor.mockSidechainsCheckBox,  QtCore.SIGNAL('stateChanged(int)'),  self.toggleMockSideChains)
        if main:
            self.connect(self.app.viewers["calpha"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.updateFromViewerSelection)    
    
    @classmethod
    def changeDockVisibility(cls, main, viewer):
        chainObj = Chain.getChain( Chain.getSelectedChainKey() )
        if not chainObj: chainObj = Chain('', main)
        if cls.__dock:
            if cls.__dock.app.actions.getAction("seqDock").isChecked():
                cls.__dock.app.addDockWidget(QtCore.Qt.LeftDockWidgetArea,  cls.__dock)
                cls.__dock.seqWidget.scrollable.setSequence(chainObj)
                cls.__dock.show()
            else:
                cls.__dock.app.removeDockWidget(cls.__dock)
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
        #TODO: determine if there is a way to update the display without deleting and re-adding atoms to the renderer
        viewer = self.viewer
        
        if self.seqWidget.structureEditor.mockSidechainsCheckBox.isChecked():
            self.seqWidget.structureEditor.renderMockSidechains(self.chainObj)
        else:
            self.seqWidget.structureEditor.clearMockSidechains(self.chainObj)         
       
        for i in self.chainObj.residueRange():
            atom = self.chainObj[i].getAtom('CA')
            if atom:
                viewer.renderer.deleteAtom(atom.getHashKey())
                viewer.renderer.addAtom(atom)
                
        viewer.emitModelChanged()
            
class SequenceWidget(QtGui.QWidget):
    def __init__(self, chainObj, parent=None):
        super(SequenceWidget, self).__init__(parent)
        self.chainObj = chainObj
        self.setMinimumSize(400,600)
        self.scrollable = ScrollableSequenceView(chainObj, self)
        self.scrollable.setMinimumSize(300, 180)
        self.structureEditor = StructureEditor(chainObj, self)
        
        
        self.globalView=GlobalSequenceView(chainObj)
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

class StructureEditor(QtGui.QWidget):
    def __init__(self, chainObj, parent=None):
        super(StructureEditor, self).__init__(parent)
        
        self.chainObj = chainObj
        self.possibleAtomsList = []
        self.previouslyChosenPossibleAtomToDisplay = None
        
        #These go on the left hand side
        self.undoButton = QtGui.QPushButton('Undo')
        self.redoButton = QtGui.QPushButton('Redo')
        self.CAdoubleSpinBox = QtGui.QDoubleSpinBox()
        self.CAdoubleSpinBox.setValue(3.8)
        self.CAlabel = QtGui.QLabel('C-Alplha Interval')
        self.mockSidechainsCheckBox = QtGui.QCheckBox('Mock Sidechains')
        self.acceptButton = QtGui.QPushButton('Accept')
        self.tabWidget = QtGui.QTabWidget()
        self.helixTab = QtGui.QWidget()
        self.atomicTab = QtGui.QWidget()
        self.loopTab = QtGui.QWidget()
        self.optimizeTab = QtGui.QWidget()
        
        resNameFont = QtGui.QFont(self.font())
        resNameFont.setPointSize(41)
        resIndexFont = QtGui.QFont(self.font())
        resIndexFont.setPointSize(13)
        
        #These go in the atomic tab
        self.possibilityNumSpinBox = QtGui.QSpinBox()
        self.possibilityNumSpinBox.setRange(0, 0)
        self.numPossibilities = QtGui.QLabel('of ?')
        self.atomicForwardRadioButton = QtGui.QRadioButton('Forward')
        self.atomicForwardRadioButton.setChecked(True)
        self.atomicBackwardRadioButton = QtGui.QRadioButton('Backward')
        self.atomicResNames = { -2:QtGui.QLabel('?'),  -1:QtGui.QLabel('?'),  0:QtGui.QLabel('?'), 1:QtGui.QLabel('?'), 2:QtGui.QLabel('?') }
        self.atomicResNumbers = { -2:QtGui.QLabel('#?'),  -1:QtGui.QLabel('#?'),  0:QtGui.QLabel('#?'), 1:QtGui.QLabel('#?'), 2:QtGui.QLabel('#?') }
                
        for i in self.atomicResNames.keys():
            self.atomicResNames[i].setStyleSheet("QLabel {font-size: 40pt}")#.setFont(resNameFont)
            self.atomicResNumbers[i].setAlignment(QtCore.Qt.AlignHCenter)
            self.atomicResNumbers[i].setStyleSheet("QLabel {font-size: 12pt}") #.setFont(resIndexFont)
        
        self.atomicResNumbers[0].setStyleSheet("QLabel {color: white; background-color:black; font-size: 12pt}") #This uses syntax similar to Cascading Style Sheets (CSS)
        self.atomicResNames[0].setStyleSheet("QLabel {color: white; background-color:black; font-size: 40pt}") 
        self.atomicResNumbers[1].setStyleSheet("QLabel {color: green; font-size: 12pt}") #This uses syntax similar to Cascading Style Sheets (CSS)
        self.atomicResNames[1].setStyleSheet("QLabel {color: green; font-size: 40pt}")
        
        self.back1resButton = QtGui.QPushButton('<-')
        self.forward1resButton = QtGui.QPushButton('->') 
        
        #These go in the loop tab
        self.loopStartLabel = QtGui.QLabel('Start Residue')
        self.loopStartSpinBox = QtGui.QSpinBox()
        self.loopStopLabel = QtGui.QLabel('Stop Residue')
        self.loopStopSpinBox = QtGui.QSpinBox()
        self.loopIDLabel = QtGui.QLabel('Loop ID & Score')
        self.loopFindButton = QtGui.QPushButton('Find Loops')
        self.loopComboBox = QtGui.QComboBox()
        self.loopAcceptButton = QtGui.QPushButton('Accept Loop')
        self.loopRejectButton = QtGui.QPushButton('Reject Loop')
        
        #These go in the optimize tab
        self.optimizeButton = QtGui.QPushButton('Optimize')
        self.optFastRadioButton = QtGui.QRadioButton('Fast')
        self.optExhaustiveRadioButton = QtGui.QRadioButton('Exhaustive')
        self.optTranslateLabel = QtGui.QLabel('Translate')
        self.optRotateLabel = QtGui.QLabel('Rotate')
        self.optMoveLabelsDict = {
                              'x': QtGui.QLabel('x'), 
                              'y': QtGui.QLabel('y'), 
                              'z': QtGui.QLabel('z'), 
                              'alt': QtGui.QLabel('alt'), 
                              'az': QtGui.QLabel('az'), 
                              'phi': QtGui.QLabel('phi')
                              }
        self.optMoveSpinBoxDict = {
                                   'x': QtGui.QDoubleSpinBox(), 
                                   'y': QtGui.QDoubleSpinBox(), 
                                   'z': QtGui.QDoubleSpinBox(), 
                                   'alt': QtGui.QDoubleSpinBox(), 
                                   'az': QtGui.QDoubleSpinBox(), 
                                   'phi': QtGui.QDoubleSpinBox()
                                   }
        self.setupUi()
        self.connect(self.back1resButton, QtCore.SIGNAL('clicked()'), self.prevButtonPress)
        self.connect(self.forward1resButton, QtCore.SIGNAL('clicked()'), self.nextButtonPress)
        self.connect(self.acceptButton, QtCore.SIGNAL('clicked()'),  self.acceptButtonPress)
        self.connect(self.possibilityNumSpinBox,  QtCore.SIGNAL('valueChanged(int)'),  self.choosePossibleAtomToDisplay)
        self.connect(self.atomicForwardRadioButton,  QtCore.SIGNAL('toggled(bool)'), self.forwardBackwardRadioButtonChange)
    
    def setupUi(self):
        layout = QtGui.QHBoxLayout()
        
        leftLayout = QtGui.QVBoxLayout()
        leftLayout.addStretch()
        
        undoRedoLayout = QtGui.QHBoxLayout()
        undoRedoLayout.addWidget(self.undoButton)
        undoRedoLayout.addWidget(self.redoButton)
        undoRedoLayout.addStretch()
        
        leftLayout.addLayout(undoRedoLayout)
        leftLayout.addWidget(self.mockSidechainsCheckBox)
        
        CAIntervalLayout = QtGui.QHBoxLayout()
        CAIntervalLayout.addWidget(self.CAdoubleSpinBox)
        CAIntervalLayout.addWidget(self.CAlabel)
        
        acceptLayout = QtGui.QHBoxLayout()
        acceptLayout.addStretch()
        acceptLayout.addWidget(self.acceptButton)
        acceptLayout.addStretch()
        
        leftLayout.addLayout(CAIntervalLayout)        
        leftLayout.addLayout(acceptLayout)
        leftLayout.addStretch()
        
        self.tabWidget.addTab(self.helixTab, self.tr('Helix Editor'))
        self.tabWidget.addTab(self.atomicTab, self.tr('Atomic Editor'))
        self.tabWidget.addTab(self.loopTab, self.tr('Loop Editor'))
        self.tabWidget.addTab(self.optimizeTab, self.tr('Optimize'))
        
        atomicLayout = QtGui.QVBoxLayout()
        
        atomicPossibilityLayout = QtGui.QHBoxLayout()
        atomicPossibilityLayout.setAlignment(QtCore.Qt.AlignHCenter)
        atomicPossibilityLayout.addWidget(self.possibilityNumSpinBox)
        atomicPossibilityLayout.addWidget(self.numPossibilities)
        atomicForwardBackwardLayout = QtGui.QVBoxLayout()
        atomicForwardBackwardLayout.addWidget(self.atomicForwardRadioButton)
        atomicForwardBackwardLayout.addWidget(self.atomicBackwardRadioButton)
        atomicPossibilityLayout.addLayout(atomicForwardBackwardLayout)        
        atomicLayout.addLayout(atomicPossibilityLayout)
        
        atomic3ResLayout = QtGui.QHBoxLayout()
        atomic3ResSublayouts = {-2:QtGui.QVBoxLayout(), -1:QtGui.QVBoxLayout(), 0:QtGui.QVBoxLayout(), 1:QtGui.QVBoxLayout(), 2:QtGui.QVBoxLayout() }

        atomic3ResLayout.addStretch()
        for i in sorted(atomic3ResSublayouts.keys()):
            atomic3ResSublayouts[i].addWidget(self.atomicResNumbers[i])
            atomic3ResSublayouts[i].addWidget(self.atomicResNames[i])
            atomic3ResLayout.addLayout(atomic3ResSublayouts[i])

        atomic3ResLayout.addStretch()
        atomicLayout.addLayout(atomic3ResLayout)

        atomicResButtonLayout = QtGui.QHBoxLayout()
        atomicResButtonLayout.addWidget(self.back1resButton)
        atomicResButtonLayout.addWidget(self.forward1resButton)
        atomicLayout.addLayout(atomicResButtonLayout)
        self.atomicTab.setLayout(atomicLayout)
        
        loopLayout = QtGui.QGridLayout()
        loopLayout.addWidget(self.loopStartLabel, 0, 0)
        loopLayout.addWidget(self.loopStartSpinBox, 0, 1)
        loopLayout.addWidget(self.loopStopLabel, 1, 0)
        loopLayout.addWidget(self.loopStopSpinBox, 1, 1)
        loopLayout.addWidget(self.loopIDLabel, 2, 1)
        loopLayout.addWidget(self.loopFindButton, 3, 0)
        loopLayout.addWidget(self.loopComboBox, 3, 1)
        loopLayout.addWidget(self.loopAcceptButton, 4, 0)
        loopLayout.addWidget(self.loopRejectButton, 4, 1)
        self.loopTab.setLayout(loopLayout)
        
        optimizeLayout = QtGui.QGridLayout()
        optimizeLayout.addWidget(self.optimizeButton, 0, 0,  2, 1)
        optimizeLayout.addWidget(self.optFastRadioButton, 0, 1)
        optimizeLayout.addWidget(self.optExhaustiveRadioButton, 1, 1)
        optimizeLayout.addWidget(self.optTranslateLabel, 2, 0)
        optimizeLayout.addWidget(self.optRotateLabel, 2, 1)
        optSpinLabelLayoutDict = {
                              'x': QtGui.QHBoxLayout(), 
                              'y': QtGui.QHBoxLayout(), 
                              'z': QtGui.QHBoxLayout(), 
                              'alt': QtGui.QHBoxLayout(), 
                              'az': QtGui.QHBoxLayout(), 
                              'phi': QtGui.QHBoxLayout()
                              }
        for key in optSpinLabelLayoutDict.keys():
            curLayout = optSpinLabelLayoutDict[key]
            curLayout.addWidget(self.optMoveSpinBoxDict[key])
            curLayout.addWidget(self.optMoveLabelsDict[key])
        
        optimizeLayout.addLayout(optSpinLabelLayoutDict['x'], 3, 0)
        optimizeLayout.addLayout(optSpinLabelLayoutDict['y'], 4, 0)
        optimizeLayout.addLayout(optSpinLabelLayoutDict['z'], 5, 0)
        optimizeLayout.addLayout(optSpinLabelLayoutDict['alt'], 3, 1)
        optimizeLayout.addLayout(optSpinLabelLayoutDict['az'], 4, 1)
        optimizeLayout.addLayout(optSpinLabelLayoutDict['phi'], 5, 1)
        self.optimizeTab.setLayout(optimizeLayout)
        
        layout.addLayout(leftLayout)
        layout.addWidget(self.tabWidget)
        self.setLayout(layout)
    
    def acceptButtonPress(self):
        currentWidget = self.tabWidget.currentWidget()
        if currentWidget is self.atomicTab:
            possibleAtoms = self.possibleAtomsList
            possibilityNum = self.possibilityNumSpinBox.value()
            chosenAtom = possibleAtoms[possibilityNum-1]
            for atom in possibleAtoms:
                if atom is chosenAtom:
                    continue
                else:
                    del atom
            viewer = self.parentWidget().parentWidget().viewer
            if self.atomicBackwardRadioButton.isChecked():
                resSeqNum = int(self.atomicResNumbers[-1].text())
            elif self.atomicForwardRadioButton.isChecked():
                resSeqNum = int(self.atomicResNumbers[1].text())
            chosenAtom.setResSeq(resSeqNum)
            self.chainObj[resSeqNum].addAtomObject(chosenAtom)
            #self.parentWidget()=>SequenceWidget, self.parentWidget().parentWidget() => SequenceDock
            self.chainObj[resSeqNum].setCAlphaColorToDefault()            
            viewer.renderer.addAtom(chosenAtom)
            if resSeqNum - 1 in self.chainObj.residueRange():
                prevCAlpha = self.chainObj[resSeqNum - 1].getAtom('CA')
                if prevCAlpha:
                    print "adding a bond before"
                    bond=PDBBond()
                    bond.setAtom0Ix(prevCAlpha.getHashKey())
                    bond.setAtom1Ix(chosenAtom.getHashKey())
                    viewer.renderer.addBond(bond)
            if resSeqNum + 1 in self.chainObj.residueRange():
                nextCAlpha = self.chainObj[resSeqNum + 1].getAtom('CA')
                if nextCAlpha:
                    print "adding a bond after"
                    bond = PDBBond()
                    bond.setAtom0Ix(nextCAlpha.getHashKey())
                    bond.setAtom1Ix(chosenAtom.getHashKey())
                    viewer.renderer.addBond(bond)
            viewer.emitModelChanged()
            if self.atomicBackwardRadioButton.isChecked():
                self.prevButtonPress()
            elif self.atomicForwardRadioButton.isChecked():
                self.nextButtonPress()
            print 'end acceptButtonPress'
            
    def clearMockSidechains(self,  chain):
        for index in chain.residueRange():
            res = chain[index]
            res.setCAlphaColorToDefault()
            res.setCAlphaSizeToDefault()
        print "The mock side-chains should be cleared,  but not yet drawn to the screen."
    
    def choosePossibleAtomToDisplay(self,  choiceNum):
        if choiceNum == 0:
            return
        viewer = self.parentWidget().parentWidget().viewer
        if self.previouslyChosenPossibleAtomToDisplay:
            viewer.renderer.deleteAtom(self.previouslyChosenPossibleAtomToDisplay.getHashKey())
        atomToDisplay = self.possibleAtomsList[choiceNum-1]
        viewer.renderer.addAtom(atomToDisplay)
        viewer.emitModelChanged()
        self.previouslyChosenPossibleAtomToDisplay = atomToDisplay
    
    def findCAlphaPositionPossibilities(self):
        print 'in findCAlphaPositionPossibilities'
        self.possibleAtomsList = []
        radius = float( self.CAdoubleSpinBox.value() )
        #self.parentWidget()=>SequenceWidget, self.parentWidget().parentWidget() => SequenceDock
        skeletonViewer = self.parentWidget().parentWidget().app.viewers['skeleton']
        meshRenderer = skeletonViewer.renderer
        atom = self.chainObj[ int( str(self.atomicResNumbers[0].text()) ) ].getAtom('CA')
        if not atom:
            self.possibilityNumSpinBox.setRange(0, 0)
            self.numPossibilities.setText('of ?')
            return
        atomPos = atom.getPosition()
        #print atomPos,  radius
        if skeletonViewer.loaded:
            print "Number of intersections:", 
            numIntersections = meshRenderer.intersectMeshAndSphere(atomPos, radius)
            print numIntersections
            
            if numIntersections == 0:
                print "No possibilities!"
                self.numPossibilities.setText('of 0')
                self.possibilityNumSpinBox.setRange(0, 0)
                return
            possiblePositionsList = []
            for i in range(numIntersections):
                possiblePositionsList.append( meshRenderer.getIntersectionPoint(i) )
            print 'Possible positions:',  possiblePositionsList
            for i in range(len(possiblePositionsList)):
                pos = possiblePositionsList[i]
                x, y, z = (pos.x(), pos.y(), pos.z())
                print '(%f, %f, %f)' % (x, y, z)
                rawAtom=PDBAtom(self.chainObj.getPdbID(), self.chainObj.getChainID() , i+1, 'CA')
                rawAtom.setPosition(pos)
                rawAtom.setElement('C')
                rawAtom.setColor(0, 1, 0, 1)
                
                try:
                    prevAtom = self.chainObj[int( str(self.atomicResNumbers[0].text()) )-1].getAtom('CA')
                    previousAtomPos = prevAtom.getPosition()
                    prevDistSquared = (pos.x() - previousAtomPos.x())**2 + (pos.y() - previousAtomPos.y())**2 + (pos.z() - previousAtomPos.z())**2
                except KeyError,  IndexError:
                    prevDistSquared = 100000
                except AttributeError:
                    prevDistSquared = 100000
                try:
                    nextAtom = self.chainObj[int( str(self.atomicResNumbers[0].text()) )+1].getAtom('CA')
                    nextAtomPos = nextAtom.getPosition()
                    nextDistSquared = (pos.x() - nextAtomPos.x())**2 + (pos.y() - nextAtomPos.y())**2 + (pos.z() - nextAtomPos.z())**2
                except KeyError,  IndexError:
                    nextDistSquared = 100000
                except AttributeError:
                    nextDistSquared = 100000
                
                if  prevDistSquared < 4 or nextDistSquared < 4:
                     print 'one possible atom was too close to an existing atom'
                else:
                    self.possibleAtomsList.append(rawAtom)
                
            print 'possible atoms:',  self.possibleAtomsList
            self.numPossibilities.setText('of ' + str(len(self.possibleAtomsList)))
            self.possibilityNumSpinBox.setRange(1, len(self.possibleAtomsList))
            
            for atom in self.possibleAtomsList:
                if atom.getResSeq() == self.possibilityNumSpinBox.value():
                    self.previouslyChosenPossibleAtomToDisplay = atom #We remove this atom from the viewer when we display a different possibility
                    self.parentWidget().parentWidget().viewer.renderer.addAtom(atom)
                    self.parentWidget().parentWidget().viewer.emitModelChanged()
    
    def forwardBackwardRadioButtonChange(self):
        if self.atomicForwardRadioButton.isChecked():
            self.atomicResNumbers[1].setStyleSheet("QLabel {color: green; font-size: 12pt}")
            self.atomicResNames[1].setStyleSheet("QLabel {color: green; font-size: 40pt}")
            self.atomicResNumbers[-1].setStyleSheet("QLabel {color: black; font-size: 12pt}")
            self.atomicResNames[-1].setStyleSheet("QLabel {color: black; font-size: 40pt}")
        elif self.atomicBackwardRadioButton.isChecked():
            self.atomicResNumbers[-1].setStyleSheet("QLabel {color: green; font-size: 12pt}")
            self.atomicResNames[-1].setStyleSheet("QLabel {color: green; font-size: 40pt}")
            self.atomicResNumbers[1].setStyleSheet("QLabel {color: black; font-size: 12pt}")
            self.atomicResNames[1].setStyleSheet("QLabel {color: black; font-size: 40pt}")
            
    def nextButtonPress(self):
        chainObj = self.parentWidget().chainObj
        if chainObj.getSelection():
            newSelection = [ chainObj.getSelection()[-1] + 1 ]
            if newSelection[0] > max(chainObj.residueRange()): 
                return
            self.parentWidget().scrollable.seqView.setSequenceSelection(newSelection)
            self.setResidues(newSelection)
    
    def prevButtonPress(self):
        #self.parentWidget() returns a SequenceWidget object
        chainObj = self.parentWidget().chainObj
        if chainObj.getSelection():
            newSelection = [ chainObj.getSelection()[-1] - 1 ]
            if newSelection[0] <min(chainObj.residueRange()): 
                return
            self.parentWidget().scrollable.seqView.setSequenceSelection(newSelection)
            self.setResidues(newSelection)
            
    def renderMockSidechains(self,  chain):
        color = {
            'greasy': (0.0, 1.0, 0.0, 1.0), 
            'polarNoSulfur': (0.0, 0.0, 0.6, 1.0), 
            'charged': (0.0, 0.0, 1.0, 1.0), 
            'sulfur': (1.0, 1.0, 0.0, 1.0)
            }
        for index in chain.residueRange():
            res = chain[index]
            atom = res.getAtom('CA')
            if not atom:
                continue
            size = 1.8 * res.size[res.symbol3]
            atom.setAtomRadius( size  )
            for key in color.keys():
                if res.symbol3 in res.residueTypes[key]:
                    atom.setColor( *color[key] )
                    break
        print "The mock side-chains should be ready to draw to the screen"

    def setResidues(self, newSelection):
        #newSelection is a list of Residue indeces that are selected
        if not newSelection:
            print 'In sequence_view.StructureEdit.setResidues().  The new selection is empty!'
            return
        curResNum = newSelection[-1]
        
        for i in self.atomicResNames.keys():
            try: 
                self.atomicResNames[i].setText(unicode(self.chainObj[curResNum+i]))
                self.atomicResNumbers[i].setText(unicode(curResNum+i))
            except (IndexError,  KeyError):
                self.atomicResNames[i].setText('')
                self.atomicResNumbers[i].setText('')
        self.findCAlphaPositionPossibilities()

class SequenceView(QtGui.QWidget):
  """
  This QWidget gives residues as one letter abbreviations for residues and the index below.  
  Most chains will be too big to fit on the screen on this class. Thus, a ScrollableSequenceView
  contains this class.
  """
  def __init__(self,sequence,parent=None):
    super(SequenceView,self).__init__(parent)
    # Initialize font
    self.fontName='Arial'
    self.fontSize=30
    self.font=QtGui.QFont(self.fontName,self.fontSize)

    # Initialize sequence
    self.sequence = sequence
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
      
      self.parentWidget().parentWidget().parentWidget().structureEditor.setResidues([resNum])
      self.repaint()
      return

    # LEFT MOUSE PRESS
    if mouseEvent.button() == QtCore.Qt.LeftButton:
      additionalResidue=self.getResidueIndexByMousePosition(mouseEvent.x(),mouseEvent.y())
      self.parentWidget().parentWidget().parentWidget().structureEditor.setResidues([additionalResidue])
      
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
    dock = self.parentWidget().parentWidget().parentWidget().parentWidget()
    #self.parentWidget() => QWidget, self.parentWidget().parentWidget() => ScrollableSequenceView
    #self.parentWidget().parentWidget().parentWidget() => SequenceDock
    viewer = dock.viewer
    renderer = viewer.renderer
    app = dock.app
    
    selectionToClear = self.sequence.getSelection()
    for i in selectionToClear:
        try: 
            self.sequence[i]
        except KeyError: 
            continue
        atom = self.sequence[i].getAtom('CA')
        if atom:
            renderer.deleteAtom(atom.getHashKey())
            atom.setSelected(False)
            renderer.addAtom(atom)
    self.sequence.setSelection(newSelection,removeOne,addOne,addRange)
    
    try:
        selectedAtom = self.sequence[ self.sequence.getSelection()[-1] ].getAtom('CA')
    except KeyError:
        return
    if not selectedAtom:
        return
    renderer.deleteAtom(selectedAtom.getHashKey())
    selectedAtom.setSelected(True)
    renderer.addAtom(selectedAtom)
    
    pos = selectedAtom.getPosition()
    x, y, z = pos.x()*viewer.scale[0],  pos.y()*viewer.scale[1],  pos.z()*viewer.scale[2]
    app.mainCamera.setCenter( x, y, z )
    viewer.emitModelChanged()
    
    #TODO: change BaseViewer.modelLoaded() to redraw PDBAtoms if their attributes have changed
    #Current: Here we delete atoms from the renderer and add them again, which isn't the best way, probably.

          
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
  def __init__(self,sequence, parent=None):
    super(ScrollableSequenceView,self).__init__()
    self.seqView=SequenceView(sequence,parent=self)
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


def renderCAlphas(chain):
  for index in chain.residueRange():
    res=chain[index]
    if 'CA' in res.getAtomNames():
        thisAtom=res.getAtom('CA')
        Chain.getViewer().renderer.addAtom(groel[index].getAtom('CA'))

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
