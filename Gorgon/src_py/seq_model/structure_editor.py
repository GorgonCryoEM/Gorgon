# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Class: StructureEditor
# Class Description: Widget for placing atoms and secondary strucutre elements as well as for optimizing placement.

from PyQt4 import QtGui, QtCore
from libpyGORGON import PDBAtom, PDBBond, Vector3DFloat
from calpha_interactive_loop_builder import CAlphaInteractiveLoopBuilder
from seq_model.findHelixCalphas import helixEndpointsToCAlphaPositions
from seq_model.Helix import Helix
import math
from vector_lib import *

class StructureEditor(QtGui.QWidget):
    """
An instance of this class is a member of a SequenceWidget. It is used
for editing the chain model--atomic editor, helix editor, loop editor, 
position editor, etc.  
    """
    def __init__(self, currentChainModel, parent=None):
        super(StructureEditor, self).__init__(parent)
        
        if self.parentWidget().parentWidget().app:
            self.app = self.parentWidget().parentWidget().app        
        
        self.currentChainModel = currentChainModel
        self.builder = False
        self.atomJustAdded = None
        self.possibleAtomsList = []
        self.previouslySelectedPossibleAtom = None
        self.undoStack = QtGui.QUndoStack(self)
                       
        self.setupUi()
        self.enableDisable()
        
        self.connect(self.atomicBack1resButton, QtCore.SIGNAL('clicked()'), self.atomPrevButtonPress)
        self.connect(self.atomicForward1resButton, QtCore.SIGNAL('clicked()'), self.atomNextButtonPress)
        self.connect(self.acceptButton, QtCore.SIGNAL('clicked()'),  self.acceptButtonPress)
        self.connect(self.atomicPossibilityNumSpinBox,  QtCore.SIGNAL('valueChanged(int)'),  self.atomChoosePossibleAtom)
        self.connect(self.atomicForwardRadioButton,  QtCore.SIGNAL('toggled(bool)'), self.atomForwardBackwardChange)
        self.connect(self.undoButton, QtCore.SIGNAL('clicked()'), self.undoStack.undo)
        self.connect(self.redoButton, QtCore.SIGNAL('clicked()'), self.undoStack.redo)
        self.connect(self.helixDecreasePositionButton, QtCore.SIGNAL('clicked()'), self.helixDecreaseButtonPress)
        self.connect(self.helixIncreasePositionButton, QtCore.SIGNAL('clicked()'), self.helixIncreaseButtonPress)
        self.connect(self.helixFlipButton, QtCore.SIGNAL('clicked()'), self.helixFlipButtonPress)
        self.connect(self.currentChainModel, QtCore.SIGNAL('selection updated'), self.updateSelectedResidues)
        self.connect(self.tabWidget, QtCore.SIGNAL('currentChanged(int)'), self.enableDisable)
        if self.parentWidget().parentWidget().app:
            self.updateCurrentMatch() #In case an observed helix is already selected
            self.CAlphaViewer = self.app.viewers['calpha']                       
            self.connect(self.app.viewers['sse'], QtCore.SIGNAL("SSE selected"), self.updateCurrentMatch)
            self.connect(self.app.viewers["calpha"], QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.posUpdateValues)
            self.connect(self.posMoveDict['x'], QtCore.SIGNAL('valueChanged(double)'), self.posMoveCM_x)
            self.connect(self.posMoveDict['y'], QtCore.SIGNAL('valueChanged(double)'), self.posMoveCM_y)
            self.connect(self.posMoveDict['z'], QtCore.SIGNAL('valueChanged(double)'), self.posMoveCM_z)
            self.connect(self.posMoveDict['roll'], QtCore.SIGNAL('valueChanged(int)'), self.posRotateCM_roll)
            self.connect(self.posMoveDict['pitch'], QtCore.SIGNAL('valueChanged(int)'), self.posRotateCM_pitch)
            self.connect(self.posMoveDict['yaw'], QtCore.SIGNAL('valueChanged(int)'), self.posRotateCM_yaw)
            self.connect(self.removeButton, QtCore.SIGNAL('clicked()'), self.removeSelectedAtoms)
            self.connect(self.app.viewers['volume'], QtCore.SIGNAL("modelLoaded()"), self.updateLoopEditorEnables)
            self.connect(self.app.viewers['volume'], QtCore.SIGNAL("modelUnloaded()"), self.updateLoopEditorEnables)
      
    def acceptButtonPress(self):
        """
This is called when the accept button is pressed.  What it does depends
on which tab is active.  
        """
        print '\nAccept Button Pressed'
        currentWidget = self.tabWidget.currentWidget()
        if currentWidget is self.atomicTab:
            self.atomPlaceCAatom()
        elif currentWidget is self.helixTab:
            self.helixCreateCAhelix()
    
    def atomChoosePossibleAtom(self, choiceNum):
        """
This function highlights one of the possible atoms which will be chosen
if the user clicks accept.
        """
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
        """
This function places atoms at intersections with the skeleton that are
the indicated distance (self.CAdoubleSpinBox) from the C-alpha
atom of the selected residue.
        """
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
        """
This reponds to whether the atomic editor should be moving forward 
through the chain or backward.
        """
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
        """
This moves to the next residue and updates the selected residue.
        """
    	print 'atomNextButtonPress'
        currentChainModel = self.parentWidget().currentChainModel
        if currentChainModel.getSelection():
            newSelection = [ currentChainModel.getSelection()[-1] + 1 ]
            if newSelection[0] > max(currentChainModel.residueRange()): 
                return
            self.parentWidget().scrollable.seqView.setSequenceSelection(newSelection)
            #self.setResidues(newSelection)
    
    def atomPlaceCAatom(self):
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
            
    def atomPrevButtonPress(self):
        """
This moves to the previous residue and updates the selected residue.
        """
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
        """
This changes the atoms' properties back to default.
        """
        for index in chain.residueRange():
            res = chain[index]
            res.setCAlphaColorToDefault()
            res.setCAlphaSizeToDefault()
        #print "The mock side-chains should be cleared, but not yet drawn to the screen."
    
    def enableDisable(self):
        """
Depending on which tab is active, this enables and disables widtets.
This is used for not-yet-implemented and non-applicable widgets.
        """
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
            self.redoButton.setEnabled(True)
            self.undoButton.setEnabled(True)
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
        """
This creates a C-alpha helix between the C-alpha atoms from residues 
given by self.helixNtermSpinBox and self.helixCtermSpinBox.
        """
        print 'In helixCreateCAhelix'
        startIndex = self.helixNtermSpinBox.value()
        stopIndex = self.helixCtermSpinBox.value()
        observedHelix = self.app.viewers['sse'].currentMatch.observed
        direction = self.app.viewers['sse'].currentMatch.direction #Forward=0, Reverse=1
        predHelix = self.app.viewers['sse'].currentMatch.predicted
        if observedHelix.__class__.__name__ != 'ObservedHelix':
            raise TypeError, observedHelix.__class__.__name__
            
        moveStart = 1.5*(startIndex - predHelix.startIndex)
        print 'moveStart', moveStart
        moveEnd = 1.5*(stopIndex - predHelix.stopIndex)
        print 'moveEnd', moveEnd
        midpoint = observedHelix.getMidpoint()
        unitVector = observedHelix.getUnitVector()
        print 'unitVector', unitVector
        structPredCoord1 = vectorAdd( midpoint, vectorScalarMultiply(-1*predHelix.getLengthInAngstroms()/2, unitVector) )
        structPredCoord2 = vectorAdd( midpoint, vectorScalarMultiply(predHelix.getLengthInAngstroms()/2, unitVector) )
                
        if direction == 0:
            startMoveVector = vectorScalarMultiply( moveStart, unitVector)
            endMoveVector = vectorScalarMultiply( moveEnd, unitVector)
            coord1 = vectorAdd(structPredCoord1, startMoveVector)
            coord2 = vectorAdd(structPredCoord2, endMoveVector)
        elif direction == 1:
            startMoveVector = vectorScalarMultiply( -1*moveStart, unitVector)
            endMoveVector = vectorScalarMultiply( -1*moveEnd, unitVector)
            coord1 = vectorAdd(structPredCoord1, endMoveVector)
            coord2 = vectorAdd(structPredCoord2, startMoveVector)
                
        command = CommandPlaceHelix(self.currentChainModel, predHelix, startIndex, stopIndex, coord1, coord2, self, description = "Create C-alpha helix")
        self.undoStack.push(command)        

    def helixDecreaseButtonPress(self):
        """
This decreases the start and stop residue numbers by one.
        """
        startIx = self.helixNtermSpinBox.value()
        stopIx = self.helixCtermSpinBox.value()
        startIx -= 1
        stopIx -= 1
        self.helixNtermSpinBox.setValue(startIx)
        self.helixNtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
        self.helixCtermSpinBox.setValue(stopIx)
        self.helixCtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
    
    def helixFindSelectedCAHelices(self):
        """
This finds C-alpha helices that contain the selected residue numbers.
        """
        selectedResidues = self.currentChainModel.getSelection()
        helices = []
        for resNum in selectedResidues:
            secel = self.currentChainModel.getSecelByIndex(resNum)
            if secel.type == 'helix':
                if not secel in helices:
                    helices.append(secel)
        return helices
    
    def helixFlipButtonPress(self):
        """
This flips the direction of a C-alpha helix. Bonds from the ends of the
helix to atoms outside the helix are removed during the flip, and new 
bonds are created only if the length of the new bond would be <= 4.2 A.
        """
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
                distance = vectorSize( (disp.x(), disp.y(), disp.z()) )
                if distance <= 4.2:
                    startBond = PDBBond()
                    startBond.setAtom0Ix(atomStart0.getHashKey())
                    stopBond.setAtom0Ix(atomStart1.getHashKey())
                    renderer.addBond(startBond)
 
            atomStop0 = chain[helix.stopIndex].getAtom('CA')
            atomStop1 = chain[helix.stopIndex+1].getAtom('CA')
            if atomStop0 and atomStop1:
                disp = atomStop0.getPosition() - atomStop1.getPosition()
                distance = vectorSize( (disp.x(), disp.y(), disp.z()) )
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
        """
This increases the start and stop residue numbers by one.
        """
        startIx = self.helixNtermSpinBox.value()
        stopIx = self.helixCtermSpinBox.value()
        startIx += 1
        stopIx += 1
        self.helixNtermSpinBox.setValue(startIx)
        self.helixNtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
        self.helixCtermSpinBox.setValue(stopIx)
        self.helixCtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
        
    def posMoveCM_x(self):
        """
This translates the selection on the x-axis.
        """
        oldX = self.x
        newX = self.posMoveDict['x'].value()
        moveX =  newX - oldX
        self.x = newX
        translateVector = Vector3DFloat(moveX, 0, 0)
        self.CAlphaViewer.renderer.selectionMove(translateVector)
        self.CAlphaViewer.emitModelChanged()
    def posMoveCM_y(self):
        """
This translates the selection on the y-axis.
        """
        oldY = self.y
        newY = self.posMoveDict['y'].value()
        moveY =  newY - oldY
        self.y = newY
        translateVector = Vector3DFloat(0, moveY, 0)
        self.CAlphaViewer.renderer.selectionMove(translateVector)
        self.CAlphaViewer.emitModelChanged()
    def posMoveCM_z(self):
        """
This translates the selection on the z-axis.
        """
        oldZ = self.z
        newZ = self.posMoveDict['z'].value()
        moveZ = newZ - oldZ
        self.z = newZ
        translateVector = Vector3DFloat(0, 0, moveZ)
        self.CAlphaViewer.renderer.selectionMove(translateVector)
        self.CAlphaViewer.emitModelChanged()
       
    def posRotateCM_roll(self, angle):
        """
This rotates the selection around its 'center of mass' (actually 
geometric center) in a clockwise direction around a normal line to the
screen.
        """
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
        """
This rotates the selection around its 'center of mass' (actually 
geometric center) around a line parallel to a horizontal line on the
screen.
        """
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
        """
This rotates the selection around its 'center of mass' (actually
geometric center) around a line parallel to a vertical line on the
screen.
        """
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
        """
This updates the spin boxes to show the C-alpha coordinates of the 
selection's geometric center.
        """
        cAlphaRenderer = self.app.viewers['calpha'].renderer
        cm = cAlphaRenderer.selectionCenterOfMass()
        self.x = cm.x()
        print cm.x()
        self.y = cm.y()
        print cm.y()
        self.z = cm.z()
        print cm.z()
        self.posMoveDict['x'].setValue(cm.x())
        self.posMoveDict['y'].setValue(cm.y())
        self.posMoveDict['z'].setValue(cm.z())
            
    def posXDecr(self):
        """
This decreases the position editor's x-coordinate spin box by 1.
        """
        self.posMoveDict['x'].setValue(self.posMoveDict['x'].value()-1)
    def posXIncr(self):
        """
This increases the position editor's x-cordinate spin box by 1.
        """
        self.posMoveDict['x'].setValue(self.posMoveDict['x'].value()+1)
    def posYDecr(self):
        """
This decreases the position editor's y-coordinate spin box by 1.
        """
        self.posMoveDict['y'].setValue(self.posMoveDict['y'].value()-1)
    def posYIncr(self):
        """
This increases the position editor's y-coordinate spin box by 1.
        """
        self.posMoveDict['y'].setValue(self.posMoveDict['y'].value()+1)
    def posZDecr(self):
        """
This decreases the position editor's z-coordinate spin box by 1.
        """
        self.posMoveDict['z'].setValue(self.posMoveDict['z'].value()-1)
    def posZIncr(self):
        """
This increases the position editor's z-coordinate spin box by 1.
        """
        self.posMoveDict['z'].setValue(self.posMoveDict['z'].value()+1)
    def posRollDecr(self):
        """
This decreases the position editor's roll spin box by 3.
        """
        self.posMoveDict['roll'].setValue(self.posMoveDict['roll'].value()-3)
    def posRollIncr(self):
        """
This increases the position editor's roll spin box by 3.
        """
        self.posMoveDict['roll'].setValue(self.posMoveDict['roll'].value()+3)
    def posPitchDecr(self):
        """
This decreases the position editor's pitch spin box by 3.
        """
        self.posMoveDict['pitch'].setValue(self.posMoveDict['pitch'].value()-3)
    def posPitchIncr(self):
        """
This increases the position editor's pitch spin box by 3.
        """
        self.posMoveDict['pitch'].setValue(self.posMoveDict['pitch'].value()+3)
    def posYawDecr(self):
        """
This decreases the position editor's yaw spin box by 3.
        """
        self.posMoveDict['yaw'].setValue(self.posMoveDict['yaw'].value()-3)
    def posYawIncr(self):
        """
This increases the position editor's yaw spin box by 3.
        """
        self.posMoveDict['yaw'].setValue(self.posMoveDict['yaw'].value()+3)
    
    def removeSelectedAtoms(self):
        """
This deletes the selected atoms and the attached bonds. It also removes
any secels that contain those atoms from the chain.
        """
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
        """
This sets the colors and sizes of the spheres that represent mock
sidechains but does not update the screen.
        """
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
        #print "The mock side-chains should be ready to draw to the screen"

    def setResidues(self, newSelection):
        """
This takes a list of residues and chooses the last item of the list to
be the current residue for the atomic editor.  
        """
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
        
    def setLoopEditorValues(self, newSelection):
        if(newSelection):
            self.loopStartSpinBox.setValue(newSelection[0])
            self.loopStopSpinBox.setValue(newSelection[-1])
            if(self.builder):
                self.builder.setLoopAtoms(newSelection[0], newSelection[-1])
        else :
            self.loopStartSpinBox.setValue(0)
            self.loopStopSpinBox.setValue(0)            
    
    def updateLoopEditorEnables(self):
        print "in updateLoopEditorEnables"
        volumeViewer = self.app.viewers['volume']
        
        self.loopVolumeLoadButton.setVisible(not volumeViewer.loaded)        
        self.loopVolumeLoadedLabel.setVisible(not volumeViewer.loaded)

        self.loopStartEndBuildingButton.setEnabled(volumeViewer.loaded)
        self.loopStartLabel.setEnabled(volumeViewer.loaded)
        self.loopStartSpinBox.setEnabled(volumeViewer.loaded)
        self.loopStopLabel.setEnabled(volumeViewer.loaded)
        self.loopStopSpinBox.setEnabled(volumeViewer.loaded)
            
    def startEndLoopBuilding(self):
        self.loopBuildingStarted = not self.loopBuildingStarted
        
        if(self.loopBuildingStarted):
            self.loopStartEndBuildingButton.setText('End Loop Placement')
            self.setCursor(QtCore.Qt.BusyCursor)
            self.builder = CAlphaInteractiveLoopBuilder(self.app, self.currentChainModel)
            self.builder.setLoopAtoms(self.loopStartSpinBox.value(), self.loopStopSpinBox.value())
            self.setCursor(QtCore.Qt.ArrowCursor)                       
        else:
            self.loopStartEndBuildingButton.setText('Start Loop Placement')
            del self.builder
            self.builder = False
            
    def getLoopLength(self):
        return self.loopStopSpinBox.value() - self.loopStartSpinBox.value()
            
    
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
        
        termLayout = QtGui.QGridLayout()
        termLayout.addWidget(NterminusLabel, 0, 0, 1, 1)
        termLayout.addWidget(self.helixNtermResNameLabel, 0, 1, 1, 1)
        termLayout.addWidget(self.helixNtermSpinBox, 0, 2, 1, 1)
        
        termLayout.addWidget(CterminusLabel, 1, 0, 1, 1)
        termLayout.addWidget(self.helixCtermResNameLabel, 1, 1, 1, 1)
        termLayout.addWidget(self.helixCtermSpinBox, 1, 2, 1, 1)
                
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
        helixLayout.addLayout(termLayout)
        helixLayout.addLayout(positionLayout)
        helixLayout.addLayout(flipLayout)
        self.helixTab.setLayout(helixLayout)
        
        #Disabling widgets that are not yet implemented
        self.helixModifyRadioButton.setEnabled(False)
        self.helixNewRadioButtion.setEnabled(False)
    
   
    def setupLoopTab(self):    
        self.loopBuildingStarted = False    

        self.loopVolumeLoadedLabel = QtGui.QLabel('Volume not loaded.  Please load a volume to place loops.')
        self.loopVolumeLoadButton = QtGui.QPushButton('Load Volume')
        self.loopStartEndBuildingButton = QtGui.QPushButton('Start Loop Placement')
        
        self.loopStartLabel = QtGui.QLabel('Start Residue:')
        self.loopStartSpinBox = QtGui.QSpinBox()
        self.loopStartSpinBox.setMaximum(10000)
        self.loopStopLabel = QtGui.QLabel('Stop Residue:')
        self.loopStopSpinBox = QtGui.QSpinBox()
        self.loopStopSpinBox.setMaximum(10000)
                
        loopLayout = QtGui.QGridLayout()
        loopLayout.addWidget(self.loopVolumeLoadedLabel, 0, 0, 1, 2)
        loopLayout.addWidget(self.loopVolumeLoadButton, 1, 0, 1, 1)
        loopLayout.addWidget(self.loopStartEndBuildingButton, 2, 0, 1, 1)
        loopLayout.addWidget(self.loopStartLabel, 3, 0, 1, 1)
        loopLayout.addWidget(self.loopStartSpinBox, 3, 1, 1, 1)
        loopLayout.addWidget(self.loopStopLabel, 4, 0, 1, 1)
        loopLayout.addWidget(self.loopStopSpinBox, 4, 1, 1, 1)
        self.loopTab.setLayout(loopLayout)
        
        self.connect(self.loopVolumeLoadButton, QtCore.SIGNAL('clicked()'), self.app.viewers['volume'].loadData)
        self.connect(self.loopStartEndBuildingButton, QtCore.SIGNAL('clicked()'), self.startEndLoopBuilding)
        
        self.updateLoopEditorEnables()
        
    def setupPositionTab(self):
        self.posTranslateGroup = QtGui.QGroupBox('Translate:')
        self.posRotateGroup = QtGui.QGroupBox('Rotate:')
        self.posMoveLabelsDict = {
                              'x': QtGui.QLabel('X:'), 
                              'y': QtGui.QLabel('Y:'), 
                              'z': QtGui.QLabel('Z:'), 
                              'roll': QtGui.QLabel('Roll:'), 
                              'pitch': QtGui.QLabel('Pitch:'), 
                              'yaw': QtGui.QLabel('Yaw:')
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
        
        def addElement(key, row, layout):
            layout.addWidget(self.posMoveLabelsDict[key], row, 0, 1, 1)
            layout.addWidget(self.posDecreaseButtonDict[key], row, 1, 1, 1)
            layout.addWidget(self.posMoveDict[key], row, 2, 1, 1)
            layout.addWidget(self.posIncreaseButtonDict[key], row, 3, 1, 1)
         
        translateLayout = QtGui.QGridLayout() 
        addElement('x', 0, translateLayout)
        addElement('y', 1, translateLayout)
        addElement('z', 2, translateLayout)
        self.posTranslateGroup.setLayout(translateLayout)
        
        rotateLayout = QtGui.QGridLayout() 
        addElement('roll', 0, rotateLayout)
        addElement('pitch', 1, rotateLayout)
        addElement('yaw', 2, rotateLayout)    
        self.posRotateGroup.setLayout(rotateLayout)
                            
        positionLayout = QtGui.QVBoxLayout()
        positionLayout.addWidget(self.posTranslateGroup)
        positionLayout.addWidget(self.posRotateGroup)
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
        #These go on the left hand side
        self.undoButton = QtGui.QPushButton('Undo')
        self.redoButton = QtGui.QPushButton('Redo')
        self.CAdoubleSpinBox = QtGui.QDoubleSpinBox()
        self.CAdoubleSpinBox.setValue(3.8)
        self.CAlabel = QtGui.QLabel('C-Alpha Interval')
        self.mockSidechainsCheckBox = QtGui.QCheckBox('Mock Sidechains')
        self.acceptButton = QtGui.QPushButton('Accept')
        self.removeButton = QtGui.QPushButton('Remove')        
        
        self.tabWidget = QtGui.QTabWidget()
        self.helixTab = QtGui.QWidget()
        self.atomicTab = QtGui.QWidget()
        self.loopTab = QtGui.QWidget()
        self.positionTab = QtGui.QWidget()
        
        resNameFont = QtGui.QFont(self.font())
        resNameFont.setPointSize(41)
        resIndexFont = QtGui.QFont(self.font())
        resIndexFont.setPointSize(13)
        
        layout = QtGui.QVBoxLayout()
        
        bottomLayout = QtGui.QGridLayout()
                
        bottomLayout.addWidget(self.undoButton, 0, 0, 1, 1)
        bottomLayout.addWidget(self.redoButton, 0, 1, 1, 1)        
        bottomLayout.addWidget(self.mockSidechainsCheckBox, 1, 0, 1, 2)
        bottomLayout.addWidget(self.CAlabel, 2, 0, 1, 1)
        bottomLayout.addWidget(self.CAdoubleSpinBox, 2, 1, 1, 1)
        bottomLayout.addWidget(self.acceptButton, 3, 1)
        bottomLayout.addWidget(self.removeButton, 3, 2)
        
        self.tabWidget.addTab(self.helixTab, self.tr('Helix Editor'))
        self.tabWidget.addTab(self.atomicTab, self.tr('Atomic Editor'))
        self.tabWidget.addTab(self.loopTab, self.tr('Loop Editor'))
        self.tabWidget.addTab(self.positionTab, self.tr('Position'))
    
        self.setupHelixTab()
        self.setupAtomicTab()
        self.setupLoopTab()
        self.setupPositionTab()

        layout.addWidget(self.tabWidget)
        layout.addLayout(bottomLayout)
        self.setLayout(layout)
    
    def updateCurrentMatch(self):
        """
This uses the SSE viewer's currentMatch attribute to find the start and
stop indices for the current secel.  It uses this to set the Nterm and 
Cterm spin boxes in the helix editor.  
        """
        sseViewer = self.app.viewers['sse']
        if not sseViewer.currentMatch: 
            return
        startIx = sseViewer.currentMatch.predicted.startIndex
        stopIx = sseViewer.currentMatch.predicted.stopIndex
        #TODO: check on whether the current match is a helix or a strand
        self.helixNtermSpinBox.setValue(startIx)
        self.helixCtermSpinBox.setValue(stopIx)
        self.helixNtermResNameLabel.setText(self.currentChainModel[startIx].symbol3)
        self.helixCtermResNameLabel.setText(self.currentChainModel[stopIx].symbol3)
    '''
    #This is the old version of this function
    def updateCurrentMatch(self, sseType, sseIndex):
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
    '''
    def updateSelectedResidues(self):
        """
This gets the selected residues from the current chain model, and sends
that list of residue indices to self.setResidues to update the current 
residue in the atomic editor. It also updates the positions in the
position editor.
        """
        print '\nIn updateSelectedResidues'
        selection = self.currentChainModel.getSelection()
        self.setLoopEditorValues(selection)
        self.setResidues(selection)
        self.posUpdateValues()
        
class CommandAcceptAtomPlacement(QtGui.QUndoCommand):
    """
This class creates the QUndoCommand objects for the undo/redo stack.
    """
    def __init__(self, currentChainModel, structureEditor, resSeqNum, chosenCoordinates, viewer, description=None):
        super(CommandAcceptAtomPlacement, self).__init__(description)
        self.currentChainModel = currentChainModel
        self.structureEditor = structureEditor
        self.resSeqNum = resSeqNum
        self.chosenCoordinates = chosenCoordinates
        self.viewer = viewer
    def redo(self):
        """
In addition to being called to redo an action, this is called the first
time the action occurs.
        """
        print self.chosenCoordinates
        raw = PDBAtom(self.currentChainModel.getPdbID(), self.currentChainModel.getChainID(), self.resSeqNum, 'CA')
        raw.setPosition(self.chosenCoordinates)
        atom = self.viewer.renderer.addAtom(raw) 
        print atom
        self.currentChainModel[self.resSeqNum].addAtomObject(atom)
        self.currentChainModel[self.resSeqNum].setCAlphaColorToDefault() 
        bondBefore = None
        bondAfter = None
        if self.resSeqNum - 1 in self.currentChainModel.residueRange():
            prevCAlpha = self.currentChainModel[self.resSeqNum - 1].getAtom('CA')
            if prevCAlpha:
                print "adding a bond before"
                bondBefore=PDBBond()
                bondBefore.setAtom0Ix(prevCAlpha.getHashKey())
                bondBefore.setAtom1Ix(atom.getHashKey())
        if self.resSeqNum + 1 in self.currentChainModel.residueRange():
            nextCAlpha = self.currentChainModel[self.resSeqNum + 1].getAtom('CA')
            if nextCAlpha:
                print "adding a bond after"
                bondAfter = PDBBond()
                bondAfter.setAtom0Ix(nextCAlpha.getHashKey())
                bondAfter.setAtom1Ix(atom.getHashKey())
        
        if bondBefore:
            self.viewer.renderer.addBond(bondBefore)
        if bondAfter:
            self.viewer.renderer.addBond(bondAfter)
        
        self.viewer.emitModelChanged()
        self.structureEditor.atomJustAdded = atom
        
        if self.structureEditor.atomicBackwardRadioButton.isChecked():
            self.structureEditor.atomPrevButtonPress()
        elif self.structureEditor.atomicForwardRadioButton.isChecked():
            self.structureEditor.atomNextButtonPress()
        
    def undo(self):
        print self.structureEditor.atomJustAdded
        atom = self.currentChainModel[self.resSeqNum].getAtom('CA')
        
        try:
            atomBefore = self.currentChainModel[self.resSeqNum-1].getAtom('CA')
        except (IndexError, AttributeError, KeyError):
            atomBefore = None
        try:
            atomAfter = self.currentChainModel[self.resSeqNum+1].getAtom('CA')
        except (IndexError, AttributeError, KeyError):
            atomAfter = None
        
        if atomBefore:
            bondBeforeIx = self.viewer.renderer.getBondIndex(atomBefore.getHashKey(), atom.getHashKey())
            self.viewer.renderer.deleteBond(bondBeforeIx)
            pass
        if atomAfter:
            bondAfterIx = self.viewer.renderer.getBondIndex(atomAfter.getHashKey(), atom.getHashKey())
            self.viewer.renderer.deleteBond(bondAfterIx)
            pass
            
        self.currentChainModel[self.resSeqNum].clearAtom('CA')
        self.viewer.renderer.deleteAtom(atom.getHashKey())
        
        self.viewer.emitModelChanged()
        
        if self.structureEditor.atomicBackwardRadioButton.isChecked():
            self.structureEditor.atomNextButtonPress()
        elif self.structureEditor.atomicForwardRadioButton.isChecked():
            self.structureEditor.atomPrevButtonPress()
#command = CommandPlaceHelix(currentChainModel, predHelix, startIndex, stopIndex, coord1, coord2, self, description = "Create C-alpha helix")
class CommandPlaceHelix(QtGui.QUndoCommand):
    """
This class creates the QUndoCommand objects for the undo/redo stack.
    """
    def __init__(self, currentChainModel, predHelix, startIndex, stopIndex, coord1, coord2, structureEditor, description=None):
        super(CommandPlaceHelix, self).__init__(description)
        self.currentChainModel = currentChainModel
        self.predHelix = predHelix
        self.startIndex = startIndex
        self.stopIndex = stopIndex
        self.coord1 = coord1
        self.coord2 = coord2
        self.structureEditor = structureEditor
        self.CAlphaViewer = self.structureEditor.CAlphaViewer
    
    def redo(self):
        self.helix = Helix(self.currentChainModel, self.predHelix.serialNo, self.predHelix.label, self.startIndex, self.stopIndex)
        self.currentChainModel.addHelix(self.predHelix.serialNo, self.helix)
        self.helix.setAxisPoints(self.coord1, self.coord2)
        
        helixCoordList = helixEndpointsToCAlphaPositions(self.coord1, self.coord2)
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
            residue = self.currentChainModel[self.startIndex+i]
            rawAtom = residue.addAtom('CA', pos[0], pos[1], pos[2], 'C')
            atom = self.CAlphaViewer.renderer.addAtom(rawAtom)
            residue.addAtomObject(atom)
            atom.setSelected(True)
            try:
                prevAtom = self.currentChainModel[self.startIndex+i-1].getAtom('CA')
                bond = PDBBond()
                bond.setAtom0Ix(prevAtom.getHashKey())
                bond.setAtom1Ix(atom.getHashKey())
                self.CAlphaViewer.renderer.addBond(bond)
            except (KeyError, IndexError, AttributeError):
                continue

        try:
            nextAtom = self.currentChainModel[self.startIndex+len(helixCoordList)]
            bond = PDBBond()
            bond.setAtom0Ix(atom.getHashKey())
            bond.setAtom1Ix(nextAtom.getHashKey())
            self.CAlphaViewer.renderer.addBond(bond)
        except (KeyError, IndexError, AttributeError):
            pass
        
        self.currentChainModel.setSelection(newSelection = range(self.startIndex, 1+self.stopIndex))
        print self.helix
        
        if not self.CAlphaViewer.loaded:
            self.CAlphaViewer.loaded = True
            self.CAlphaViewer.emitModelLoaded()
        else:
            self.CAlphaViewer.emitModelChanged()
        
    def undo(self):
        for resNum in range(self.startIndex, 1+self.stopIndex):
            print 'first helix undo for loop'
            
            try:
                atom = self.currentChainModel[resNum].getAtom('CA')
            except (KeyError, IndexError, AttributeError):
                atom = None
            if atom:
                
                try:
                    atomBefore = self.currentChainModel[resNum-1].getAtom('CA')
                except (KeyError, IndexError, AttributeError):
                    atomBefore = None
                if atomBefore:
                    bondBeforeIx = self.CAlphaViewer.renderer.getBondIndex(atomBefore.getHashKey(), atom.getHashKey())
                    if bondBeforeIx != -1:
                        self.CAlphaViewer.renderer.deleteBond(bondBeforeIx)
                
                try:
                    atomAfter = self.currentChainModel[resNum+1].getAtom('CA')
                except (KeyError, IndexError, AttributeError):
                    atomAfter = None
                if atomAfter:
                    bondAfterIx = self.CAlphaViewer.renderer.getBondIndex(atomAfter.getHashKey(), atom.getHashKey())
                    if bondAfterIx != -1:
                        self.CAlphaViewer.renderer.deleteBond(bondAfterIx)
        
        for resNum in range(self.startIndex, 1+self.stopIndex):
            print 'second helix undo for loop'
            try:
                atom = self.currentChainModel[resNum].getAtom('CA')
            except (IndexError, AttributeError, KeyError):
                continue
            self.CAlphaViewer.renderer.deleteAtom(atom.getHashKey())                
            self.currentChainModel[resNum].clearAtom('CA')
        
        print 'finished both helix undo for loops'
        self.currentChainModel.removeSecel(self.helix)
        
        if not self.CAlphaViewer.loaded:
            self.CAlphaViewer.loaded = True
            self.CAlphaViewer.emitModelLoaded()
        else:
            self.CAlphaViewer.emitModelChanged()
        print 'finished helix undo'

if __name__ == '__main__':
    from seq_model.Chain import Chain
    import sys
    app = QtGui.QApplication(sys.argv)
    chain = Chain('', app)
    window = StructureEditor(chain)
    window.show()
    sys.exit(app.exec_())
