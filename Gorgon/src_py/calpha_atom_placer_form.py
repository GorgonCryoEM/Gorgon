# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: A widget can be used to place c-alpha atoms


from PyQt4 import QtCore, QtGui
from ui_dialog_calpha_atom_placer import Ui_DialogCAlphaAtomPlacer
from gorgon_cpp_wrapper import PDBAtom, Vector3DFloat

class CAlphaAtomPlacerForm(QtGui.QWidget):
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = viewer
        self.skeletonViewer = self.app.viewers["skeleton"]
        self.connect(self.skeletonViewer, QtCore.SIGNAL("elementSelected (int, int, int, int, int, int, QMouseEvent)"), self.skeletonSelected)
        self.createUI()
        self.createActions()
        self.createMenus()

    def createUI(self):
        self.ui = Ui_DialogCAlphaAtomPlacer()
        self.ui.setupUi(self)       
        self.dock = QtGui.QDockWidget(self.tr("C-Alpha - Atom Placer"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.pushButtonAddAtom, QtCore.SIGNAL("pressed()"), self.addAtom)
        
    def loadWidget(self):
        if(self.app.actions.getAction("perform_CAlphaManualAtomPlacement").isChecked()) :
            self.app.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.dock)
            self.skeletonViewer.setSelectEnabled(True)
            self.dock.show()
        else:
            self.skeletonViewer.setSelectEnabled(False)
            self.app.removeDockWidget(self.dock)
            
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("perform_CAlphaManualAtomPlacement").setChecked(visible)
        
    def skeletonSelected(self, h0, h1, h2, h3, h4, h5, event):
        position = self.skeletonViewer.renderer.get3DCoordinates(h0, h1, h2, h3, h4, h5)
        self.ui.doubleSpinBoxPositionX.setValue(position.x());
        self.ui.doubleSpinBoxPositionY.setValue(position.y());
        self.ui.doubleSpinBoxPositionZ.setValue(position.z());
    
    def addAtom(self):
        atom = PDBAtom()
        atom.setSerial(self.ui.spinBoxSerial.value())
        #TODO: Find out how to convert QStrings to Std:Strings
        atom.setName(str(self.ui.lineEditAtomName.text()))
        #atom.setAltLoc(self.ui.lineEditAlternateLocation.text()[0])
        atom.setResName(str(self.ui.lineEditResidueName.text()))
        #atom.setChainId(self.ui.lineEditChainIdentifier.text()[0])
        atom.setResSeq(self.ui.spinBoxResidueSequenceNo.value())
        #atom.setICode(self.ui.lineEditInsertionCode.text()[0])
        atom.setPosition(Vector3DFloat(self.ui.doubleSpinBoxPositionX.value(), 
                                       self.ui.doubleSpinBoxPositionY.value(),
                                       self.ui.doubleSpinBoxPositionZ.value()))
        atom.setOccupancy(self.ui.doubleSpinBoxOccupancy.value())
        atom.setTempFactor(self.ui.doubleSpinBoxTemperatureFactor.value())
        atom.setElement(str(self.ui.lineEditElement.text()))
        atom.setCharge(str(self.ui.lineEditCharge.text()))
        self.viewer.renderer.addAtom(atom)
        if(not self.viewer.loaded):
            self.viewer.loaded = True
            self.viewer.emitModelLoaded()
        self.viewer.dirty = True
        self.viewer.emitModelChanged()
    
    def createActions(self):               
        placeAct = QtGui.QAction(self.tr("&Manual C-Alpha Atom Placement"), self)
        placeAct.setStatusTip(self.tr("Perform Manual C-Alpha atom placement"))
        placeAct.setCheckable(True)
        placeAct.setChecked(False)
        self.connect(placeAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("perform_CAlphaManualAtomPlacement", placeAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-calpha-manualAtomPlacement", self.app.actions.getAction("perform_CAlphaManualAtomPlacement"), "actions-calpha")        
    
        