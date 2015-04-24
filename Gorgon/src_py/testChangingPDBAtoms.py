#!/usr/bin/python
import sys
from libpyGORGON import PDBAtom,Vector3DFloat
from PyQt4 import QtGui,  QtCore
from window_manager import WindowManager
from main_window_form import MainWindowForm

app = QtGui.QApplication(sys.argv)
window = MainWindowForm("2.0")
window.addModule(WindowManager(window))
window.showMaximized()
cAlphaViewer=window.viewers['calpha']

rawAtom=PDBAtom('AAAA', 'A', 1, 'CA')
rawAtom.setPosition(Vector3DFloat(-1, 0, 0))
rawAtom = cAlphaViewer.renderer.addAtom(rawAtom)

a = PDBAtom('AAAA', 'A', 2, 'CA')
a.setPosition(Vector3DFloat(1, 0, 0))
print 'a:', a
b = cAlphaViewer.renderer.addAtom(a)
print 'b-1:', b

if not cAlphaViewer.loaded:
    cAlphaViewer.loaded = True
    cAlphaViewer.emitModelLoaded()

del a   #This line causes a segmentation fault later on, which is weird because we never use the atom at this location in memory after this
#Thus, unless there's a change, I'll have to be extremely careful about Python's garbage collection

rawAtom.setColor(1, 0, 0, 1) #Red
rawAtom.setAtomRadius(2)

cAlphaViewer.renderer.deleteAtom(b.getHashKey())

print 'b-2:', b
c = cAlphaViewer.renderer.addAtom(b) #The seg fault occurs here
print 'c:', c

cAlphaViewer.emitModelChanged()

sys.exit(app.exec_())
