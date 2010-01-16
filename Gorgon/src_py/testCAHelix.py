#!/usr/bin/python
import sys
from libpyGORGON import PDBAtom,Vector3DFloat, PDBBond
from PyQt4 import QtGui
from window_manager import WindowManager
from main_window_form import MainWindowForm
from seq_model.Chain import Chain
from seq_model.findHelixCalphas import helixEndpointsToCAlphaPositions

# Create the application object required by Qt
app = QtGui.QApplication(sys.argv)

# Create Gorgon's main window and get a handle on the CAlpha Viewer
window = MainWindowForm("2.0")
window.addModule(WindowManager(window))
window.showMaximized()
cAlphaViewer=window.viewers['calpha']

startPos = (-20,-10,-5)
stopPos = (20,10,5)

# Instantiate an Atom
rawAtom=PDBAtom('AAAA', 'A', 1, 'CA')
rawAtom.setPosition(Vector3DFloat(*startPos))
rawAtom.setAtomRadius(2)
rawAtom.setColor(0,1,0,1)
rawAtom = cAlphaViewer.renderer.addAtom(rawAtom) #A new PDBAtom is returned by this function, and rawAtom needs to refer to it
    
# Instantiate a Second Atom
rawAtom2=PDBAtom('AAAA', 'A', 2, 'CA')
rawAtom2.setPosition(Vector3DFloat(*stopPos))
rawAtom2.setAtomRadius(2)
rawAtom2.setColor(1,0,0,1)
rawAtom2 = cAlphaViewer.renderer.addAtom(rawAtom2)

# Get coordinates for helix with axis starting and ending at startPos and stopPos
coordList = helixEndpointsToCAlphaPositions(startPos,stopPos)
atomList = []

# Create helix atoms and bonds
for i in xrange(len(coordList)):
    raw = PDBAtom('AAAA', 'A', i+3, 'CA')
    raw.setPosition(Vector3DFloat(coordList[i][0],coordList[i][1],coordList[i][2]))
    atomList.append( cAlphaViewer.renderer.addAtom(raw) )
    if i != 0:
        bond = PDBBond()
        bond.setAtom0Ix(atomList[-2].getHashKey())
        bond.setAtom1Ix(atomList[-1].getHashKey())
        cAlphaViewer.renderer.addBond(bond)

if not cAlphaViewer.loaded:
    cAlphaViewer.loaded = True
    cAlphaViewer.emitModelLoaded()

cAlphaViewer.emitViewerSetCenter()

sys.exit(app.exec_())
