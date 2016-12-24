from PyQt4 import QtCore, QtGui
from ui_volume_builder import Ui_DialogVolumeSSEBuilder
from ...toolkit.sse.hunter import pySSEHunter
from auto_helix_builder_engine import AutoHelixBuilderEngine
# from ...toolkit.libpytoolkit import SSERenderer
# from ...toolkit.libpytoolkit import CAlphaRenderer
# from calpha_viewer import CAlphaViewer
# from sse_viewer import SSEViewer


class VolumeSSEBuilderForm(QtGui.QDialog, Ui_DialogVolumeSSEBuilder):
        
    def __init__(self, parent, dockArea=QtCore.Qt.LeftDockWidgetArea):
        QtGui.QDialog.__init__(self, parent)
        self.parent = parent
        self.dockArea = dockArea
    
    def init_again(self):
        self.volume = self.parent.volume
        self.skeleton = self.parent.skeleton
        self.args = self.parent.args
        self.calpha = self.parent.calpha
        self.sse = self.parent.sse
        self.viewer = self.sse
        
        dock = QtGui.QDockWidget("SSEBuilder", self.volume)
        dock.setWidget(self)
        dock.setAllowedAreas(QtCore.Qt.AllDockWidgetAreas)
        self.parent.addDockWidget(self.dockArea, dock)

        self.connect(self.volume, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.calpha, QtCore.SIGNAL("atomSelectionUpdated(PyQt_PyObject)"), self.atomSelectionChanged)

        self.createUI()

    def createUI(self):
        self.setupUi(self)
        
        self.connect(self.pushButtonBrowseAtomScore,  QtCore.SIGNAL("clicked (bool)"),       self.browseAtomScoreFile)
        self.connect(self.pushButtonSelectionToHelix, QtCore.SIGNAL("clicked (bool)"),       self.selectionToHelix)
        self.connect(self.pushButtonSelectionToSheet, QtCore.SIGNAL("clicked (bool)"),       self.selectionToSheet)
        self.connect(self.pushButtonRemoveSSE,        QtCore.SIGNAL("clicked (bool)"),       self.removeSSE)
        self.connect(self.pushButtonSSEHunter,        QtCore.SIGNAL("clicked (bool)"),       self.runSSEHunter)
        self.connect(self.pushButtonSavePseudoatoms,  QtCore.SIGNAL("clicked (bool)"),       self.savePseudoatoms)
        self.connect(self.pushButtonAddHelices,       QtCore.SIGNAL("clicked (bool)"),       self.autoBuildHelices)
        self.connect(self.pushButtonRemoveHelices,    QtCore.SIGNAL("clicked (bool)"),       self.removeHelices)
        self.connect(self.doubleSpinBoxCorrelation,   QtCore.SIGNAL("valueChanged(double)"), self.updateTotalScoreSSEHunterAtoms)
        self.connect(self.doubleSpinBoxSkeleton,      QtCore.SIGNAL("valueChanged(double)"), self.updateTotalScoreSSEHunterAtoms)
        self.connect(self.doubleSpinBoxGeometry,      QtCore.SIGNAL("valueChanged(double)"), self.updateTotalScoreSSEHunterAtoms)
        
        self.pushButtonSaveHelices.clicked.connect(self.sse.saveHelixData)
        
    def savePseudoatoms(self):
        fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Pseudoatoms"), "", self.tr("Protein Data Bank (PDB) Format (*.pdb)"))
        if not fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            if not(self.calpha.renderer.saveSSEHunterFile(str(fileName))):
                # TODO: Put a error message here telling the user that the save failed
                pass
            self.setCursor(QtCore.Qt.ArrowCursor)
    
    def removeHelices(self):
        self.viewer.renderer.removeHelices()
        self.viewer.modelChanged()
        
    def removeSheets(self):
        self.viewer.renderer.removeSheets()
        self.viewer.modelChanged()
        
    def removeSSE(self, temp):
        if(QtGui.QMessageBox.question(self, "Remove Selected SSEs?", "This will remove the selected SSEs. Are you sure?", "Yes", "Cancel") == 0):
            self.viewer.renderer.removeSelectedSSEs()
            self.viewer.modelChanged()
            # self.bringToFront()
                                                                                                                                        
    def modelLoaded(self):
        #self.detectSSEAct.setEnabled(True)
        maxDensity = self.volume.renderer.getMaxDensity()
        minDensity = self.volume.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2

        self.doubleSpinBoxThreshold.setValue(defaultDensity)
        
        print minDensity, maxDensity, defaultDensity
       
    def dockVisibilityChanged(self, visible):
        BaseDockWidget.dockVisibilityChanged(self, visible)
        self.calpha.centerOnRMB = not visible
#         if(visible):
#             self.connect(self.calpha, QtCore.SIGNAL("atomSelectionUpdated(PyQt_PyObject)"), self.atomSelectionChanged)
#         else:
#             self.disconnect(self.calpha, QtCore.SIGNAL("atomSelectionUpdated(PyQt_PyObject)"), self.atomSelectionChanged)

    def browseAtomScoreFile(self, result):
        pdbFile = QtGui.QFileDialog.getOpenFileName(self, self.tr("Load SSEHunter Results"), "", self.tr("PDB Files (*.pdb)"))
        if not pdbFile.isEmpty():
            self.calpha.loadSSEHunterData(pdbFile)
            self.lineEditAtomScore.setText(pdbFile)
            self.pushButtonSavePseudoatoms.setEnabled(True)
#         self.bringToFront()
        
    def autoBuildHelices(self):
        print "VolumeSSEBuilderForm.autoBuildHelices()"
        patom_hashkeys = self.calpha.renderer.getAtomHashes();
        patoms = [self.calpha.renderer.getAtom(hashkey) for hashkey in patom_hashkeys]
        
        score_thresh = self.doubleSpinBoxScoreThresh.value()
        pt_line_dist_thresh = self.horizontalSliderLinearityThresh.value() / 4.0
        auto_helix_builder = AutoHelixBuilderEngine(patoms, score_thresh, pt_line_dist_thresh)
        helix_list = auto_helix_builder.get_helix_list()
        print helix_list
        print "helix_list szie: ", len(helix_list)
        for helix in helix_list:
            for atom in helix:
                atom.setSelected(True)
            self.selectionToHelix(None)
            for atom in helix:
                atom.setSelected(False)
        
    def runSSEHunter(self, result):
        threshold = self.doubleSpinBoxThreshold.value()
        resolution = self.doubleSpinBoxResolution.value()
        correlationWeight = self.doubleSpinBoxCorrelation.value()
        skeletonWeight = self.doubleSpinBoxSkeleton.value()
        geometryWeight = self.doubleSpinBoxGeometry.value()

        #self.calpha.runSSEHunter( threshold, resolution, correlationWeight, skeletonWeight, geometryWeight )

#         vol  = self.parent.volume.renderer.getVolume()
#         skel = self.parent.skeleton.renderer.getMesh()
#         sseh = SSEHunter(vol, skel, resolution, threshold)
#         patoms = self.calpha.loadSSEHunterData('pseudoatoms.pdb')
#         self.calpha.loadData()
        if hasattr(self.args, 'output'):
            output = self.args.output
        else:
            output = ""
        sseh = pySSEHunter(self.args.volume,
                           self.args.skeleton,
                           output,
                           resolution,
                           threshold,
                           correlationWeight,
                           skeletonWeight,
                           geometryWeight)
               
        sseh.threshold = threshold
        patoms = sseh.getScoredAtoms(correlationWeight, skeletonWeight, geometryWeight)
#
        self.calpha.renderer.deleteAtoms()
        for pseudoatom in patoms:
            self.calpha.renderer.addAtom(pseudoatom)
         
        self.calpha.renderer.colorSSEHunterAtoms()
        self.calpha.dirty = False
        self.calpha.loaded = True
#         self.calpha.emitModelLoadedPreDraw()
        self.calpha.modelChanged()
#         self.calpha.emitViewerSetCenter()
        self.pushButtonSavePseudoatoms.setEnabled(True)
#         self.bringToFront()
        
    def updateTotalScoreSSEHunterAtoms(self):
        self.calpha.updateTotalScoreSSEHunterAtoms( self.doubleSpinBoxCorrelation.value(), self.doubleSpinBoxSkeleton.value(),
            self.doubleSpinBoxGeometry.value() )
        
    def atomSelectionChanged(self, selection):
        self.tableWidgetSelection.clearContents()
        atomCnt = self.calpha.renderer.selectionAtomCount()
        print "  ....atomCnt: ", atomCnt
        self.tableWidgetSelection.setRowCount(atomCnt)
        
        for i in range(atomCnt):
            atom = self.calpha.renderer.getSelectedAtom(i)
            self.tableWidgetSelection.setItem(i, 0, QtGui.QTableWidgetItem(str(atom.getResSeq())))
            self.tableWidgetSelection.setItem(i, 1, QtGui.QTableWidgetItem(str(atom.getTempFactor())))

    def getHelixEnds(self, atoms):
        distance = 0
        ends = []
        for pt1 in atoms:
            pos1 = pt1.getPosition()
            for pt2 in atoms:
                pos2 = pt2.getPosition()
                atomDist = (pos2 - pos1).length()
                if(atomDist > distance):
                    distance = atomDist
                    ends = [pos1, pos2]
        return ends
                
    def selectionToHelix(self, result):
        self.pushAtomsToEngine()
            
        self.sse.renderer.finalizeHelix()
        
        self.sse.loaded = True
        self.sse.helixLoaded = True
        self.sse.dirty = True
        self.sse.modelChanged()
#         self.bringToFront()
    
    def pushAtomsToEngine(self):
        atomCnt = self.calpha.renderer.selectionAtomCount()
        
        self.sse.renderer.startNewSSE();
        
        print "...pushAtomsToEngine:"
        for i in range(atomCnt):
            atom = self.calpha.renderer.getSelectedAtom(i)
            position = atom.getPosition()
            self.sse.renderer.addSSEPoint(position)
            print "   ...", atom, position
        
    def selectionToSheet(self, result):
        self.pushAtomsToEngine()
            
        self.sse.renderer.finalizeSheet()
        
        if(self.sse.loaded):
            self.sse.sheetLoaded = True
            self.sse.dirty = True
            self.sse.modelChanged()
        else:
            self.sse.loaded = True
            self.sse.sheetLoaded = True
            self.sse.dirty = True
            self.sse.emitModelLoadedPreDraw()
            self.sse.emitModelLoaded()
        self.bringToFront()
