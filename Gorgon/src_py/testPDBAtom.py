#!/usr/bin/python
import sys
from libpyGORGON import PDBAtom,Vector3DFloat, PDBBond
from PyQt4 import QtGui
from window_manager import WindowManager
from main_window_form import MainWindowForm
from seq_model.Chain import Chain

# Create the application object required by Qt
app = QtGui.QApplication(sys.argv)

# Create Gorgon's main window and get a handle on the CAlpha Viewer
window = MainWindowForm("2.0")
window.addModule(WindowManager(window))
window.showMaximized()
cAlphaViewer=window.viewers['calpha']

# Instantiate an Atom
rawAtom=PDBAtom('AAAA', 'A', 1, 'CA')
rawAtom.setPosition(Vector3DFloat(-.5,-0.5,-0.5))
rawAtom = cAlphaViewer.renderer.addAtom(rawAtom) #A new PDBAtom is returned by this function, and rawAtom needs to refer to it
    
# Instantiate an Second Atom
rawAtom2=PDBAtom('AAAA', 'A', 2, 'CA')
rawAtom2.setPosition(Vector3DFloat(.5,0.5,0.5))
rawAtom2 = cAlphaViewer.renderer.addAtom(rawAtom2)

# Confirm that these accessor functions are working
key=rawAtom2.getHashKey()
atom=cAlphaViewer.renderer.getAtom(key)
print atom.getPDBId()
print atom.getChainId()

# Instantiate a Bond between Atoms
bond=PDBBond()
bond.setAtom0Ix(rawAtom.getHashKey())
bond.setAtom1Ix(rawAtom2.getHashKey())
cAlphaViewer.renderer.addBond(bond)

# I don't understand this code, but first 3 lines are required, last two are not
if not cAlphaViewer.loaded:
    cAlphaViewer.loaded = True
    cAlphaViewer.emitModelLoaded()
#cAlphaViewer.dirty = True
#cAlphaViewer.emitModelChanged()

sys.exit(app.exec_())
