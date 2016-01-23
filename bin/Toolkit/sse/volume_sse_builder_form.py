import logging
from libpycore import *
from sse_hunter_engine import SSEHunterEngine


class VolumeSSEBuilderForm(object):

    def __init__(self, volume, skeleton, output):
        self.logger = logging.getLogger(__name__)
        
        self.volume = Volume()
        self.volume.loadFile(volume)
        self.volume = self.volume.getVolume()
        
        self.skeleton = MeshRenderer()
        self.skeleton.loadFile(skeleton)
        self.skeleton = self.skeleton.getMesh()
        
        self.output = output
        self.calphaRenderer = CAlphaRenderer()

    def savePseudoatoms(self, temp):
        fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Pseudoatoms"), "", self.tr("Protein Data Bank (PDB) Format (*.pdb)"))
        if not fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            if not(self.app.viewers["calpha"].renderer.saveSSEHunterFile(str(fileName))):
                # TODO: Put a error message here telling the user that the save failed
                pass
            self.setCursor(QtCore.Qt.ArrowCursor)
    
    def removeHelices(self):
        self.viewer.renderer.removeHelices()
        
    def removeSheets(self):
        self.viewer.renderer.removeSheets()

    def browseAtomScoreFile(self, result):
        pdbFile = QtGui.QFileDialog.getOpenFileName(self, self.tr("Load SSEHunter Results"), "", self.tr("PDB Files (*.pdb)"))
        if not pdbFile.isEmpty():
            self.calphaViewer = self.app.viewers["calpha"]
            self.calphaViewer.loadSSEHunterData(pdbFile)
            self.sseViewer = self.app.viewers["sse"]
            self.lineEditAtomScore.setText(pdbFile)
            self.connect(self.app.viewers["calpha"],  QtCore.SIGNAL("modelUnloaded()"), self.disableSavePseudoatoms)
            self.pushButtonSavePseudoatoms.setEnabled(True)
        
    def autoBuildHelices(self):
        print "VolumeSSEBuilderForm.autoBuildHelices()"
        self.calphaViewer = self.app.viewers["calpha"]
        patom_hashkeys = self.calphaViewer.renderer.getAtomHashes();
        patoms = [self.calphaViewer.renderer.getAtom(hashkey) for hashkey in patom_hashkeys]
        
        score_thresh = self.doubleSpinBoxScoreThresh.value()
        pt_line_dist_thresh = self.horizontalSliderLinearityThresh.value() / 4.0
        auto_helix_builder = AutoHelixBuilderEngine(patoms, score_thresh, pt_line_dist_thresh)
        helix_list = auto_helix_builder.get_helix_list()
        print helix_list
        for helix in helix_list:
            for atom in helix:
                atom.setSelected(True)
            self.selectionToHelix(None)
            for atom in helix:
                atom.setSelected(False)
        
    def runSSEHunter(self, result):
        self.calphaViewer = self.app.viewers["calpha"]
        self.sseViewer = self.app.viewers["sse"]
        threshold = self.doubleSpinBoxThreshold.value()
        resolution = self.doubleSpinBoxResolution.value()
        correlationWeight = self.doubleSpinBoxCorrelation.value()
        skeletonWeight = self.doubleSpinBoxSkeleton.value()
        geometryWeight = self.doubleSpinBoxGeometry.value()

        #self.calphaViewer.runSSEHunter( threshold, resolution, correlationWeight, skeletonWeight, geometryWeight )

        vol = self.app.viewers["volume"].renderer.getVolume()
        skel = self.app.viewers["skeleton"].renderer.getMesh()
        sseh = SSEHunterEngine(vol, skel, resolution, threshold)
        patoms = sseh.getScoredAtoms(correlationWeight, skeletonWeight, geometryWeight)
        
        for pseudoatom in patoms:
            self.calphaViewer.renderer.addAtom(pseudoatom)
        
        self.calphaViewer.renderer.colorSSEHunterAtoms()
        self.calphaViewer.dirty = False
        self.calphaViewer.loaded = True
        self.calphaViewer.emitModelLoadedPreDraw()
        self.calphaViewer.emitModelLoaded()
        self.calphaViewer.emitViewerSetCenter()
        self.connect(self.app.viewers["calpha"],  QtCore.SIGNAL("modelUnloaded()"), self.disableSavePseudoatoms)
        self.pushButtonSavePseudoatoms.setEnabled(True)
        self.bringToFront()
        
    def updateTotalScoreSSEHunterAtoms(self):
        self.calphaViewer.updateTotalScoreSSEHunterAtoms( self.doubleSpinBoxCorrelation.value(), self.doubleSpinBoxSkeleton.value(),
            self.doubleSpinBoxGeometry.value() )
        
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
                
