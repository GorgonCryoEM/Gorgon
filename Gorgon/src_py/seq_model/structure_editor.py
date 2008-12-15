#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Class: StructureEditor
# Class Description: Widget for placing atoms and secondary strucutre elements as well as for optimizing placement.

from PyQt4 import QtGui, QtCore
from libpyGORGON import PDBAtom, PDBBond, Vector3DFloat
import sans_numpy as snum
from seq_model.findHelixCalphas import helixEndpointsToCAlphaPositions
from seq_model.Helix import Helix
import math
import vector_lib

class StructureEditor(QtGui.QWidget):
    def __init__(self, currentChainModel, parent=None):
        super(StructureEditor, self).__init__(parent)
        
        self.currentChainModel = currentChainModel
        self.atomJustAdded = None
        self.possibleAtomsList = []
        self.previouslySelectedPossibleAtom = None
        self.undoStack = QtGui.QUndoStack(self)
        self.currentMatch = None
        
        #These go on the left hand side
        self.undoButton = QtGui.QPushButton('Undo')
        self.redoButton = QtGui.QPushButton('Redo')
        self.CAdoubleSpinBox = QtGui.QDoubleSpinBox()
        self.CAdoubleSpinBox.setValue(3.8)
        self.CAlabel = QtGui.QLabel('C-Alplha Interval')
        self.mockSidechainsCheckBox = QtGui.QCheckBox('Mock Sidechains')
        self.acceptButton = QtGui.QPushButton('Accept')
        self.removeButton = QtGui.QPushButton('Remove')
                
        self.setupUi()
        self.enableDisable()
        
        self.connect(self.atomicBack1resButton, QtCore.SIGNAL('clicked()'), self.atomPrevButtonPress)
        self.connect(self.atomicForward1resButton, QtCore.SIGNAL('clicked()'), self.atomNextButtonPress)
        self.connect(self.acceptButton, QtCore.SIGNAL('clicked()'),  self.acceptButtonPress)
        self.connect(self.atomicPossibilityNumSpinBox,  QtCore.SIGNAL('valueChanged(int)'),  self.atomChoosePossibleAtom)
        self.connect(self.atomicForwardRadioButton,  QtCore.SIGNAL('toggled(bool)'), self.atomForwardBackwardChange)
        self.connect(self.undoButton,  QtCore.SIGNAL('clicked()'), self.undoStack.undo)
        self.connect(self.redoButton,  QtCore.SIGNAL('clicked()'), self.undoStack.redo)
        self.connect(self.helixDecreasePositionButton, QtCore.SIGNAL('clicked()'), self.helixDecreaseButtonPress)
        self.connect(self.helixIncreasePositionButton, QtCore.SIGNAL('clicked()'), self.helixIncreaseButtonPress)
        self.connect(self.helixFlipButton, QtCore.SIGNAL('clicked()'), self.helixFlipButtonPress)
        self.connect(self.currentChainModel, QtCore.SIGNAL('selection updated'), self.updateSelectedResidues)
        self.connect(self.tabWidget, QtCore.SIGNAL('currentChanged(int)'), self.enableDisable)
        if self.parentWidget().parentWidget().app:
            self.app = self.parentWidget().parentWidget().app
            self.CAlphaViewer = self.app.viewers['calpha']            
            self.connect(self.app.viewers['sse'], QtCore.SIGNAL('elementSelected (int, int, int, int, int, int, QMouseEvent)'), self.updateCurrentMatch)
            self.connect(self.app.viewers["calpha"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.posUpdateValues)
            self.connect(self.posMoveDict['x'], QtCore.SIGNAL('valueChanged(double)'), self.posMoveCM_x)
            self.connect(self.posMoveDict['y'], QtCore.SIGNAL('valueChanged(double)'), self.posMoveCM_y)
            self.connect(self.posMoveDict['z'], QtCore.SIGNAL('valueChanged(double)'), self.posMoveCM_z)
            self.connect(self.posMoveDict['roll'], QtCore.SIGNAL('valueChanged(int)'), self.posRotateCM_roll)
            self.connect(self.posMoveDict['pitch'], QtCore.SIGNAL('valueChanged(int)'), self.posRotateCM_pitch)
            self.connect(self.posMoveDict['yaw'], QtCore.SIGNAL('valueChanged(int)'), self.posRotateCM_yaw)
            self.connect(self.removeButton, QtCore.SIGNAL('clicked()'), self.removeSelectedAtoms)
      
    def acceptButtonPress(self):
        print '\nAccept Button Pressed'
        currentWidget = self.tabWidget.currentWidget()
        if currentWidget is self.atomicTab:
            possibilityNum = self.atomicPossibilityNumSpinBox.value()
            chosenAtom = self.possibleAtomsList[possibilityNum-1]
            #self.parentWidget()=>SequenceWidget, self.parentWidget().parentWidget() => SequenceDock
            viewer = self.parentWidget().parentWidget().viewer
            for atom in self.possibleAtomsList:
                if atom is chosenAtom:
                    chosenCoordinates = chosenAtom.getPosition()
                viewer.renderer.deleteAtom(atom.getHashKey())
                del atom
            self.possibleAtomsList = []
            if self.atomicBackwardRadioButton.isChecked():
                resSeqNum = int(self.atomicResNumbers[-1].text())
            elif self.atomicForwardRadioButton.isChecked():
                resSeqNum = int(self.atomicResNumbers[1].text())
            command = CommandAcceptAtomPlacement( self.currentChainModel, self, resSeqNum, chosenCoordinates, viewer,  
                                description = "Accept Location of C-alpha atom for residue #%s" % resSeqNum )
            self.undoStack.push(command)
        elif currentWidget is self.helixTab:
            self.helixCreateCAhelix()
    
    def atomChoosePossibleAtom(self, choiceNum):
    	print 'atomChoosePossibleAtom'
        if choiceNum == 0:
            return
        viewer = self.parentWidget().parentWidget().viewer
        if self.previouslySelectedPossibleAtom:
            self.previouslySelectedPossibleAtom.setColor(0, 1, 0, 1)
            
        atomToDisplay = self.possibleAtomsList[choiceNum-1]
        atomToDisplay.setColor(0, 1, 1, 1)
        viewer.emitModelChanged()
        self.previouslySelectedPossibleAtom = atomToDisplay
        
    def atomFindPositionPossibilities(self):
    	print 'atomFindPositionPossibilities'
        self.possibleAtomsList = []
        #self.parentWidget()=>SequenceWidget, self.parentWidget().parentWidget() => SequenceDock
        skeletonViewer = self.parentWidget().parentWidget().app.viewers['skeleton']
        meshRenderer = skeletonViewer.renderer
        radius = float( self.CAdoubleSpinBox.value() )
        resNum = int( str(self.atomicResNumbers[0].text()) )
        residue = self.currentChainModel[ resNum ]
        atom = residue.getAtom('CA')
        if not atom:
            self.atomicPossibilityNumSpinBox.setRange(0, 0)
            self.atomicNumPossibilities.setText('of ?')
            return
        if self.currentChainModel[resNum-1].getAtomNames() and self.currentChainModel[resNum+1].getAtomNames():
            self.atomicNumPossibilities.setText('of 0')
            self.atomicPossibilityNumSpinBox.setRange(0, 0)
            return
        atomPos = atom.getPosition()
        atomPosMeshCoords =  skeletonViewer.worldToObjectCoordinates(self.CAlphaViewer.objectToWorldCoordinates([atomPos.x(), atomPos.y(), atomPos.z()]))
        atomPosMeshCoords = Vector3DFloat(atomPosMeshCoords[0], atomPosMeshCoords[1], atomPosMeshCoords[2])
         
        if skeletonViewer.loaded:
            assert skeletonViewer.renderer.getSpacingX() == skeletonViewer.renderer.getSpacingY()
            assert skeletonViewer.renderer.getSpacingX() == skeletonViewer.renderer.getSpacingZ()
            numIntersections = meshRenderer.intersectMeshAndSphere( atomPosMeshCoords, radius/skeletonViewer.renderer.getSpacingX() )
            #TODO: find a more elegant way than dividing radius by the apix_X to fix the scaling problem
            #print "\nNumber of intersections:", numIntersections
            if numIntersections == 0:
                self.atomicNumPossibilities.setText('of 0')
                self.atomicPossibilityNumSpinBox.setRange(0, 0)
                return
            possiblePositionsList = []
            for i in range(numIntersections):
                pos = meshRenderer.getIntersectionPoint(i)
                pos = self.CAlphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates([pos.x(), pos.y(), pos.z()]))
                pos = Vector3DFloat(pos[0], pos[1], pos[2])                               
                possiblePositionsList.append(pos)
            for i in range(len(possiblePositionsList)):
                pos = possiblePositionsList[i]
                rawAtom=PDBAtom(self.currentChainModel.getPdbID(), self.currentChainModel.getChainID() , i+1, 'CA')
                rawAtom.setPosition(pos)
                rawAtom.setElement('C')
                rawAtom.setColor(0, 1, 0, 1)
                
                try:
                    prevAtom = self.currentChainModel[resNum-1].getAtom('CA')
                    previousAtomPos = prevAtom.getPosition()
                    prevDistSquared = (pos.x() - previousAtomPos.x())**2 + (pos.y() - previousAtomPos.y())**2 + (pos.z() - previousAtomPos.z())**2
                except KeyError,  IndexError:
                    prevDistSquared = 100000
                except AttributeError:
                    prevDistSquared = 100000
                try:
                    nextAtom = self.currentChainModel[resNum+1].getAtom('CA')
                    nextAtomPos = nextAtom.getPosition()
                    nextDistSquared = (pos.x() - nextAtomPos.x())**2 + (pos.y() - nextAtomPos.y())**2 + (pos.z() - nextAtomPos.z())**2
                except KeyError,  IndexError:
                    nextDistSquared = 100000
                except AttributeError:
                    nextDistSquared = 100000
                
                if  prevDistSquared < 4 or nextDistSquared < 4: #two C-alpha atoms are never less than 2 angstroms apart
                    #print "Removed impossibly close atom location"
                    pass
                else:
                    self.possibleAtomsList.append(rawAtom)
                
            print '\nPossible atom locations:'
            for atom in self.possibleAtomsList:
                print "(%f, %f, %f)" % (atom.getPosition().x(), atom.getPosition().y(), atom.getPosition().z())
            self.atomicNumPossibilities.setText('of ' + str(len(self.possibleAtomsList)))
            #Note that a valueChanged signal might be emitted in either or both of the following two lines.
            self.atomicPossibilityNumSpinBox.setRange(1, len(self.possibleAtomsList))
            self.atomicPossibilityNumSpinBox.setValue(1)
            
            for index in range( len(self.possibleAtomsList) ):
                atom = self.possibleAtomsList[index]
                atom.setColor(0, 1, 0, 1)
                self.possibleAtomsList.pop(index)
                atom = self.parentWidget().parentWidget().viewer.renderer.addAtom(atom)
                self.possibleAtomsList.insert(index, atom)
                
                if index + 1 == self.atomicPossibilityNumSpinBox.value():
                    atom.setColor(0, 1, 1, 1)
                    self.previouslySelectedPossibleAtom = atom #We change this atom's colors when we select a different possibility
                    
            self.parentWidget().parentWidget().viewer.emitModelChanged()
    
    def atomForwardBackwardChange(self):
    	print 'atomForwardBackwardChange'
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
    
    def atomNextButtonPress(self):
    	print 'atomNextButtonPress'
        currentChainModel = self.parentWidget().currentChainModel
        if currentChainModel.getSelection():
            newSelection = [ currentChainModel.getSelection()[-1] + 1 ]
            if newSelection[0] > max(currentChainModel.residueRange()): 
                return
            self.parentWidget().scrollable.seqView.setSequenceSelection(newSelection)
            #self.setResidues(newSelection)
    
    def atomPrevButtonPress(self):
    	print 'atomPrevButtonPress'
        #self.parentWidget() returns a SequenceWidget object
        currentChainModel = self.parentWidget().currentChainModel
        if currentChainModel.getSelection():
            newSelection = [ currentChainModel.getSelection()[-1] - 1 ]
            if newSelection[0] <min(currentChainModel.residueRange()): 
                return
            self.parentWidget().scrollable.seqView.setSequenceSelection(newSelection)
            #self.setResidues(newSelection)
            
    def clearMockSidechains(self,  chain):
        for index in chain.residueRange():
            res = chain[index]
            res.setCAlphaColorToDefault()
            res.setCAlphaSizeToDefault()
        print "The mock side-chains should be cleared, but not yet drawn to the screen."
    
    def enableDisable(self):
        currentTab = self.tabWidget.currentWidget()
        if currentTab is self.atomicTab:
            self.CAdoubleSpinBox.setEnabled(True)
            self.CAlabel.setEnabled(True)
            self.acceptButton.setEnabled(True)
            self.removeButton.setEnabled(True)
            self.redoButton.setEnabled(True)
            self.undoButton.setEnabled(True)
        elif currentTab is self.helixTab:
            self.CAdoubleSpinBox.setEnabled(False)
            self.CAlabel.setEnabled(False)
            self.acceptButton.setEnabled(True)
            self.removeButton.setEnabled(True)
            self.redoButton.setEnabled(False)
            self.undoButton.setEnabled(False)
        elif currentTab is self.loopTab:
            self.CAdoubleSpinBox.setEnabled(False)
            self.CAlabel.setEnabled(False)
            self.acceptButton.setEnabled(False)
            self.removeButton.setEnabled(True)
            self.redoButton.setEnabled(False)
            self.undoButton.setEnabled(False)
        elif currentTab is self.positionTab:
            self.CAdoubleSpinBox.setEnabled(False)
            self.CAlabel.setEnabled(False)
            self.acceptButton.setEnabled(False)
            self.removeButton.setEnabled(True)
            self.redoButton.setEnabled(False)
            self.undoButton.setEnabled(False)            
    
    def helixCreateCAhelix(self):
        print 'In helixCreateCAhelix'
        startIndex = self.helixNtermSpinBox.value()
        stopIndex = self.helixCtermSpinBox.value()
        observedHelix = self.currentMatch.observed
        direction = self.currentMatch.direction #Forward=0, Reverse=1
        predHelix = self.currentMatch.predicted
        if observedHelix.__class__.__name__ != 'ObservedHelix':
            raise TypeError, observedHelix.__class__.__name__
            
        helix = Helix(self.currentChainModel, predHelix.serialNo,  predHelix.label, startIndex, stopIndex)
        self.currentChainModel.addHelix(predHelix.serialNo, helix)
        
        moveStart = 1.5*(startIndex - predHelix.startIndex)
        print 'moveStart', moveStart
        moveEnd = 1.5*(stopIndex - predHelix.stopIndex)
        print 'moveEnd', moveEnd
        midpoint = observedHelix.getMidpoint()
        unitVector = observedHelix.getUnitVector()
        print 'unitVector', unitVector
        structPredCoord1 = snum.vectorAdd( midpoint, snum.scalarTimesVector(-1*predHelix.getAngstromLength()/2, unitVector) )
        structPredCoord2 = snum.vectorAdd( midpoint, snum.scalarTimesVector(predHelix.getAngstromLength()/2, unitVector) )
        
        
        if direction == 0:
            startMoveVector = snum.scalarTimesVector( moveStart, unitVector)
            endMoveVector = snum.scalarTimesVector( moveEnd, unitVector)
            coord1 = snum.vectorAdd(structPredCoord1, startMoveVector)
            coord2 = snum.vectorAdd(structPredCoord2, endMoveVector)
            helix.setAxisPoints(coord1, coord2)
        elif direction == 1:
            startMoveVector = snum.scalarTimesVector( -1*moveStart, unitVector)
            endMoveVector = snum.scalarTimesVector( -1*moveEnd, unitVector)
            coord1 = snum.vectorAdd(structPredCoord1, endMoveVector)
            coord2 = snum.vectorAdd(structPredCoord2, startMoveVector)
            helix.setAxisPoints(coord1, coord2)
        
        helixCoordList = helixEndpointsToCAlphaPositions(coord1, coord2)
        print helixCoordList                
        
        '''
        #To see the ends of the helical axis as green and red atoms
        startAtom = PDBAtom('AAAA', 'A', 100000, 'CA')
        startAtom.setPosition(Vector3DFloat(*coord1))
        startAtom.setColor(0, 1, 0, 1)
        startAtom = self.CAlphaViewer.renderer.addAtom(startAtom)
        stopAtom = startAtom = PDBAtom('AAAA', 'A', 100001, 'CA')
        stopAtom.setPosition(Vector3DFloat(*coord2))
        stopAtom.setColor(1, 0, 0, 1)        
        stopAtom = self.CAlphaViewer.renderer.addAtom(stopAtom)
        '''
        
        for i in range(len(helixCoordList)):
            pos = helixCoordList[i]
            residue = self.currentChainModel[startIndex+i]
            rawAtom = residue.addAtom('CA', pos[0], pos[1], pos[2], 'C')
            atom = self.CAlphaViewer.renderer.addAtom(rawAtom)
            residue.addAtomObject(atom)
            atom.setSelected(True)
            if i != 0:
                prevAtom = self.currentChainModel[startIndex+i-1].getAtom('CA')
                bond = PDBBond()
                bond.setAtom0Ix(prevAtom.getHashKey())
                bond.setAtom1Ix(atom.getHashKey())
                self.CAlphaViewer.renderer.addBond(bond)
        
        self.currentChainModel.setSelection(newSelection = range(helix.startIndex, 1+helix.stopIndex))
        print helix
        if not self.CAlphaViewer.loaded:
            self.CAlphaViewer.loaded = True
            self.CAlphaViewer.emitModelLoaded()
        else:
            self.CAlphaViewer.emitModelChanged()
        

    def helixDecreaseButtonPress(self):
        startIx = self.helixNtermSpinBox.value()
        stopIx = self.helixCtermSpinBox.value()
        startIx -= 1
        stopIx -= 1
        self.helixNtermSpinBox.setValue(startIx)
        self.helixNtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
        self.helixCtermSpinBox.setValue(stopIx)
        self.helixCtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
    
    def helixFindSelectedCAHelices(self):
        selectedResidues = self.currentChainModel.getSelection()
        helices = []
        for resNum in selectedResidues:
            secel = self.currentChainModel.getSecelByIndex(resNum)
            if secel.type == 'helix':
                if not secel in helices:
                    helices.append(secel)
        return helices
    
    def helixFlipButtonPress(self):
    	print 'helixFlipButtonPress'
        helices = self.helixFindSelectedCAHelices()
        chain = self.currentChainModel
        viewer = self.CAlphaViewer
        renderer = self.CAlphaViewer.renderer
        if len(helices) == 0:
            return
        elif len(helices) == 1:
            helix = helices[0]
            
            #We have to re-make the starting and ending bonds after a helix flip
            #Deleting the old starting and ending bonds
            atomStart0 = chain[helix.startIndex-1].getAtom('CA')
            atomStart1 = chain[helix.startIndex].getAtom('CA')
            if atomStart0 and atomStart1:
                startBondIndex = renderer.getBondIndex(atomStart0.getHashKey(), atomStart1.getHashKey())
                renderer.deleteBond(startBondIndex)
            #Must delete before we find the next index because all the indices after startBondIndex decrease by 1
            atomStop0 = chain[helix.stopIndex].getAtom('CA')
            atomStop1 = chain[helix.stopIndex+1].getAtom('CA')
            if atomStop0 and atomStop1:
                stopBondIndex = renderer.getBondIndex(atomStop0.getHashKey(), atomStop1.getHashKey())
                renderer.deleteBond(stopBondIndex)
            
            helix.flipHelix()
            
            atomStart0, atomStart1, atomStop0, atomStop1 = None, None, None, None
            
            #We now have to make new starting and ending bonds
 
            #We must re-assign these variables because the helix has been flipped
            atomStart0 = chain[helix.startIndex-1].getAtom('CA')
            atomStart1 = chain[helix.startIndex].getAtom('CA')
            if atomStart0 and atomStart1:
            	disp = atomStart1.getPosition() - atomStart0.getPosition()
                distance = vector_lib.vectorSize( (disp.x(), disp.y(), disp.z()) )
                if distance <= 4.2:
                    startBond = PDBBond()
                    startBond.setAtom0Ix(atomStart0.getHashKey())
                    stopBond.setAtom0Ix(atomStart1.getHashKey())
                    renderer.addBond(startBond)
 
            atomStop0 = chain[helix.stopIndex].getAtom('CA')
            atomStop1 = chain[helix.stopIndex+1].getAtom('CA')
            if atomStop0 and atomStop1:
                disp = atomStop0.getPosition() - atomStop1.getPosition()
                distance = vector_lib.vectorSize( (disp.x(), disp.y(), disp.z()) )
                if distance <= 4.2:
                    stopBond = PDBBond()
                    stopBond.setAtom0Ix(atomStop0.getHashKey())
                    stopBond.setAtom1Ix(atomStop1.getHashKey())
                    renderer.addBond(stopBond)
            self.CAlphaViewer.emitModelChanged()
        else:
            print 'more than one helix selected!'
            raise ValueError, len(helices)
            
    def helixIncreaseButtonPress(self):
        startIx = self.helixNtermSpinBox.value()
        stopIx = self.helixCtermSpinBox.value()
        startIx += 1
        stopIx += 1
        self.helixNtermSpinBox.setValue(startIx)
        self.helixNtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
        self.helixCtermSpinBox.setValue(stopIx)
        self.helixCtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
        
    def posMoveCM_x(self):
        oldX = self.x
        newX = self.posMoveDict['x'].value()
        moveX =  newX - oldX
        self.x = newX
        translateVector = Vector3DFloat(moveX, 0, 0)
        self.CAlphaViewer.renderer.selectionMove(translateVector)
        self.CAlphaViewer.emitModelChanged()
    def posMoveCM_y(self):
        oldY = self.y
        newY = self.posMoveDict['y'].value()
        moveY =  newY - oldY
        self.y = newY
        translateVector = Vector3DFloat(0, moveY, 0)
        self.CAlphaViewer.renderer.selectionMove(translateVector)
        self.CAlphaViewer.emitModelChanged()
    def posMoveCM_z(self):
        oldZ = self.z
        newZ = self.posMoveDict['z'].value()
        moveZ = newZ - oldZ
        self.z = newZ
        translateVector = Vector3DFloat(0, 0, moveZ)
        self.CAlphaViewer.renderer.selectionMove(translateVector)
        self.CAlphaViewer.emitModelChanged()
       
    def posRotateCM_roll(self, angle):
        print 'roll:', angle
        axis = self.CAlphaViewer.worldToObjectCoordinates(self.app.mainCamera.look)
        oldAngle = self.roll        
        
        axis = Vector3DFloat(axis[0], axis[1], axis[2])
        
        cm = self.CAlphaViewer.renderer.selectionCenterOfMass()
        newAngle = math.pi*angle/180
        self.CAlphaViewer.renderer.selectionRotate(cm, axis, newAngle-oldAngle)
        self.CAlphaViewer.emitModelChanged()
        
        self.roll = newAngle
    def posRotateCM_pitch(self, angle):
        print 'pitch:', angle
        axis = self.CAlphaViewer.worldToObjectCoordinates(self.app.mainCamera.right)
        oldAngle = self.pitch
        
        axis = Vector3DFloat(axis[0], axis[1], axis[2])
        
        cm = self.CAlphaViewer.renderer.selectionCenterOfMass()
        newAngle = math.pi*angle/180
        self.CAlphaViewer.renderer.selectionRotate(cm, axis, newAngle-oldAngle)
        self.CAlphaViewer.emitModelChanged()
        
        self.pitch = newAngle
    def posRotateCM_yaw(self, angle):
        print 'yaw:',  angle
        axis = self.CAlphaViewer.worldToObjectCoordinates(self.app.mainCamera.up)
        axis = (-1*axis[0], -1*axis[1], -1*axis[2])
        oldAngle = self.yaw
        
        axis = Vector3DFloat(axis[0], axis[1], axis[2])
        cm = self.CAlphaViewer.renderer.selectionCenterOfMass()
        newAngle = math.pi*angle/180
        self.CAlphaViewer.renderer.selectionRotate(cm, axis, newAngle-oldAngle)
        self.CAlphaViewer.emitModelChanged()
        
        self.yaw = newAngle

    def posUpdateValues(self):
        cAlphaRenderer = self.app.viewers['calpha'].renderer
        cm = cAlphaRenderer.selectionCenterOfMass()
        self.x = cm.x()
        self.y = cm.y()
        self.z = cm.z()
        self.posMoveDict['x'].setValue(cm.x())
        self.posMoveDict['y'].setValue(cm.y())
        self.posMoveDict['z'].setValue(cm.z())
            
    def posXDecr(self):
        self.posMoveDict['x'].setValue(self.posMoveDict['x'].value()-1)
    def posXIncr(self):
        self.posMoveDict['x'].setValue(self.posMoveDict['x'].value()+1)
    def posYDecr(self):
        self.posMoveDict['y'].setValue(self.posMoveDict['y'].value()-1)
    def posYIncr(self):
        self.posMoveDict['y'].setValue(self.posMoveDict['y'].value()+1)
    def posZDecr(self):
        self.posMoveDict['z'].setValue(self.posMoveDict['z'].value()-1)
    def posZIncr(self):
        self.posMoveDict['z'].setValue(self.posMoveDict['z'].value()+1)
    def posRollDecr(self):
        self.posMoveDict['roll'].setValue(self.posMoveDict['roll'].value()-3)
    def posRollIncr(self):
        self.posMoveDict['roll'].setValue(self.posMoveDict['roll'].value()+3)
    def posPitchDecr(self):
        self.posMoveDict['pitch'].setValue(self.posMoveDict['pitch'].value()-3)
    def posPitchIncr(self):
        self.posMoveDict['pitch'].setValue(self.posMoveDict['pitch'].value()+3)
    def posYawDecr(self):
        self.posMoveDict['yaw'].setValue(self.posMoveDict['yaw'].value()-3)
    def posYawIncr(self):
        self.posMoveDict['yaw'].setValue(self.posMoveDict['yaw'].value()+3)
    def removeSelectedAtoms(self):
        print 'helices', self.currentChainModel.helices.keys()
        print 'orphan strands', self.currentChainModel.orphanStrands.keys()
        print self.currentChainModel.secelList.keys()
        for resIndex in self.currentChainModel.getSelection():
            res = self.currentChainModel[resIndex]
            atom = res.getAtom('CA')
            if not atom:
                continue
            if atom.getSelected():
                #Check if it is in a Secel
                secel = self.currentChainModel.getSecelByIndex(resIndex)
                if secel.label != 'no-label':
                    self.currentChainModel.removeSecel(secel)
                prevAtom = self.currentChainModel[resIndex-1].getAtom('CA')
                nextAtom = self.currentChainModel[resIndex+1].getAtom('CA')
                if prevAtom:
                    #print 'There is a previous atom'
                    bondIndex = self.CAlphaViewer.renderer.getBondIndex(atom.getHashKey(), prevAtom.getHashKey())
                    if bondIndex:
                        #print 'removing bond before'
                        self.CAlphaViewer.renderer.deleteBond(bondIndex)
                if nextAtom:
                    #print 'There is a next atom'
                    bondIndex = self.CAlphaViewer.renderer.getBondIndex(atom.getHashKey(), nextAtom.getHashKey())
                    if bondIndex:
                        #print 'removing bond after'
                        self.CAlphaViewer.renderer.deleteBond(bondIndex)
                res.clearAtom('CA')
                #print res.getAtomNames()
                #print 'removing atom w/ resNum of:', atom.getResSeq()
                self.CAlphaViewer.renderer.deleteAtom(atom.getHashKey())
                del atom
        self.CAlphaViewer.emitModelChanged()
        print 'helices', self.currentChainModel.helices.keys()
        print 'orphan strands', self.currentChainModel.orphanStrands.keys()
        print self.currentChainModel.secelList.keys()

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
        print 'In setResidues'
        #newSelection is a list of Residue indeces that are selected
        if not newSelection:
            print 'In sequence_view.StructureEdit.setResidues().  The new selection is empty!'
            return
        #self.parentWidget() is SequenceWidget & self.parentWidget().parentWidget() is SequenceDock
        viewer = self.parentWidget().parentWidget().viewer
        for atom in self.possibleAtomsList:
            if atom is self.atomJustAdded: 
                continue
            else:
                viewer.renderer.deleteAtom(atom.getHashKey())
                viewer.emitModelChanged()
        self.possbileAtomsList = []
        self.atomJustAdded = None
        self.previouslySelectedPossibleAtom = None
        curResNum = newSelection[-1]
        
        print 'curResNum:', curResNum
        for i in self.atomicResNames.keys():
            try: 
                self.atomicResNames[i].setText(unicode(self.currentChainModel[curResNum+i]))
                self.atomicResNumbers[i].setText(unicode(curResNum+i))
            except (IndexError,  KeyError):
                self.atomicResNames[i].setText('')
                self.atomicResNumbers[i].setText('')
        self.atomFindPositionPossibilities()
        
    def setupAtomicTab(self):
        #These go in the atomic tab
        self.atomicPossibilityNumSpinBox = QtGui.QSpinBox()
        self.atomicPossibilityNumSpinBox.setRange(0, 0)
        self.atomicNumPossibilities = QtGui.QLabel('of ?')
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
        
        self.atomicBack1resButton = QtGui.QPushButton('<-')
        self.atomicForward1resButton = QtGui.QPushButton('->') 
        
        atomicLayout = QtGui.QVBoxLayout()
        
        atomicPossibilityLayout = QtGui.QHBoxLayout()
        atomicPossibilityLayout.setAlignment(QtCore.Qt.AlignHCenter)
        atomicPossibilityLayout.addWidget(self.atomicPossibilityNumSpinBox)
        atomicPossibilityLayout.addWidget(self.atomicNumPossibilities)
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
        atomicResButtonLayout.addWidget(self.atomicBack1resButton)
        atomicResButtonLayout.addWidget(self.atomicForward1resButton)
        atomicLayout.addLayout(atomicResButtonLayout)
        self.atomicTab.setLayout(atomicLayout)
    
    def setupHelixTab(self):
        #These go in the Helix tab
        self.helixModifyRadioButton = QtGui.QRadioButton(self.tr('Modify'))
        self.helixNewRadioButtion = QtGui.QRadioButton(self.tr('New'))
        NterminusLabel = QtGui.QLabel('N term')
        self.helixNtermResNameLabel = QtGui.QLabel('???')
        self.helixNtermSpinBox = QtGui.QSpinBox()
        if self.currentChainModel.residueRange():
            minIx = min(self.currentChainModel.residueRange())
            maxIx = max(self.currentChainModel.residueRange())
        else:
            minIx = 1
            maxIx = 10000
        self.helixNtermSpinBox.setRange(minIx, maxIx)
        CterminusLabel = QtGui.QLabel('C term')
        self.helixCtermResNameLabel = QtGui.QLabel('???')
        self.helixCtermSpinBox = QtGui.QSpinBox()
        self.helixCtermSpinBox.setRange(minIx, maxIx)
        
        self.helixDecreasePositionButton = QtGui.QPushButton('-')
        self.helixDecreasePositionButton.setMaximumWidth(30)
        positionLabel = QtGui.QLabel(self.tr('Position'))
        self.helixIncreasePositionButton = QtGui.QPushButton('+')
        self.helixIncreasePositionButton.setMaximumWidth(30)
        self.helixFlipButton = QtGui.QPushButton(self.tr('Flip'))
        
        radioLayout = QtGui.QHBoxLayout()
        radioLayout.addWidget(self.helixModifyRadioButton)
        radioLayout.addWidget(self.helixNewRadioButtion)
        radioLayout.addStretch()
        
        NtermLayout = QtGui.QHBoxLayout()
        NtermLayout.addWidget(NterminusLabel)
        NtermLayout.addWidget(self.helixNtermResNameLabel)
        NtermLayout.addWidget(self.helixNtermSpinBox)
        NtermLayout.addStretch()        
        
        CtermLayout = QtGui.QHBoxLayout()
        CtermLayout.addWidget(CterminusLabel)
        CtermLayout.addWidget(self.helixCtermResNameLabel)
        CtermLayout.addWidget(self.helixCtermSpinBox)
        CtermLayout.addStretch()
        
        positionLayout = QtGui.QHBoxLayout()
        positionLayout.addWidget(self.helixDecreasePositionButton)
        positionLayout.addWidget(positionLabel)
        positionLayout.addWidget(self.helixIncreasePositionButton)
        positionLayout.addStretch()

        flipLayout = QtGui.QHBoxLayout()
        flipLayout.addWidget(self.helixFlipButton)
        flipLayout.addStretch()
                
        helixLayout  = QtGui.QVBoxLayout()
        helixLayout.addLayout(radioLayout)
        helixLayout.addLayout(NtermLayout)
        helixLayout.addLayout(CtermLayout)
        helixLayout.addLayout(positionLayout)
        helixLayout.addLayout(flipLayout)
        self.helixTab.setLayout(helixLayout)
        
        #Disabling widgets that are not yet implemented
        self.helixModifyRadioButton.setEnabled(False)
        self.helixNewRadioButtion.setEnabled(False)
        
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
        
        #Disabling widgets that are not yet implemented
        self.loopAcceptButton.setEnabled(False)
        self.loopComboBox.setEnabled(False)
        self.loopFindButton.setEnabled(False)
        self.loopIDLabel.setEnabled(False)
        self.loopRejectButton.setEnabled(False)
        self.loopStartLabel.setEnabled(False)
        self.loopStartSpinBox.setEnabled(False)
        self.loopStopLabel.setEnabled(False)
        self.loopStopSpinBox.setEnabled(False)
    
    def setupPositionTab(self):
        self.posTranslateLabel = QtGui.QLabel('Translate')
        self.posRotateLabel = QtGui.QLabel('Rotate')
        self.posMoveLabelsDict = {
                              'x': QtGui.QLabel('x'), 
                              'y': QtGui.QLabel('y'), 
                              'z': QtGui.QLabel('z'), 
                              'roll': QtGui.QLabel('roll'), 
                              'pitch': QtGui.QLabel('pitch'), 
                              'yaw': QtGui.QLabel('yaw')
                              }
                              
        self.posMoveDict = {
                                   'x': QtGui.QDoubleSpinBox(), 
                                   'y': QtGui.QDoubleSpinBox(), 
                                   'z': QtGui.QDoubleSpinBox(), 
                                    'roll': QtGui.QSlider(), 
                                    'pitch': QtGui.QSlider(), 
                                    'yaw': QtGui.QSlider()
                                   }
        for key in ['x', 'y', 'z']:
            self.posMoveDict[key].setRange(-10000, 10000)
        for key in ['roll', 'pitch', 'yaw']:
            self.posMoveDict[key].setRange(-180, 180)
            self.posMoveDict[key].setOrientation(QtCore.Qt.Horizontal)
        
        self.posDecreaseButtonDict = {
                                   'x': QtGui.QPushButton('-'), 
                                   'y': QtGui.QPushButton('-'), 
                                   'z': QtGui.QPushButton('-'), 
                                   'roll': QtGui.QPushButton('-3'), 
                                   'pitch': QtGui.QPushButton('-3'), 
                                   'yaw': QtGui.QPushButton('-3')
                                   }
        self.posIncreaseButtonDict = {
                                   'x': QtGui.QPushButton('+'), 
                                   'y': QtGui.QPushButton('+'), 
                                   'z': QtGui.QPushButton('+'), 
                                   'roll': QtGui.QPushButton('+3'), 
                                   'pitch': QtGui.QPushButton('+3'), 
                                   'yaw': QtGui.QPushButton('+3')
                                   }
        for key in self.posDecreaseButtonDict.keys():
            self.posDecreaseButtonDict[key].setMaximumWidth(30)
            self.posIncreaseButtonDict[key].setMaximumWidth(30)

        posSpinLabelLayoutDict = {
                              'x': QtGui.QHBoxLayout(), 
                              'y': QtGui.QHBoxLayout(), 
                              'z': QtGui.QHBoxLayout(), 
                              'roll': QtGui.QHBoxLayout(), 
                              'pitch': QtGui.QHBoxLayout(), 
                              'yaw': QtGui.QHBoxLayout()
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
        positionLayout.addLayout(posSpinLabelLayoutDict['roll'], 5, 0)
        positionLayout.addLayout(posSpinLabelLayoutDict['pitch'], 6, 0)
        positionLayout.addLayout(posSpinLabelLayoutDict['yaw'], 7, 0)
        self.positionTab.setLayout(positionLayout)
        
        self.connect(self.posDecreaseButtonDict['x'], QtCore.SIGNAL('clicked()'), self.posXDecr)
        self.connect(self.posDecreaseButtonDict['y'], QtCore.SIGNAL('clicked()'), self.posYDecr)
        self.connect(self.posDecreaseButtonDict['z'], QtCore.SIGNAL('clicked()'), self.posZDecr)
        self.connect(self.posIncreaseButtonDict['x'], QtCore.SIGNAL('clicked()'), self.posXIncr)
        self.connect(self.posIncreaseButtonDict['y'], QtCore.SIGNAL('clicked()'), self.posYIncr)
        self.connect(self.posIncreaseButtonDict['z'], QtCore.SIGNAL('clicked()'), self.posZIncr)
        self.connect(self.posDecreaseButtonDict['roll'], QtCore.SIGNAL('clicked()'), self.posRollDecr)
        self.connect(self.posIncreaseButtonDict['roll'], QtCore.SIGNAL('clicked()'), self.posRollIncr)
        self.connect(self.posDecreaseButtonDict['pitch'], QtCore.SIGNAL('clicked()'), self.posPitchDecr)
        self.connect(self.posIncreaseButtonDict['pitch'], QtCore.SIGNAL('clicked()'), self.posPitchIncr)
        self.connect(self.posDecreaseButtonDict['yaw'], QtCore.SIGNAL('clicked()'), self.posYawDecr)
        self.connect(self.posIncreaseButtonDict['yaw'], QtCore.SIGNAL('clicked()'), self.posYawIncr)
        
        self.roll = 0
        self.pitch = 0
        self.yaw = 0
        
        
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
        
        buttonLayout = QtGui.QHBoxLayout()
        buttonLayout.addStretch()
        buttonLayout.addWidget(self.acceptButton)
        buttonLayout.addWidget(self.removeButton)
        buttonLayout.addStretch()
        
        leftLayout.addLayout(CAIntervalLayout)        
        leftLayout.addLayout(buttonLayout)
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
    
    def updateCurrentMatch(self, sseType, sseIndex):
        sseViewer = self.app.viewers['sse']
        if sseType == 0:
            print 'helix'
            corrLib = sseViewer.correspondenceLibrary
            currCorrIndex = corrLib.getCurrentCorrespondenceIndex()
            print 'currCorrIndex:',  currCorrIndex
            matchList = corrLib.correspondenceList[currCorrIndex].matchList
            for match in matchList:
                if match.observed.label == sseIndex: 
                    self.currentMatch = match
                    print self.currentMatch.predicted, self.currentMatch.observed
                    startIx = self.currentMatch.predicted.startIndex
                    stopIx = self.currentMatch.predicted.stopIndex
                    self.helixNtermSpinBox.setValue(startIx)
                    self.helixCtermSpinBox.setValue(stopIx)
                    self.helixNtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
                    self.helixCtermResNameLabel.setText(self.currentChainModel[stopIx].symbol3)
                    break            
        print 'Index:', sseIndex
    
    def updateSelectedResidues(self):
        print '\nIn updateSelectedResidues'
        if self.tabWidget.currentWidget() is self.atomicTab:
            self.setResidues(self.currentChainModel.getSelection())
        
class CommandAcceptAtomPlacement(QtGui.QUndoCommand):
        def __init__(self, currentChainModel, structureEditor, resSeqNum, chosenCoordinates, viewer, bondBefore=None, bondAfter=None, description=None):
            super(CommandAcceptAtomPlacement, self).__init__(description)
            self.currentChainModel = currentChainModel
            self.structureEditor = structureEditor
            self.resSeqNum = resSeqNum
            self.chosenCoordinates = chosenCoordinates
            self.viewer = viewer
            self.bondBefore = bondBefore
            self.bondAfter = bondAfter
        def redo(self):
            print self.chosenCoordinates
            raw = PDBAtom(self.currentChainModel.getPdbID(), self.currentChainModel.getChainID(), self.resSeqNum, 'CA')
            raw.setPosition(self.chosenCoordinates)
            atom = self.viewer.renderer.addAtom(raw) 
            print atom
            self.currentChainModel[self.resSeqNum].addAtomObject(atom)
            self.currentChainModel[self.resSeqNum].setCAlphaColorToDefault() 
            if self.resSeqNum - 1 in self.currentChainModel.residueRange():
                prevCAlpha = self.currentChainModel[self.resSeqNum - 1].getAtom('CA')
                if prevCAlpha:
                    print "adding a bond before"
                    self.bondBefore=PDBBond()
                    self.bondBefore.setAtom0Ix(prevCAlpha.getHashKey())
                    self.bondBefore.setAtom1Ix(atom.getHashKey())
            if self.resSeqNum + 1 in self.currentChainModel.residueRange():
                nextCAlpha = self.currentChainModel[self.resSeqNum + 1].getAtom('CA')
                if nextCAlpha:
                    print "adding a bond after"
                    self.bondAfter = PDBBond()
                    self.bondAfter.setAtom0Ix(nextCAlpha.getHashKey())
                    self.bondAfter.setAtom1Ix(atom.getHashKey())
            
            if self.bondBefore:
                self.viewer.renderer.addBond(self.bondBefore)
            if self.bondAfter:
                self.viewer.renderer.addBond(self.bondAfter)
            
            self.viewer.emitModelChanged()
            self.structureEditor.atomJustAdded = atom
            
            if self.structureEditor.atomicBackwardRadioButton.isChecked():
                self.structureEditor.atomPrevButtonPress()
            elif self.structureEditor.atomicForwardRadioButton.isChecked():
                self.structureEditor.atomNextButtonPress()
            
        def undo(self):
            print self.structureEditor.atomJustAdded
            atom = self.currentChainModel[self.resSeqNum].getAtom('CA')
            self.currentChainModel[self.resSeqNum].clearAtom('CA')
            self.viewer.renderer.deleteAtom(atom.getHashKey())
            
            if self.bondBefore:
                numBonds = self.viewer.renderer.getBondCount()
                self.viewer.renderer.deleteBond(numBonds-1)
                pass
            if self.bondAfter:
                numBonds = self.viewer.renderer.getBondCount()
                self.viewer.renderer.deleteBond(numBonds-1)
                pass
            self.viewer.emitModelChanged()
            
            if self.structureEditor.atomicBackwardRadioButton.isChecked():
                self.structureEditor.atomNextButtonPress()
            elif self.structureEditor.atomicForwardRadioButton.isChecked():
                self.structureEditor.atomPrevButtonPress()
            
if __name__ == '__main__':
    from seq_model.Chain import Chain
    import sys
    app = QtGui.QApplication(sys.argv)
    chain = Chain('', app)
    window = StructureEditor(chain)
    window.show()
    sys.exit(app.exec_())
