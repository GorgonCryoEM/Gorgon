#We can modify this file to load data and dialogs that we have not yet 
#built into the GUI.
#This modification of Gorgon.pyw by Ross Coleman

import sys
from window_manager import WindowManager
from PyQt4 import QtGui
from main_window_form import MainWindowForm
from about_form import AboutForm
import seq_model.Chain as Chain
import seq_model.SequenceView as SequenceView

if __name__ == '__main__':
    
    app = QtGui.QApplication(sys.argv)
    
    window = MainWindowForm()
    window.addModule(WindowManager(window))
    window.showMaximized()
    
    viewer = window.viewers["calpha"]
    mychain = Chain.Chain.load('1KPO.pdb', qparent=app)
    mychain.setViewer(viewer)
    Chain.Chain.setSelectedChainKey(mychain.getIDs())
    mychain.addCalphaBonds()
    renderer = mychain.getViewer().renderer
    
    SequenceView.renderMockSidechains(mychain)
    
    for i in mychain.residueRange():
        atom = mychain[i].getAtom('CA')
        renderer.addAtom(atom)
    
    if (not mychain.getViewer().loaded):
        viewer.dirty = False
        mychain.getViewer().loaded = True
        viewer.emitModelLoadedPreDraw()
        mychain.getViewer().emitModelLoaded()
        mychain.getViewer().emitViewerSetCenter()
    
    print "The mock sidechains should now be visible."    
    SequenceView.clearMockSidechains(mychain)
    viewer.setModelVisibility(False)
    viewer.setModelVisibility(True)
    print "There should now be no mock sidechains displayed."
    #Note: setModelVisibility calls base_viewer.modelChanged(), which doesn't seem to pick up on changes in atom attributes
    #The 3 lines of code (excluding print statements) above should cause the default yellow atoms to be displayed.
    #However, our mock side chains continue to be displayed
    #On the other hand, if we unload the data, and then reload it, changes to atom attributes do show up, as the code below shows.
    
    '''
    SequenceView.clearMockSidechains(mychain)
    viewer.unloadData()
    mychain.addCalphaBonds()
    
    for i in mychain.residueRange():
        atom = mychain[i].getAtom('CA')
        #mychain[i].setCAlphaColorScheme1()
        #mychain[i].setCAlphaColorScheme2()
        #mychain[i].setCAlphaColorsToDefault()
        renderer.addAtom(atom)
    if (not mychain.getViewer().loaded):
        viewer.dirty = False
        mychain.getViewer().loaded = True
        viewer.emitModelLoadedPreDraw()
        mychain.getViewer().emitModelLoaded()
        mychain.getViewer().emitViewerSetCenter()
    '''
    
    
    
    sys.exit(app.exec_())
