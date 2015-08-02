# History Log:
#   Revision 1.0
#   Adding in CVS meta data
#

from PyQt4 import QtGui, QtCore, QtOpenGL
from libpyGORGON import CAlphaRenderer
from base_viewer import BaseViewer
from calpha_viewer import CAlphaViewer
from calpha_choose_chain_to_load_form import CAlphaChooseChainToLoadForm
from calpha_atom_placer_form import CAlphaAtomPlacerForm
from calpha_sequence_dock import CAlphaSequenceDock
from seq_model.Chain import Chain
from atom_visualization_form import AtomVisualizationForm
from correspondence.StructurePrediction import StructurePrediction
from calpha_choose_chain_model import CAlphaChooseChainModel
from calpha_flexible_fitting_form import CAlphaFlexibleFittingForm

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import collections, math, colorsys

class CAlphaReferenceViewer(CAlphaViewer):
    DisplayStyleBackbone = 3
    DisplayStyleRibbon = 4
    DisplayStyleSideChain = 5
    
    def __init__(self, main, parent=None):
        BaseViewer.__init__(self, main, parent)
        self.title = "Reference"
        self.shortTitle = "CALREF"
        self.app.themes.addDefaultRGB("Reference:Atom", 228, 250, 255, 255)
        self.app.themes.addDefaultRGB("Reference:Bond", 0, 41, 122, 255)
        self.app.themes.addDefaultRGB("Reference:Helix", 0, 255, 0, 255)
        self.app.themes.addDefaultRGB("Reference:Strand", 128, 255, 255, 255)
        self.app.themes.addDefaultRGB("Reference:Loop", 255, 128, 0, 255)
        self.app.themes.addDefaultRGB("Reference:Carbon", 200, 200, 200, 255)
        self.app.themes.addDefaultRGB("Reference:Nitrogen", 0, 0, 255, 255)
        self.app.themes.addDefaultRGB("Reference:Oxygen", 255, 0, 0, 255)
        self.app.themes.addDefaultRGB("Reference:Sulphur", 255, 255, 0, 255)
        self.app.themes.addDefaultRGB("Reference:BoundingBox", 255, 255, 255, 255)
        self.isClosedMesh = False
        self.centerOnRMB = True
        self.selectEnabled = True
        self.renderer = CAlphaRenderer()
        self.displayStyle = self.DisplayStyleBackbone
        self.renderer.setDisplayStyle(self.displayStyle)
        self.main_chain = Chain('', self.app)
        self.structPred = None
        self.createUI()
        self.app.viewers["calpharef"] = self
        self.cAlphaViewer = self.app.viewers["calpha"]
        self.atomsVisible = True
        self.bondsVisible = True
        self.helicesVisible = True
        self.strandsVisible = True
        self.loopsVisible = True
        self.initVisualizationOptions(AtomVisualizationForm(self.app, self))
        self.loadedChains = []
        self.loadedAtoms = []
        self.ribbonMouseMapping = {}
        self.ribbonMouseMapping[0] = {}
        self.ribbonMouseMapping[1] = {}
        self.ribbonMouseMapping[2] = {}
        self.connect(self, QtCore.SIGNAL("elementClicked (int, int, int, int, int, int, QMouseEvent)"), self.processElementClick)

    # Overridden
    def createChildWindows(self):
        # Don't want to double child windows created
        return

    # Overridden
    def createMenus(self):
        self.app.menus.addAction("file-open-calpha", self.app.actions.getAction("load_CAlpha"), "file-open")
        self.app.menus.addAction('file-open-sequence', self.app.actions.getAction('load_sequence'), 'file-open')
        self.app.menus.addAction("file-save-calpha", self.app.actions.getAction("save_CAlpha"), "file-save")
        self.app.menus.addAction("file-export-calpha", self.app.actions.getAction("export_CAlpha"), "file-export")
        self.app.menus.addAction("file-close-calpha", self.app.actions.getAction("unload_CAlpha"), "file-close")
        # self.app.menus.addMenu("actions-calpha", self.tr("C-&Alpha Atoms"), "actions")
        # self.app.menus.addAction("showSeqDock", self.app.actions.getAction("seqDock"), "actions-calpha")

    # Overridden
    def createActions(self):
        openRefAct = QtGui.QAction(self.tr("C-&Alpha Reference..."), self)
        openRefAct.setShortcut(self.tr("Ctrl+Shift+A"))
        openRefAct.setStatusTip(self.tr("Load a C-Alpha atom reference"))
        self.connect(openRefAct, QtCore.SIGNAL("triggered()"), self.loadRefData)
        self.app.actions.addAction("load_CAlpha_ref", openRefAct)

        validateAct = QtGui.QAction(self.tr("C-&Alpha Validation..."), self)
        validateAct.setStatusTip(self.tr("Validate your model"))
        self.connect(validateAct, QtCore.SIGNAL("triggered()"), self.validate)
        self.app.actions.addAction("validate", validateAct)

        visualizePairwiseDistAct = QtGui.QAction(self.tr("Visualize Pairwise Distance..."), self)
        visualizePairwiseDistAct.setStatusTip(self.tr("Visualize pairwise distances"))
        self.connect(visualizePairwiseDistAct, QtCore.SIGNAL("triggered()"), self.visualizePairwiseDistance)
        self.app.actions.addAction("visualizePairwiseDist", visualizePairwiseDistAct)

        displayValidationResultsAct = QtGui.QAction(self.tr("Validation Results.."), self)
        # validateAct.setStatusTip(self.tr("Validate your model"))
        self.connect(displayValidationResultsAct, QtCore.SIGNAL("triggered()"), self.validate)
        self.app.actions.addAction("displayValidationResults", displayValidationResultsAct)

        # openSeqAct = QtGui.QAction(self.tr('Se&quence and SSE prediction'), self)
        # openSeqAct.setShortcut(self.tr('Ctrl+Shift+U'))
        # openSeqAct.setStatusTip(self.tr('Load a sequence possibly with SSE predictions'))
        # self.connect(openSeqAct, QtCore.SIGNAL('triggered()'), self.loadSeq)
        # self.app.actions.addAction('load_sequence', openSeqAct)

        # saveAct = QtGui.QAction(self.tr("C-&Alpha Atoms..."), self)
        # saveAct.setStatusTip(self.tr("Save a C-Alpha atom reference"))
        # self.connect(saveAct, QtCore.SIGNAL("triggered()"), self.saveData)
        # self.app.actions.addAction("save_CAlpha", saveAct)
        #
        # exportAct = QtGui.QAction(self.tr('Atoms to &PDB...'), self)
        # exportAct.setStatusTip(self.tr('Export a PDB file with no placeholder atoms'))
        # self.connect(exportAct, QtCore.SIGNAL('triggered()'), self.exportData)
        # self.app.actions.addAction('export_CAlpha', exportAct)

        closeAct = QtGui.QAction(self.tr("C-&Alpha Reference"), self)
        closeAct.setStatusTip(self.tr("Close the loaded C-Alpha atom reference"))
        self.connect(closeAct, QtCore.SIGNAL("triggered()"), self.unloadData)
        self.app.actions.addAction("unload_CAlpha_ref", closeAct)

    # Overridden
    def loadRefData(self):
        #Overwriting the function in CAlphaviewer
        def setupChain(mychain):
            self.main_chain = mychain
            self.loadedChains.append(mychain)
            mychain.setViewer(self)
            mychain.addCalphaBonds()
            mychain.addSideChainBonds()
            renderer = self.renderer
            for i in mychain.residueRange():
                for atomName in mychain[i].getAtomNames():
                    atom = mychain[i].getAtom(atomName)
                    if atom:
                        atom = renderer.addAtom(atom)
                        mychain[i].addAtomObject(atom)
                        if atom.getName() == "CA":
                            self.loadedAtoms.append(atom)

        dialog = QtGui.QFileDialog(self)
        dialog.setWindowTitle(self.tr('Open Reference Data'))
        dialog.setNameFilter(self.tr('Atom Positions (*.pdb)\nFASTA (*.fas *.fa *.fasta)'))
        dialog.setFileMode(QtGui.QFileDialog.ExistingFile)

        if dialog.exec_() == QtGui.QDialog.Accepted:
            self.fileName = dialog.selectedFiles()[0]
            fileNameTemp = self.fileName
            self.whichChainID = None
            filename = unicode(self.fileName)
            if filename.split('.')[-1].lower() == 'pdb':
                dlg = CAlphaChooseChainToLoadForm(unicode(self.fileName))
                if dlg.exec_():
                    self.whichChainID = dlg.whichChainID
                    if not self.fileName.isEmpty():
                        if(self.loaded):
                            self.unloadData()
                        self.fileName = fileNameTemp
                        if self.whichChainID == 'ALL':
                            mychainKeys = Chain.loadAllChains(str(self.fileName), qparent=self.app)
                            for chainKey in mychainKeys:
                                setupChain(Chain.getChain(chainKey))
                        else:
                            mychain = Chain.load(str(self.fileName), qparent=self.app, whichChainID = self.whichChainID)
                            setupChain(mychain)
                        if not self.loaded:
                            self.dirty = False
                            self.loaded = True
                            self.setAtomColorsAndVisibility(self.displayStyle)
                            self.emitModelLoadedPreDraw()
                            self.emitModelLoaded()
                            self.emitViewerSetCenter()

            # Update UI here, necessary because calpha_flexible_fitting_form.py can't access this class
            # due to order in which each viewer is loaded
            self.cAlphaViewer.refLoaded = True
            self.cAlphaViewer.flexibleFitter.enableDisableWindowElements()

            # Initial call to validate
            self.validate()

    # Overridden
    def unloadData(self):
        #overwriting the function in base viewer
        for chain in self.loadedChains:
            del chain
            chain = None
        self.loadedChains = []
        BaseViewer.unloadData(self)
        self.cAlphaViewer.refLoaded = False

    # Calculates and displays the RMSD and Hausdorff distance of two models
    def validate(self):
        isPerfectMatching = self.cAlphaViewer.flexibleFitter.usePerfectMatchingCheckbox.isChecked()
        modelAtomsByDist, refAtomsByDist = self.computeDistance(self.cAlphaViewer.loadedAtoms, self.loadedAtoms, isPerfectMatching)
        RMSD = self.computeRMSD(modelAtomsByDist, refAtomsByDist)
        hausdorff = self.computeHausdorff(modelAtomsByDist)
        self.cAlphaViewer.flexibleFitter.displayValidationResults(hausdorff, RMSD)

    # Colors atoms of two models based on the distance between two like-atoms
    def visualizePairwiseDistance(self):
        if not (self.cAlphaViewer.loaded and self.loaded):
            print "Need to load two models"
            return

        # User unchecked the checkbox, so display default colors instead of coloring by distance
        if not self.cAlphaViewer.flexibleFitter.visualizePairwiseDistanceCheckbox.isChecked():
            # Update reference colors
            refColor = self.getAtomColor()
            for chain in self.loadedChains:
                for i in chain.residueRange():
                    if i in chain.residueList:
                        for atomName in chain[i].getAtomNames():
                            atom = chain[i].getAtom(atomName)
                            if atom and atom.getName() == "CA":
                                atom.setColor(refColor.redF(), refColor.greenF(), refColor.blueF(), refColor.alphaF())
            self.emitModelChanged()

            # Update model colors
            modelColor = self.cAlphaViewer.getAtomColor()
            for chain in self.cAlphaViewer.loadedChains:
                for i in chain.residueRange():
                    if i in chain.residueList:
                        for atomName in chain[i].getAtomNames():
                            atom = chain[i].getAtom(atomName)
                            if atom and atom.getName() == "CA":
                                atom.setColor(modelColor.redF(), modelColor.greenF(), modelColor.blueF(), modelColor.alphaF())
            self.cAlphaViewer.emitModelChanged()
            return

        # Colorization of atoms depends on if the atom sequences in PDB files were corresponding or not
        isPerfectMatching = self.cAlphaViewer.flexibleFitter.usePerfectMatchingCheckbox.isChecked()
        modelAtomsByDist, refAtomsByDist = self.computeDistance(self.cAlphaViewer.loadedAtoms, self.loadedAtoms, isPerfectMatching)

        # Get color spectrum for atoms and save in new QColor objects
        refColor = QtGui.QColor()
        refColor.setRedF(self.cAlphaViewer.getClosestDistColor().redF())
        refColor.setGreenF(self.cAlphaViewer.getClosestDistColor().greenF())
        refColor.setBlueF(self.cAlphaViewer.getClosestDistColor().blueF())

        modelColor = QtGui.QColor()
        modelColor.setRedF(self.cAlphaViewer.getClosestDistColor().redF())
        modelColor.setGreenF(self.cAlphaViewer.getClosestDistColor().greenF())
        modelColor.setBlueF(self.cAlphaViewer.getClosestDistColor().blueF())

        closestColor = QtGui.QColor()
        closestColor.setRedF(refColor.redF())
        closestColor.setGreenF(refColor.greenF())
        closestColor.setBlueF(refColor.blueF())

        farthestColor = self.cAlphaViewer.getFarthestDistColor()

        # Get HSV versions of closest and farthest colors
        closestColorHSV = closestColor.toHsv()
        farthestColorHSV = farthestColor.toHsv()

        # Update reference
        maxDist = refAtomsByDist.get(refAtomsByDist.keys()[-1])
        i = 0
        for chain in self.loadedChains:
            for i in chain.residueRange():
                if i in chain.residueList:
                    for atomName in chain[i].getAtomNames():
                        atom = chain[i].getAtom(atomName)
                        if atom and atom.getName() == "CA" and atom is not None:
                            # Find atom's distance and calculate its percent from maximum distance
                            print atom.getResSeq()
                            dist = refAtomsByDist.get(atom)
                            if maxDist is 0:
                                ratio = 1
                            else:
                                if dist is None:
                                    ratio = 1
                                else:
                                    ratio = dist/maxDist

                            if self.cAlphaViewer.flexibleFitter.useHueColorsCheckbox.isChecked():
                                # Interpolate HSV values based on atom's distance and then convert to RGB
                                farthestHue = farthestColorHSV.hueF()
                                closestHue = closestColorHSV.hueF()
                                hue = (farthestHue*ratio + closestHue*(1-ratio))
                                sat = val = 1.0
                                red, green, blue = colorsys.hsv_to_rgb(hue, sat, val)
                                red, green, blue = 255*red, 255*green, 255*blue

                            else:
                                # Interpolate RGB values based on atom's distance
                                red = 255*(farthestColor.redF()*ratio + closestColor.redF()*(1-ratio))
                                green = 255*(farthestColor.greenF()*ratio + closestColor.greenF()*(1-ratio))
                                blue = 255*(farthestColor.blueF()*ratio + closestColor.blueF()*(1-ratio))

                            refColor.setRed(red)
                            refColor.setGreen(green)
                            refColor.setBlue(blue)

                            # Display updated colors for one atom
                            atom.setColor(refColor.redF(), refColor.greenF(), refColor.blueF(), refColor.alphaF())
                            # atom.setColor(i, 0, 0, 1)
                            print "Actual:", 255*atom.getColorR(), 255*atom.getColorG(), 255*atom.getColorB()
                            print "----"
        self.emitModelChanged()

        # Now update the other model
        maxDist = modelAtomsByDist.get(modelAtomsByDist.keys()[-1])
        for chain in self.cAlphaViewer.loadedChains:
            for i in chain.residueRange():
                if i in chain.residueList:
                    for atomName in chain[i].getAtomNames():
                        atom = chain[i].getAtom(atomName)
                        if atom and atom.getName() == "CA" and atom is not None:
                            # Find atom's distance and calculate its percent from maximum distance
                            dist = modelAtomsByDist.get(atom)
                            if maxDist is 0:
                                ratio = 1
                            else:
                                if dist is None:
                                    ratio = 1
                                else:
                                    ratio = dist/maxDist

                            if self.cAlphaViewer.flexibleFitter.useHueColorsCheckbox.isChecked():
                                # Interpolate HSV values based on atom's distance and then convert to RGB
                                farthestHue = farthestColorHSV.hueF()
                                closestHue = closestColorHSV.hueF()
                                hue = (farthestHue*ratio + closestHue*(1-ratio))
                                sat = val = 1.0
                                red, green, blue = colorsys.hsv_to_rgb(hue, sat, val)
                                red, green, blue = 255*red, 255*green, 255*blue

                            else:
                                red = 255*(farthestColor.redF()*ratio + closestColor.redF()*(1-ratio))
                                green = 255*(farthestColor.greenF()*ratio + closestColor.greenF()*(1-ratio))
                                blue = 255*(farthestColor.blueF()*ratio + closestColor.blueF()*(1-ratio))

                            modelColor.setRed(red)
                            modelColor.setGreen(green)
                            modelColor.setBlue(blue)

                            # Display updated colors for one atom
                            atom.setColor(modelColor.redF(), modelColor.greenF(), modelColor.blueF(), modelColor.alphaF())

        self.cAlphaViewer.emitModelChanged()

    # Calculate Euclidean distance in R3 for each point in two lists of PDBAtoms
    def computeDistance(self, modelAtoms, refAtoms, perfectMatching):
        if len(modelAtoms) == 0 or len(refAtoms) == 0:
            return 0

        modelPointsByDist = collections.OrderedDict()
        refPointsByDist = collections.OrderedDict()

        if perfectMatching:
            for i in range(0, len(max(modelAtoms, refAtoms))):
                dist = math.sqrt((modelAtoms[i].getPosition().x() - refAtoms[i].getPosition().x())**2 +
                                 (modelAtoms[i].getPosition().y() - refAtoms[i].getPosition().y())**2 +
                                 (modelAtoms[i].getPosition().z() - refAtoms[i].getPosition().z())**2)
                modelPointsByDist[modelAtoms[i]] = dist
                refPointsByDist[refAtoms[i]] = dist

            # Sort the dictionaries by distance
            modelPointsByDist = collections.OrderedDict(sorted(modelPointsByDist.items(), key=lambda x: x[1]))
            refPointsByDist   = collections.OrderedDict(sorted(refPointsByDist.items(), key=lambda x: x[1]))

            return modelPointsByDist, refPointsByDist

        else:  # Find distance between a point and closest point to it, using naive algorithm
            for m in modelAtoms:
                minDist = sys.maxint
                for r in refAtoms:
                    dist = math.sqrt((m.getPosition().x() - r.getPosition().x())**2 +
                                     (m.getPosition().y() - r.getPosition().y())**2 +
                                     (m.getPosition().z() - r.getPosition().z())**2)
                    if dist < minDist:
                        minDist = dist
                modelPointsByDist[m] = minDist

            for r in refAtoms:
                minDist = sys.maxint
                for m in modelAtoms:
                    dist = math.sqrt((m.getPosition().x() - r.getPosition().x())**2 +
                                     (m.getPosition().y() - r.getPosition().y())**2 +
                                     (m.getPosition().z() - r.getPosition().z())**2)
                    if dist < minDist:
                        minDist = dist
                refPointsByDist[r] = minDist

            # Sort the dictionaries by distance
            modelPointsByDist = collections.OrderedDict(sorted(modelPointsByDist.items(), key=lambda x: x[1]))
            refPointsByDist = collections.OrderedDict(sorted(refPointsByDist.items(), key=lambda x: x[1]))

            return modelPointsByDist, refPointsByDist

    # Computes the root-mean-square deviation of two lists of atoms
    # Input is one orderedDict of key: atom, val: distance
    def computeRMSD(self, modelAtoms, refAtoms):
        if len(modelAtoms) is 0 or len(modelAtoms) is 0:
            return 0
        else:
            sumOfSquares = 0
            for atom in modelAtoms:
                sumOfSquares += modelAtoms.get(atom)**2  # gets distance of current atom, squared
            for atom in refAtoms:
                sumOfSquares += refAtoms.get(atom)**2
            return math.sqrt((1/float(len(modelAtoms)+len(refAtoms)))*sumOfSquares)

    # Computes the Hausdorff distance for two atom models
    # Input is one sorted orderedDict of atoms containing atom : distance to corresponding atom in other model
    # Returns the last value in the orderedDict (largest distance between two atoms)
    def computeHausdorff(self, atoms):
        if len(atoms) == 0:
            return 0
        return atoms.get(atoms.keys()[-1])








