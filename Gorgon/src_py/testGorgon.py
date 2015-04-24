#We can modify this file to load data and dialogs that we have not yet 
#built into the GUI.
#This modification of Gorgon.pyw by Ross Coleman

import sys
from window_manager import WindowManager
from PyQt4 import QtGui
from main_window_form import MainWindowForm
from about_form import AboutForm
import seq_model.Chain as Chain
import calpha_sequence_view as CAlphaSequenceView

if __name__ == '__main__':
    
    app = QtGui.QApplication(sys.argv)
    
    window = MainWindowForm("2.0")
    window.addModule(WindowManager(window))
    window.showMaximized()
    
    viewer = window.viewers["calpha"]
    mychain = Chain.Chain.load('1KPO.pdb', qparent=app)
    mychain.setViewer(viewer)
    #Chain.Chain.setSelectedChainKey(mychain.getIDs())
    viewer.main_chain = mychain
    mychain.addCalphaBonds()
    renderer = mychain.getViewer().renderer
    
    CAlphaSequenceView.renderMockSidechains(mychain)
    atoms = []
    for i in mychain.residueRange():
        atom = mychain[i].getAtom('CA')
        atom = renderer.addAtom(atom)
        mychain[i].addAtomObject(atom) #The residues need to reference the new atom objects created when we add them to the renderer
    
    print "mychain.getViewer() is window.viewers['calpha']?",  mychain.getViewer() is window.viewers['calpha']
    
    if not viewer.loaded:
        viewer.dirty = False
        viewer.loaded = True
        viewer.emitModelLoadedPreDraw()
        viewer.emitModelLoaded()
        viewer.emitViewerSetCenter()
    
    print "The mock sidechains should now be visible."    
    CAlphaSequenceView.clearMockSidechains(mychain)
    
    #Note: the line below doesn't cause the change (no mock sidechains) to be displayed
    viewer.emitModelChanged()
    print "There should now be no mock sidechains displayed."
    
    
    ''' The code below is old
    #Note: base_viewer.modelChanged()doesn't seem to pick up on changes in atom attributes
    
    #On the other hand, if we delete the atoms from the renderer, and add them back to the renderer, 
    #then base_viewer.modelChanged() does cause the changes to be applied
    '''
    '''
    for i in mychain.residueRange():
        atom = mychain[i].getAtom('CA')
        renderer.deleteAtom(atom.getHashKey())
        renderer.addAtom(atom)
    viewer.emitModelChanged()
    '''
        
    sys.exit(app.exec_())
