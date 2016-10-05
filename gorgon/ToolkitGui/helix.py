from PyQt4 import QtGui, QtCore
from ..toolkit.libpytoolkit import PDBAtom, PDBBond

from explorer import Vec3
from ..toolkit.sse.seq_model.Helix import Helix
from ..toolkit.sse.seq_model.findHelixCalphas import helixEndpointsToCAlphaPositions


def place_helix(calphaRenderer, currentChainModel, predHelix, coord1, coord2):
    startIndex = predHelix.startIndex
    stopIndex  = predHelix.stopIndex
    
    helix = Helix(currentChainModel, predHelix.serialNo, predHelix.label, startIndex, stopIndex)

    currentChainModel.addHelix(predHelix.serialNo, helix)
    helix.setAxisPoints(coord1, coord2)

    helixCoordList = helixEndpointsToCAlphaPositions(coord1, coord2)

    for i in range(min(len(helixCoordList), stopIndex - startIndex + 1)):
        pos = helixCoordList[i]
        residue = currentChainModel[startIndex + i]
        rawAtom = residue.addAtom('CA', pos[0], pos[1], pos[2], 'C')
        atom = calphaRenderer.addAtom(rawAtom)
        residue.addAtomObject(atom)
        atom.setSelected(True)
        try:
            prevAtom = currentChainModel[startIndex + i - 1].getAtom('CA')
            bond = PDBBond()
            bond.setAtom0Ix(prevAtom.getHashKey())
            bond.setAtom1Ix(atom.getHashKey())
            calphaRenderer.addBond(bond)
        except (KeyError, IndexError, AttributeError):
            continue

    try:
        nextAtom = currentChainModel[startIndex + len(helixCoordList)]
        bond = PDBBond()
        bond.setAtom0Ix(atom.getHashKey())
        bond.setAtom1Ix(nextAtom.getHashKey())
        calphaRenderer.addBond(bond)
    except (KeyError, IndexError, AttributeError):
        pass

    currentChainModel.setSelection(newSelection=range(startIndex, 1 + stopIndex))


def create_helix(calphaRenderer, currentChainModel, currentMatch):
    observedHelix  = currentMatch.observed
    direction      = currentMatch.direction
    predHelix      = currentMatch.predicted
    
    startIndex = predHelix.startIndex
    stopIndex  = predHelix.stopIndex

    print "Helix Create"
    """
    This creates a C-alpha helix between the C-alpha atoms from residues
    given by self.helixNtermSpinBox and self.helixCtermSpinBox.
    """
    if observedHelix.__class__.__name__ != 'ObservedHelix':
        raise TypeError, observedHelix.__class__.__name__

    moveStart = 1.5*(startIndex - predHelix.startIndex)
    moveEnd = 1.5*(stopIndex - predHelix.stopIndex)
    midpoint   = observedHelix.getMidpoint()
    unitVector = observedHelix.getUnitVector()
    coord1 = midpoint - unitVector * predHelix.getLengthInAngstroms()/2
    coord2 = midpoint + unitVector * predHelix.getLengthInAngstroms()/2

    if direction == 0:
        startMoveVector = unitVector * moveStart
        endMoveVector   = unitVector * moveEnd
    elif direction == 1:
        startMoveVector = -unitVector * moveStart
        endMoveVector   = -unitVector * moveEnd

    coord1 = coord1 + endMoveVector
    coord2 = coord2 + startMoveVector

    place_helix(calphaRenderer, currentChainModel, predHelix, coord1, coord2)
