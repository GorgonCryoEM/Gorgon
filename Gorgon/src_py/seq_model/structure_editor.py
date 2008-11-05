#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Class: StructureEditor
# Class Description: Widget for placing atoms and secondary strucutre elements as well as for optimizing placement.

from PyQt4 import QtGui, QtCore
from libpyGORGON import PDBAtom, PDBBond

class StructureEditor(QtGui.QWidget):
    def __init__(self, currentChainModel, parent=None):
        super(StructureEditor, self).__init__(parent)
        
        self.currentChainModel = currentChainModel
        self.atomJustAdded = None
        self.possibleAtomsList = []
        self.previouslyChosenPossibleAtomToDisplay = None
        self.undoStack = QtGui.QUndoStack(self)
        
        #These go on the left hand side
        self.undoButton = QtGui.QPushButton('Undo')
        self.redoButton = QtGui.QPushButton('Redo')
        self.CAdoubleSpinBox = QtGui.QDoubleSpinBox()
        self.CAdoubleSpinBox.setValue(3.8)
        self.CAlabel = QtGui.QLabel('C-Alplha Interval')
        self.mockSidechainsCheckBox = QtGui.QCheckBox('Mock Sidechains')
        self.acceptButton = QtGui.QPushButton('Accept')
        
        
        self.setupUi()
        
        self.connect(self.back1resButton, QtCore.SIGNAL('clicked()'), self.prevButtonPress)
        self.connect(self.forward1resButton, QtCore.SIGNAL('clicked()'), self.nextButtonPress)
        self.connect(self.acceptButton, QtCore.SIGNAL('clicked()'),  self.acceptButtonPress)
        self.connect(self.possibilityNumSpinBox,  QtCore.SIGNAL('valueChanged(int)'),  self.choosePossibleAtomToDisplay)
        self.connect(self.atomicForwardRadioButton,  QtCore.SIGNAL('toggled(bool)'), self.forwardBackwardRadioButtonChange)
        self.connect(self.undoButton,  QtCore.SIGNAL('clicked()'), self.undoStack.undo)
        self.connect(self.redoButton,  QtCore.SIGNAL('clicked()'), self.undoStack.redo)
      
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
            #self.parentWidget()=>SequenceWidget, self.parentWidget().parentWidget() => SequenceDock
            viewer = self.parentWidget().parentWidget().viewer
            if self.atomicBackwardRadioButton.isChecked():
                resSeqNum = int(self.atomicResNumbers[-1].text())
            elif self.atomicForwardRadioButton.isChecked():
                resSeqNum = int(self.atomicResNumbers[1].text())
            chosenAtom.setResSeq(resSeqNum)
            bondBefore = None
            bondAfter = None
            if resSeqNum - 1 in self.currentChainModel.residueRange():
                prevCAlpha = self.currentChainModel[resSeqNum - 1].getAtom('CA')
                if prevCAlpha:
                    print "adding a bond before"
                    bondBefore=PDBBond()
                    bondBefore.setAtom0Ix(prevCAlpha.getHashKey())
                    bondBefore.setAtom1Ix(chosenAtom.getHashKey())
            if resSeqNum + 1 in self.currentChainModel.residueRange():
                nextCAlpha = self.currentChainModel[resSeqNum + 1].getAtom('CA')
                if nextCAlpha:
                    print "adding a bond after"
                    bondAfter = PDBBond()
                    bondAfter.setAtom0Ix(nextCAlpha.getHashKey())
                    bondAfter.setAtom1Ix(chosenAtom.getHashKey())
            
            
            command = CommandAcceptAtomPlacement( self.currentChainModel, resSeqNum, chosenAtom, viewer, bondBefore, bondAfter,  
                                "Accept Location of C-alpha atom for residue #%s" % resSeqNum)
            self.undoStack.push(command)
            self.atomJustAdded = chosenAtom
            
            if self.atomicBackwardRadioButton.isChecked():
                self.prevButtonPress()
            elif self.atomicForwardRadioButton.isChecked():
                self.nextButtonPress()
                
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
            
    def clearMockSidechains(self,  chain):
        for index in chain.residueRange():
            res = chain[index]
            res.setCAlphaColorToDefault()
            res.setCAlphaSizeToDefault()
        print "The mock side-chains should be cleared,  but not yet drawn to the screen."
        
    def findCAlphaPositionPossibilities(self):
        print 'in findCAlphaPositionPossibilities'
        self.possibleAtomsList = []
        radius = float( self.CAdoubleSpinBox.value() )
        #self.parentWidget()=>SequenceWidget, self.parentWidget().parentWidget() => SequenceDock
        skeletonViewer = self.parentWidget().parentWidget().app.viewers['skeleton']
        meshRenderer = skeletonViewer.renderer
        atom = self.currentChainModel[ int( str(self.atomicResNumbers[0].text()) ) ].getAtom('CA')
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
                rawAtom=PDBAtom(self.currentChainModel.getPdbID(), self.currentChainModel.getChainID() , i+1, 'CA')
                rawAtom.setPosition(pos)
                rawAtom.setElement('C')
                rawAtom.setColor(0, 1, 0, 1)
                
                try:
                    prevAtom = self.currentChainModel[int( str(self.atomicResNumbers[0].text()) )-1].getAtom('CA')
                    previousAtomPos = prevAtom.getPosition()
                    prevDistSquared = (pos.x() - previousAtomPos.x())**2 + (pos.y() - previousAtomPos.y())**2 + (pos.z() - previousAtomPos.z())**2
                except KeyError,  IndexError:
                    prevDistSquared = 100000
                except AttributeError:
                    prevDistSquared = 100000
                try:
                    nextAtom = self.currentChainModel[int( str(self.atomicResNumbers[0].text()) )+1].getAtom('CA')
                    nextAtomPos = nextAtom.getPosition()
                    nextDistSquared = (pos.x() - nextAtomPos.x())**2 + (pos.y() - nextAtomPos.y())**2 + (pos.z() - nextAtomPos.z())**2
                except KeyError,  IndexError:
                    nextDistSquared = 100000
                except AttributeError:
                    nextDistSquared = 100000
                
                if  prevDistSquared < 4 or nextDistSquared < 4: #two C-alpha atoms are never less than 2 angstroms apart
                     print 'one possible atom was too close to an existing atom'
                else:
                    self.possibleAtomsList.append(rawAtom)
                
            print 'possible atoms:',  self.possibleAtomsList
            self.numPossibilities.setText('of ' + str(len(self.possibleAtomsList)))
            #Note that a valueChanged signal might be emitted in either or both of the following two lines.
            self.possibilityNumSpinBox.setRange(1, len(self.possibleAtomsList))
            self.possibilityNumSpinBox.setValue(1)
            
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
        currentChainModel = self.parentWidget().currentChainModel
        if currentChainModel.getSelection():
            newSelection = [ currentChainModel.getSelection()[-1] + 1 ]
            if newSelection[0] > max(currentChainModel.residueRange()): 
                return
            self.parentWidget().scrollable.seqView.setSequenceSelection(newSelection)
            self.setResidues(newSelection)
    
    def prevButtonPress(self):
        #self.parentWidget() returns a SequenceWidget object
        currentChainModel = self.parentWidget().currentChainModel
        if currentChainModel.getSelection():
            newSelection = [ currentChainModel.getSelection()[-1] - 1 ]
            if newSelection[0] <min(currentChainModel.residueRange()): 
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
        print '\nin set residues\n'
        #newSelection is a list of Residue indeces that are selected
        if not newSelection:
            print 'In sequence_view.StructureEdit.setResidues().  The new selection is empty!'
            return
        
        if self.previouslyChosenPossibleAtomToDisplay and not (self.atomJustAdded is self.previouslyChosenPossibleAtomToDisplay): 
            #If we did not add the atom to a residue, we want to remove it from the renderer
            #self.parentWidget() is SequenceWidget & self.parentWidget().parentWidget() is SequenceDock
            viewer = self.parentWidget().parentWidget().viewer
            viewer.renderer.deleteAtom(self.previouslyChosenPossibleAtomToDisplay.getHashKey())
            viewer.emitModelChanged()
        
        self.atomJustAdded = None
        self.previouslyChosenPossibleAtomToDisplay = None
        curResNum = newSelection[-1]
        
        for i in self.atomicResNames.keys():
            try: 
                self.atomicResNames[i].setText(unicode(self.currentChainModel[curResNum+i]))
                self.atomicResNumbers[i].setText(unicode(curResNum+i))
            except (IndexError,  KeyError):
                self.atomicResNames[i].setText('')
                self.atomicResNumbers[i].setText('')
        self.findCAlphaPositionPossibilities()
        
    def setupAtomicTab(self):
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
    
    def setupHelixTab(self):
        #These go in the Helix tab
        self.modifyRadioButton = QtGui.QRadioButton(self.tr('Modify'))
        self.newRadioButtion = QtGui.QRadioButton(self.tr('New'))
        NterminusLabel = QtGui.QLabel('N term')
        self.NtermResNameLabel = QtGui.QLabel('?')
        self.NtermSpinBox = QtGui.QSpinBox()
        self.NtermSpinBox.setRange(1, 10000)
        CterminusLabel = QtGui.QLabel('C term')
        self.CtermResNameLabel = QtGui.QLabel('?')
        self.CtermSpinBox = QtGui.QSpinBox()
        self.CtermSpinBox.setRange(1, 10000)
        
        self.decreasePositionButton = QtGui.QPushButton('-')
        self.decreasePositionButton.setMaximumWidth(30)
        positionLabel = QtGui.QLabel(self.tr('Position'))
        self.increasePositionButton = QtGui.QPushButton('+')
        self.increasePositionButton.setMaximumWidth(30)
        self.createCAHelixButton = QtGui.QPushButton(self.tr('Create C-alpha Helix'))
        self.removeButton = QtGui.QPushButton(self.tr('Remove'))
        self.flipButton = QtGui.QPushButton(self.tr('Flip'))
        
        radioLayout = QtGui.QHBoxLayout()
        radioLayout.addWidget(self.modifyRadioButton)
        radioLayout.addWidget(self.newRadioButtion)
        radioLayout.addStretch()
        
        NtermLayout = QtGui.QHBoxLayout()
        NtermLayout.addWidget(NterminusLabel)
        NtermLayout.addWidget(self.NtermResNameLabel)
        NtermLayout.addWidget(self.NtermSpinBox)
        NtermLayout.addStretch()        
        
        CtermLayout = QtGui.QHBoxLayout()
        CtermLayout.addWidget(CterminusLabel)
        CtermLayout.addWidget(self.CtermResNameLabel)
        CtermLayout.addWidget(self.CtermSpinBox)
        CtermLayout.addStretch()
        
        positionLayout = QtGui.QHBoxLayout()
        positionLayout.addWidget(self.decreasePositionButton)
        positionLayout.addWidget(positionLabel)
        positionLayout.addWidget(self.increasePositionButton)
        positionLayout.addStretch()
        
        createLayout = QtGui.QHBoxLayout()
        createLayout.addWidget(self.createCAHelixButton)
        createLayout.addStretch()
        removeLayout = QtGui.QHBoxLayout()
        removeLayout.addWidget(self.removeButton)
        removeLayout.addStretch()
        flipLayout = QtGui.QHBoxLayout()
        flipLayout.addWidget(self.flipButton)
        flipLayout.addStretch()
                
        helixLayout  = QtGui.QVBoxLayout()
        helixLayout.addLayout(radioLayout)
        helixLayout.addLayout(NtermLayout)
        helixLayout.addLayout(CtermLayout)
        helixLayout.addLayout(positionLayout)
        helixLayout.addLayout(createLayout)
        helixLayout.addLayout(removeLayout)
        helixLayout.addLayout(flipLayout)
        self.helixTab.setLayout(helixLayout)
        
    def setupLoopTab(self):
        self.loopStartLabel = QtGui.QLabel('Start Residue')
        self.loopStartSpinBox = QtGui.QSpinBox()
        self.loopStopLabel = QtGui.QLabel('Stop Residue')
        self.loopStopSpinBox = QtGui.QSpinBox()
        self.loopIDLabel = QtGui.QLabel('Loop ID & Score')
        self.loopFindButton = QtGui.QPushButton('Find Loops')
        self.loopComboBox = QtGui.QComboBox()
        self.loopAcceptButton = QtGui.QPushButton('Accept Loop')
        self.loopRejectButton = QtGui.QPushButton('Reject Loop')
        
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
    
    def setupPositionTab(self):
        self.posTranslateLabel = QtGui.QLabel('Translate')
        self.posRotateLabel = QtGui.QLabel('Rotate')
        self.posMoveLabelsDict = {
                              'x': QtGui.QLabel('x'), 
                              'y': QtGui.QLabel('y'), 
                              'z': QtGui.QLabel('z'), 
                              'alt': QtGui.QLabel('alt'), 
                              'az': QtGui.QLabel('az'), 
                              'phi': QtGui.QLabel('phi')
                              }
                              
        self.posMoveDict = {
                                   'x': QtGui.QDoubleSpinBox(), 
                                   'y': QtGui.QDoubleSpinBox(), 
                                   'z': QtGui.QDoubleSpinBox(), 
                                    'alt': QtGui.QSlider(), 
                                    'az': QtGui.QSlider(), 
                                    'phi': QtGui.QSlider()
                                   }
        
        for key in ['alt', 'az', 'phi']:
            self.posMoveDict[key].setRange(0, 360)
            self.posMoveDict[key].setOrientation(QtCore.Qt.Horizontal)
        
        self.posDecreaseButtonDict = {
                                   'x': QtGui.QPushButton('-'), 
                                   'y': QtGui.QPushButton('-'), 
                                   'z': QtGui.QPushButton('-'), 
                                   'alt': QtGui.QPushButton('-5'), 
                                   'az': QtGui.QPushButton('-5'), 
                                   'phi': QtGui.QPushButton('-5')
                                   }
        self.posIncreaseButtonDict = {
                                   'x': QtGui.QPushButton('+'), 
                                   'y': QtGui.QPushButton('+'), 
                                   'z': QtGui.QPushButton('+'), 
                                   'alt': QtGui.QPushButton('+5'), 
                                   'az': QtGui.QPushButton('+5'), 
                                   'phi': QtGui.QPushButton('+5')
                                   }
        for key in self.posDecreaseButtonDict.keys():
            self.posDecreaseButtonDict[key].setMaximumWidth(30)
            self.posIncreaseButtonDict[key].setMaximumWidth(30)

        posSpinLabelLayoutDict = {
                              'x': QtGui.QHBoxLayout(), 
                              'y': QtGui.QHBoxLayout(), 
                              'z': QtGui.QHBoxLayout(), 
                              'alt': QtGui.QHBoxLayout(), 
                              'az': QtGui.QHBoxLayout(), 
                              'phi': QtGui.QHBoxLayout()
                              }
        
        self.posAutofitRangeDict = {
                                    'x': QtGui.QDoubleSpinBox(), 
                                    'y': QtGui.QDoubleSpinBox(), 
                                    'z': QtGui.QDoubleSpinBox(), 
                                    'alt': QtGui.QDoubleSpinBox(), 
                                    'az': QtGui.QDoubleSpinBox(), 
                                    'phi': QtGui.QDoubleSpinBox()
                                    }
        
        for key in posSpinLabelLayoutDict.keys():
            curLayout = posSpinLabelLayoutDict[key]
            curLayout.addWidget(self.posMoveLabelsDict[key])
            curLayout.addWidget(self.posDecreaseButtonDict[key])
            curLayout.addWidget(self.posMoveDict[key])
            curLayout.addWidget(self.posIncreaseButtonDict[key])
            curLayout.addStretch()
        
        
        
        positionLayout = QtGui.QGridLayout()
        positionLayout.addWidget(self.posTranslateLabel, 0, 0)
        positionLayout.addLayout(posSpinLabelLayoutDict['x'], 1, 0)
        positionLayout.addLayout(posSpinLabelLayoutDict['y'], 2,  0)
        positionLayout.addLayout(posSpinLabelLayoutDict['z'], 3, 0)
        positionLayout.addWidget(self.posRotateLabel, 4, 0)
        positionLayout.addLayout(posSpinLabelLayoutDict['alt'], 5, 0)
        positionLayout.addLayout(posSpinLabelLayoutDict['az'], 6, 0)
        positionLayout.addLayout(posSpinLabelLayoutDict['phi'], 7, 0)
        self.positionTab.setLayout(positionLayout)
        
        
        
    def setupUi(self):
        self.tabWidget = QtGui.QTabWidget()
        self.helixTab = QtGui.QWidget()
        self.atomicTab = QtGui.QWidget()
        self.loopTab = QtGui.QWidget()
        self.positionTab = QtGui.QWidget()
        
        resNameFont = QtGui.QFont(self.font())
        resNameFont.setPointSize(41)
        resIndexFont = QtGui.QFont(self.font())
        resIndexFont.setPointSize(13)
        
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
        self.tabWidget.addTab(self.positionTab, self.tr('Position'))
    
        self.setupHelixTab()
        self.setupAtomicTab()
        self.setupLoopTab()
        self.setupPositionTab()

        layout.addLayout(leftLayout)
        layout.addWidget(self.tabWidget)
        self.setLayout(layout)
        
class CommandAcceptAtomPlacement(QtGui.QUndoCommand):
        def __init__(self, currentChainModel, resSeqNum, chosenAtom, viewer, bondBefore=None, bondAfter=None, description=None):
            super(CommandAcceptAtomPlacement, self).__init__(description)
            self.currentChainModel = currentChainModel
            self.resSeqNum = resSeqNum
            self.chosenAtom = chosenAtom
            self.viewer = viewer
            self.bondBefore = bondBefore
            self.bondAfter = bondAfter
        def redo(self):
            self.currentChainModel[self.resSeqNum].addAtomObject(self.chosenAtom)
            self.currentChainModel[self.resSeqNum].setCAlphaColorToDefault()      
            self.viewer.renderer.addAtom(self.chosenAtom)
            if self.bondBefore:
                self.viewer.renderer.addBond(self.bondBefore)
            if self.bondAfter:
                self.viewer.renderer.addBond(self.bondAfter)
            self.viewer.emitModelChanged()
        def undo(self):
            self.currentChainModel[self.resSeqNum].clearAtom(self.chosenAtom.getName())
            self.viewer.renderer.deleteAtom(self.chosenAtom.getHashKey())
            if self.bondBefore:
                #TODO: find out how to delete bond
                #self.viewer.renderer.deleteBond(self.bondBefore.?) #The parameter is an integer
                pass
            if self.bondAfter:
                #self.viewer.renderer.deleteBond(self.bondAfter.?)
                pass
            self.viewer.emitModelChanged()
