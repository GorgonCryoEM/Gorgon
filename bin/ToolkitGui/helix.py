from PyQt4 import QtGui, QtCore
from libpytoolkit import PDBAtom, PDBBond
from Toolkit.sse.seq_model.Helix import Helix
from Toolkit.sse.seq_model.findHelixCalphas import helixEndpointsToCAlphaPositions


def place_helix(calphaRenderer, currentChainModel, predHelix, startIndex, stopIndex, coord1, coord2):
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

    if not CAlphaViewer.loaded:
        CAlphaViewer.loaded = True
    #             self.CAlphaViewer.modelChanged()
    #         else:
    #             self.CAlphaViewer.modelChanged()
