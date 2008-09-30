#We can modify this file to load data and dialogs that we have not yet 
#built into the GUI.
#This modification of Gorgon.pyw by Ross Coleman

import sys
from window_manager import WindowManager
from PyQt4 import QtGui
from main_window_form import MainWindowForm
from about_form import AboutForm

if __name__ == '__main__':
    import seq_model.Chain as Chain
    import seq_model.SequenceView as SequenceView
    
    app = QtGui.QApplication(sys.argv)
    window = MainWindowForm()
    window.addModule(WindowManager(window))
    window.showMaximized()

    viewer = window.viewers["calpha"]
    mychain = Chain.Chain.load('1KPOgroel.pdb', qparent=app)
    seqWidget = SequenceView.SequenceWidget(mychain)
    seqWidget.show()
    #For some reason this will not allow all the residues to be accessed.
    #On the other hand, running SequenceView.py directly will allow
    #all residues to be accessed.  My guess is that the problem has something to
    #do with the window having a parent.
    
    mychain.setViewer(viewer)
    renderer = mychain.getViewer().renderer
    for i in mychain.residueRange():
        atom = mychain[i].getAtom('CA')
        renderer.addAtom(atom)
    if (not mychain.getViewer().loaded):
        viewer.dirty = False
        mychain.getViewer().loaded = True
        viewer.emitModelLoadedPreDraw()
        mychain.getViewer().emitModelLoaded()
        mychain.getViewer().emitViewerSetCenter()
    
    
    
    sys.exit(app.exec_())
