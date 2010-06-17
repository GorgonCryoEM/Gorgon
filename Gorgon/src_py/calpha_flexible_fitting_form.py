# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to fit a calpha backbone to the density

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.8  2010/05/27 05:19:31  ssa1
#   Moving all atoms when performing fitting instead of a single atom.
#
#   Revision 1.7  2010/05/26 21:53:21  ssa1
#   Adding in display styles for atom rendering.
#
#   Revision 1.6  2010/05/21 16:33:21  ssa1
#   Flexible fitting implemented in Gorgon
#
#   Revision 1.5  2010/05/21 16:11:45  ssa1
#   Flexible fitting implemented in Gorgon
#
#   Revision 1.4  2010/05/21 15:46:11  ssa1
#   Flexible fitting implemented in Gorgon
#
#   Revision 1.3  2010/05/21 15:45:16  ssa1
#   Flexible fitting implemented in Gorgon
#
#   Revision 1.2  2010/05/20 21:55:53  ssa1
#   Rigid body alignment based on largest flexible cluster
#
#   Revision 1.1  2010/05/20 19:15:15  ssa1
#   Flexible fitting interface.
#

from PyQt4 import QtCore, QtGui
from ui_dialog_calpha_flexible_fitting import Ui_DialogCAlphaFlexibleFitting
from base_dock_widget import BaseDockWidget
from libpyGORGON import FlexibleFittingEngine, SSECorrespondenceNode
from math import pi

class CAlphaFlexibleFittingForm(BaseDockWidget, Ui_DialogCAlphaFlexibleFitting):
        
    def __init__(self, main, viewer, parent=None):
        BaseDockWidget.__init__(self, 
                                main, 
                                "Fit to density", 
                                "Fit CAlpha atoms to density", 
                                "perform_CAlphaFlexibleFitting", 
                                "actions-calpha-flexiblefitting", 
                                "actions-calpha", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.RightDockWidgetArea, 
                                parent)
        self.app = main
        self.viewer = viewer
        self.cAlphaViewer = viewer
        self.volumeViewer = self.app.viewers["volume"]
        self.sseViewer = self.app.viewers["sse"]        
        self.createUI()
        self.enableDisableWindowElements()
        self.engine = FlexibleFittingEngine()
        self.chainHelixMapping = {}
        self.invChainHelixMapping = {}
        

    def createUI(self):
        self.setupUi(self)    
        self.connect(self.pushButtonLoadHelices, QtCore.SIGNAL("clicked (bool)"), self.loadHelices)
        self.connect(self.pushButtonLoadCAlpha, QtCore.SIGNAL("clicked (bool)"), self.loadBackbone)
        self.connect(self.cAlphaViewer, QtCore.SIGNAL("modelLoaded()"), self.enableDisableWindowElements)
        self.connect(self.cAlphaViewer, QtCore.SIGNAL("modelUnloaded()"), self.enableDisableWindowElements)
        self.connect(self.sseViewer, QtCore.SIGNAL("modelLoaded()"), self.enableDisableWindowElements)
        self.connect(self.sseViewer, QtCore.SIGNAL("modelUnloaded()"), self.enableDisableWindowElements)
        self.connect(self.comboBoxAlignment, QtCore.SIGNAL("currentIndexChanged (int)"), self.loadAlignment)
        self.tableWidget.setColumnWidth(0,80)
        self.tableWidget.setColumnWidth(1,80)
        self.tableWidget.setColumnWidth(2,80)
        self.connect(self.displayAct, QtCore.SIGNAL("toggled (bool)"), self.visibilityChanged)        
            
    def visibilityChanged(self, visible):
        self.sseViewer.renderer.setSSESpecificColoring(visible)
                    
    def enableDisableWindowElements(self):
        self.pushButtonLoadHelices.setVisible(not self.sseViewer.loaded)
        self.pushButtonLoadCAlpha.setVisible(not self.cAlphaViewer.loaded)
        allLoaded = self.sseViewer.loaded and self.cAlphaViewer.loaded
        self.tabWidget.setEnabled(allLoaded)
        self.buttonBox.setEnabled(allLoaded)

    def loadVolume(self, temp):
        self.app.actions.getAction("load_Volume").trigger()
        self.bringToFront()

    def loadHelices(self, temp):
        self.app.actions.getAction("load_SSE_Helix").trigger()
        self.bringToFront()

    def loadBackbone(self, temp):
        self.app.actions.getAction("load_CAlpha").trigger()
        self.bringToFront()
        
    def getColor(self, clusterIx, clusterCount):
        seed = clusterIx % 6
        loc =  float(clusterIx/6)/float(max(1,clusterCount/6))
        r = 0
        g = 0
        b = 0
        if(seed == 0):
            r = 1
            g = loc
        elif(seed == 1):
            r = 1.0 - loc
            g = 1
        elif(seed == 2):
            g = 1
            b = loc
        elif(seed == 3):
            g = 1.0 - loc
            b = 1
        elif(seed == 4):
            r = loc
            b = 1
        elif(seed == 5):
            r = 1
            b = 1.0 - loc;            
        
        return QtGui.QColor.fromRgba(QtGui.qRgba(r*255, g*255, b*255, 255))

    def loadAlignment(self, alignmentIx):
        self.tableWidget.clearContents ()
        if(self.comboBoxAlignment.count() > 0):
            for i in range(self.sseViewer.renderer.getHelixCount()):
                self.sseViewer.renderer.setHelixColor(i, 0.3, 0.3, 0.3, 1.0)
            
            clusterCount = self.engine.getClusterCount(alignmentIx)
            self.tableWidget.setRowCount(0)            
            for clusterIx in range(clusterCount):
                clusterColor = self.getColor(clusterIx, clusterCount)
                firstRow = True
                for helixIx in range(self.engine.getHelixCount(alignmentIx, clusterIx)):
                    rowIx = self.tableWidget.rowCount()
                    corr = self.engine.getCorrespondenceNode(alignmentIx, clusterIx, helixIx)
                    self.tableWidget.insertRow(rowIx)
                    self.tableWidget.setRowHeight(rowIx, 20)
                    if(firstRow):
                        self.tableWidget.setItem(rowIx, 0, QtGui.QTableWidgetItem(str(clusterIx+1)))
                    else:
                        self.tableWidget.setItem(rowIx, 0, QtGui.QTableWidgetItem(" "))
                    self.tableWidget.setItem(rowIx, 1, QtGui.QTableWidgetItem(str(self.invChainHelixMapping[corr.getPIndex()])))
                    self.tableWidget.setItem(rowIx, 2, QtGui.QTableWidgetItem(str(corr.getQIndex())))                    
                    self.tableWidget.item(rowIx, 0).setBackgroundColor(clusterColor)
                    self.tableWidget.item(rowIx, 1).setBackgroundColor(clusterColor)
                    self.tableWidget.item(rowIx, 2).setBackgroundColor(clusterColor)
                    self.sseViewer.renderer.setHelixColor(corr.getQIndex(), clusterColor.redF(), clusterColor.greenF(), clusterColor.blueF(), clusterColor.alphaF())
                    firstRow = False
                
            if(self.radioButtonRigidFitting.isChecked()):
                self.viewer.renderer.transformAllAtomLocations(self.engine.getRigidTransform(alignmentIx))
            else :
                self.doFlexibleDeformation(alignmentIx)
            self.sseViewer.emitModelChanged()
        
    def accept(self):
        self.engine.resetEngine()
        # pushing in PDB Helices to the engine
        self.chainHelixMapping = {}
        self.invChainHelixMapping = {}
        cppIndex = 0
        for chain in self.cAlphaViewer.loadedChains:
            for helixIx, helix in chain.helices.items():
                self.chainHelixMapping[helixIx] = cppIndex
                self.invChainHelixMapping[cppIndex] = helixIx
                self.engine.startPDBHelix()
                for i in range(helix.startIndex, helix.stopIndex+1):
                    self.engine.addPDBAtomLocation(chain[i].getAtom('CA').getPosition())
                self.engine.endPDBHelix()
                cppIndex = cppIndex + 1
                
        # pushing in SSEHunter helices to the engine
        for i in range(self.sseViewer.renderer.getHelixCount()):
            pt1 = self.sseViewer.renderer.getHelixCorner(i, 0)
            pt2 = self.sseViewer.renderer.getHelixCorner(i, 1)
            self.engine.addSSEHelix(pt1, pt2)
                    
        self.engine.startSearch(self.spinBoxJointAngleThreshold.value()* pi / 180.0, self.spinBoxDihedralAngleThreshold.value()* pi / 180.0, self.spinBoxHelixLengthThreshold.value(), self.spinBoxHelixCentroidThreshold.value(), self.spinBoxMaxAlignments.value())
        corrCount = self.engine.getCorrespondenceCount()
        self.comboBoxAlignment.clear()           
        if(corrCount > 0):
            self.tabAlignments.setEnabled(True)
            for i in range(corrCount):
                self.comboBoxAlignment.addItem(str(i+1))
            self.comboBoxAlignment.setCurrentIndex(0)
            self.tabWidget.setCurrentIndex(1)
        else:
            self.tabAlignments.setEnabled(False)
            print "No alignments found :( "
                                                    
        self.viewer.emitModelChanged()
                    
                    
    def doFlexibleDeformation(self, alignmentIx):
        # TODO: This method can be refactored using the chain.secels dictionary... this would stop the need to search for previous and next helices..        
        # Getting all helix transformations
        transforms = {}
        for chain in self.cAlphaViewer.loadedChains:
            for helixIx, helix in chain.helices.items():
                transform = self.engine.getHelixFlexibleTransform(alignmentIx, self.chainHelixMapping[helixIx]) 
                if(transform.getValue(3,3) > 0.5): # Checking if a transformation was found for this helix
                    transforms[helixIx] =  transform
                        
        # finding what the previous and next helices of each PDB atom is
        helixStarts = [];
        helixEnds = [];            
        prevHelices = {};
        nextHelices = {};            
        for chain in self.cAlphaViewer.loadedChains:
            for i in range(min(chain.residueRange()), max(chain.residueRange())+1):
                if not (i in prevHelices):
                    prevHelices[i] = -1
                    nextHelices[i] = -1            
            for helixIx, helix in chain.helices.items():                
                if helixIx in transforms:
                    helixStarts.append(helix.startIndex)
                    helixEnds.append(helix.stopIndex)
                    
                    #Marking helix sections                
                    for i in range(helix.startIndex, helix.stopIndex+1):
                        prevHelices[i] = helixIx
                        nextHelices[i] = helixIx
                    #Marking region before current helix
                    found = False
                    for i in range(min(chain.residueRange()), helix.startIndex)[::-1]:
                        found = found or ((prevHelices[i] == nextHelices[i]) and (prevHelices[i] != -1))
                        if found:
                            break;
                        else:
                            nextHelices[i] = helixIx
                    #Marking region after current helix
                    found = False
                    for i in range(helix.stopIndex+1, max(chain.residueRange())+1):
                        found = found or ((prevHelices[i] == nextHelices[i]) and (prevHelices[i] != -1))
                        if found:
                            break;
                        else:
                            prevHelices[i] = helixIx                                                  
        
        # Building coefficient numbers
        helixStarts.sort()
        helixEnds.sort()
        coeff = {}        
        for i in range(len(helixStarts)-1):
            loopLength = helixStarts[i+1] - helixEnds[i] - 2;
            for j in range(helixEnds[i]+1, helixStarts[i+1]):
                if loopLength <= 0:
                    coeff[j] = 0.5
                else:
                    coeff[j] =  float(j-helixEnds[i]+1)/float(loopLength)
                    
         
        # Flexible deformation of each atom
        for chain in self.cAlphaViewer.loadedChains:
            for i in chain.residueRange():
                if i in chain.residueList:
                    for atomName in chain[i].getAtomNames():
                        if (prevHelices[i] == -1) and (nextHelices[i] == -1) : #No helices in structure
                            pass
                        elif (prevHelices[i] == nextHelices[i]) : # We are working with a helix atom
                            transform = transforms[prevHelices[i]]
                            chain[i].getAtom(atomName).transform(transform)
                        elif (prevHelices[i] == -1): # We are working with the beginning of the sequence
                            transform = transforms[nextHelices[i]]
                            chain[i].getAtom(atomName).transform(transform)
                        elif (nextHelices[i] == -1): # We are working with the end of the sequence
                            transform = transforms[prevHelices[i]]
                            chain[i].getAtom(atomName).transform(transform)     
                        else: # We are working with a loop segment between two helices                    
                            transform1 = transforms[prevHelices[i]]
                            transform2 = transforms[nextHelices[i]]
                            chain[i].getAtom(atomName).interpolateTransform(transform1, transform2, coeff[i])     
                    
    
    def reject(self):
        self.showWidget(False)