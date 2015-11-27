# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to fit a calpha backbone to the density


from PyQt4 import QtCore, QtGui
from ui_dialog_calpha_flexible_fitting import Ui_DialogCAlphaFlexibleFitting
from base_dock_widget import BaseDockWidget
from libpyGORGON import FlexibleFittingEngine, SSECorrespondenceNode
from math import pi
from copy import deepcopy

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
        self.rigidDone = False   
        self.createUI()
        self.enableDisableWindowElements()
        self.engine = FlexibleFittingEngine()
        self.chainHelixMapping = {}
        self.invChainHelixMapping = {}
        self.backupPositions = {}

        
        

    def createUI(self):
        self.setupUi(self)    
        self.connect(self.pushButtonLoadHelices, QtCore.SIGNAL("clicked (bool)"), self.loadHelices)
        self.connect(self.pushButtonLoadCAlpha, QtCore.SIGNAL("clicked (bool)"), self.loadBackbone)
        self.connect(self.cAlphaViewer, QtCore.SIGNAL("modelLoaded()"), self.enableDisableWindowElements)
        self.connect(self.cAlphaViewer, QtCore.SIGNAL("modelUnloaded()"), self.enableDisableWindowElements)
        self.connect(self.sseViewer, QtCore.SIGNAL("modelLoaded()"), self.enableDisableWindowElements)
        self.connect(self.sseViewer, QtCore.SIGNAL("modelUnloaded()"), self.enableDisableWindowElements)
        self.connect(self.comboBoxAlignment, QtCore.SIGNAL("currentIndexChanged (int)"), self.loadAlignment)
        self.connect(self.comboBoxCluster, QtCore.SIGNAL("currentIndexChanged (int)"), self.alignToCluster)
        self.connect(self.cAlphaViewer, QtCore.SIGNAL("ribbonSelected (int, PyQt_PyObject, PyQt_PyObject, QMouseEvent)"), self.processElementSelected)
        self.connect(self.sseViewer, QtCore.SIGNAL("SSE selected"), self.processElementSelected)
        self.connect(self.sseViewer, QtCore.SIGNAL("SSERightClicked(PyQt_PyObject, PyQt_PyObject, QMouseEvent)"), self.sseRightClicked)
        self.tableWidget.setColumnWidth(0,80)
        self.tableWidget.setColumnWidth(1,80)
        self.tableWidget.setColumnWidth(2,80)
        self.connect(self.displayAct, QtCore.SIGNAL("toggled (bool)"), self.visibilityChanged)        
            
    def visibilityChanged(self, visible):
        self.sseViewer.renderer.setObjectSpecificColoring(visible)
        if(visible):
            self.getBackupCopy()
        self.sseViewer.emitModelChanged()
                    
    def enableDisableWindowElements(self):
        self.pushButtonLoadHelices.setVisible(not self.sseViewer.loaded)
        self.pushButtonLoadCAlpha.setVisible(not self.cAlphaViewer.loaded)
        allLoaded = self.sseViewer.loaded and self.cAlphaViewer.loaded
        self.tabWidget.setEnabled(allLoaded)
        self.tabAlignments.setEnabled(self.rigidDone)
        self.tabFlexibleFit.setEnabled(self.rigidDone)
        self.pushButtonOk.setEnabled(allLoaded)
        self.pushButtonReset.setEnabled(allLoaded)

    def loadVolume(self, temp):
        self.app.actions.getAction("load_Volume").trigger()
        self.bringToFront()

    def loadHelices(self, temp):
        self.app.actions.getAction("load_SSE_Helix").trigger()
        self.bringToFront()

    def loadBackbone(self, temp):
        self.app.actions.getAction("load_CAlpha").trigger()
        self.bringToFront()
        self.getBackupCopy()
        
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
    
    def getBackupCopy(self):
        self.backupPositions = {}
        for chain in self.cAlphaViewer.loadedChains:           
            for i in chain.residueRange():
                if i in chain.residueList:
                    for atomName in chain[i].getAtomNames():
                        atom = chain[i].getAtom(atomName)
                        self.backupPositions[atom.getHashKey()] = atom.getPosition()
                        
    def revertToBackupCopy(self):
        for hash, position in self.backupPositions.items():
            self.cAlphaViewer.renderer.getAtom(hash).setPosition(position)
        self.cAlphaViewer.renderer.updateBoundingBox()
        self.cAlphaViewer.emitModelChanged()     
        self.rigidDone = False   
        self.enableDisableWindowElements()
        

    def loadAlignment(self, alignmentIx):
        self.tableWidget.clearContents ()
        self.comboBoxCluster.clear()
        self.comboBoxMerge1.clear()
        self.comboBoxMerge2.clear()
        if(self.comboBoxAlignment.count() > 0):
            for i in range(self.sseViewer.renderer.getHelixCount()):
                self.sseViewer.renderer.setHelixColor(i, 0.3, 0.3, 0.3, 1.0)
            
            clusterCount = self.engine.getClusterCount(alignmentIx)
            self.tableWidget.setRowCount(0)            
            for clusterIx in range(clusterCount):
                self.comboBoxCluster.addItem(str(clusterIx+1))
                self.comboBoxMerge1.addItem(str(clusterIx+1))
                self.comboBoxMerge2.addItem(str(clusterIx+1))
                
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
                        
                    self.tableWidget.setItem(rowIx, 1, QtGui.QTableWidgetItem(str(corr.getQIndex())))
                    self.tableWidget.setItem(rowIx, 2, QtGui.QTableWidgetItem(str(self.invChainHelixMapping[corr.getPIndex()])))
                                        
                    self.tableWidget.item(rowIx, 0).setBackgroundColor(clusterColor)
                    self.tableWidget.item(rowIx, 1).setBackgroundColor(clusterColor)
                    self.tableWidget.item(rowIx, 2).setBackgroundColor(clusterColor)
                    self.sseViewer.renderer.setHelixColor(corr.getQIndex(), clusterColor.redF(), clusterColor.greenF(), clusterColor.blueF(), clusterColor.alphaF())
                    self.cAlphaViewer.renderer.setHelixColor(corr.getPIndex(), clusterColor.redF(), clusterColor.greenF(), clusterColor.blueF())
                    firstRow = False
                    
                    
                
            flips = self.engine.getCorrespondenceFlips(alignmentIx)
            self.sseViewer.renderer.setSSEOrientationFlips(flips)           
            self.cAlphaViewer.setStrandVisibility(False)
            self.cAlphaViewer.setLoopVisibility(False)
            self.cAlphaViewer.setDisplayStyle(self.viewer.DisplayStyleRibbon)  
            self.cAlphaViewer.renderer.updateBoundingBox()
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
        self.rigidDone = True
        self.enableDisableWindowElements()

        
        
    def on_pushButtonRunFlexible_released(self):
        try:
            alignmentIx = int(self.comboBoxAlignment.itemText(self.comboBoxAlignment.currentIndex()))-1
        except:
            return

        self.engine.saveCorrs()
        self.engine.resetEngine()           
          
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
        self.engine.loadSavedCorrs();
        
        if(self.radioButtonInterpolation.isChecked()):
            self.showInterpolation(alignmentIx)                    
        else :
            self.doFlexibleDeformationPiecewise(alignmentIx)
        self.cAlphaViewer.renderer.updateBoundingBox()
        self.cAlphaViewer.setStrandVisibility(True)
        self.cAlphaViewer.setLoopVisibility(True)


        self.sseViewer.renderer.updateBoundingBox()
        self.sseViewer.emitModelChanged()     
        
  
    
    def showInterpolation(self, alignmentIx):
        softHandleLocs = self.doInterpolation(alignmentIx)
        for chain in self.cAlphaViewer.loadedChains:
            for i in chain.residueRange():
                chain[i].getAtom('CA').setPosition(softHandleLocs[i])
    
    def doFlexibleDeformationPiecewise(self, alignmentIx):
        engine = self.engine
        # Getting all helix transformations        
        transforms = {}
        origChain = []
        count = 0
        for chain in self.cAlphaViewer.loadedChains:  
            origChain.append({})
            for i in chain.residueRange():
                #debug code
                print "i:", i
                temp1 = chain[i]
                print "temp1:", temp1
                temp2 = temp1.getAtom('CA')
                print "temp2:", temp2
                temp3 = temp2.getPosition()
                print "temp3:", temp3
                origChain[count][i] = temp3
                #origChain[count][i] = chain[i].getAtom('CA').getPosition()
            count = count +1          
            for helixIx, helix in chain.helices.items():
                transform = self.engine.getHelixFlexibleTransform(alignmentIx, self.chainHelixMapping[helixIx]) 
                if(transform.getValue(3,3) > 0.5): # Checking if a transformation was found for this helix
                    transforms[helixIx] =  transform
    
        
       
        #Getting curve interpolated points
        softHandleLocs = self.doInterpolation(alignmentIx)
        if( not self.useSoftHandlesCheckBox.isChecked()):
            for ind in softHandleLocs.keys():
                softHandleLocs[ind] = [0,0,0]
                
        doingRigid = self.doRigidInitializationCheckBox.isChecked()
        
        # START NEW DEFORMATION HERE!
       
        #Setting weights
        engine.setLaplacainW(self.laplacianWeightDoubleSpinBox.value())
        engine.setHardHandleW(self.hardHandleWeightDoubleSpinBox.value())
        engine.setSoftHandleW(self.softHandleWeightDoubleSpinBox.value())
        
        #---Ends---#
        #Ends are done separately from the middle section of the chain.  Each the first and last vertices are given the same
        #transformation as the closest helix        
        for chain in self.cAlphaViewer.loadedChains:
            #Front
            engine.prepareDeform();
            helixIx = min(transforms.keys())
            minIndex = min(chain.residueRange())
            trans = transforms[helixIx]
            helix = chain.helices[helixIx]
            for i in range(minIndex, helix.stopIndex+1):
                engine.addAtomLocation(chain[i].getAtom('CA').getPosition())
                if i == minIndex or i in range(helix.startIndex, helix.stopIndex+1):
                    newpt = chain[i].getAtom('CA').getPosition().Transform(trans)
                    engine.addHardHandleLocation(newpt)
                    engine.addSoftHandleLocation([0,0,0])
                else:
                    engine.addHardHandleLocation([0,0,0])
                    newpt = softHandleLocs[i]
                    engine.addSoftHandleLocation(newpt)
                    
            rigidInit = engine.getPairRigidTransform(alignmentIx, self.chainHelixMapping[helixIx], self.chainHelixMapping[helixIx])
            flatVertices = engine.Deform(self.neighborhoodSizeSpinBox.value(), rigidInit, doingRigid)
            finalVertices = [];
#            for j in range(0, len(flatVertices), 3):
#                finalVertices.append(tuple([flatVertices[j], flatVertices[j+1], flatVertices[j+2]]));
                
            count = 0
            for k in range(minIndex, helix.stopIndex+1):
                if k not in range(helix.startIndex, helix.stopIndex+1):
                    chain[k].getAtom('CA').setPosition(flatVertices[count])
                count = count +1
                
            #back   
            engine.prepareDeform();
            helixIx = max(transforms.keys())
            maxIndex = max(chain.residueRange())
            trans = transforms[helixIx]
            helix = chain.helices[helixIx]
            for i in range(helix.startIndex,maxIndex+1):
                engine.addAtomLocation(chain[i].getAtom('CA').getPosition())
                if i == maxIndex or i in range(helix.startIndex, helix.stopIndex+1):
                    newpt = chain[i].getAtom('CA').getPosition().Transform(trans)
                    engine.addHardHandleLocation(newpt)
                    engine.addSoftHandleLocation([0,0,0])
                else:
                    engine.addHardHandleLocation([0,0,0])
                    newpt = softHandleLocs[i]
                    engine.addSoftHandleLocation(newpt)
                    
            rigidInit = engine.getPairRigidTransform(alignmentIx, self.chainHelixMapping[helixIx], self.chainHelixMapping[helixIx])
            flatVertices = engine.Deform(self.neighborhoodSizeSpinBox.value(), rigidInit, doingRigid)
            finalVertices = []
#            for j in range(0, len(flatVertices), 3):
#                finalVertices.append(tuple([flatVertices[j], flatVertices[j+1], flatVertices[j+2]]));
                
            count = 0
            for k in range(helix.startIndex, maxIndex+1):
                if k not in range(helix.startIndex, helix.stopIndex+1):
                    chain[k].getAtom('CA').setPosition(flatVertices[count])
                count = count +1
            
        
        #All middle Chains
        for chain in self.cAlphaViewer.loadedChains:
            for helixIx, trans in transforms.items():
                nextHelixIx = helixIx+1
                if helixIx == max(transforms.keys()):
                    break
                while nextHelixIx not in transforms.keys():
                    nextHelixIx = nextHelixIx+1
                helix1 = chain.helices[helixIx]
                helix2 = chain.helices[nextHelixIx]
                              
                engine.prepareDeform();               
                for i in range(helix1.startIndex, helix2.stopIndex+1):
                    engine.addAtomLocation(chain[i].getAtom('CA').getPosition())
                    if i in range(helix1.startIndex, helix1.stopIndex+1):
                        newpt = chain[i].getAtom('CA').getPosition().Transform(trans)
                        engine.addHardHandleLocation(newpt)
                        engine.addSoftHandleLocation([0,0,0])
                    elif i in range(helix2.startIndex, helix2.stopIndex+1):
                        newpt = chain[i].getAtom('CA').getPosition().Transform(transforms[nextHelixIx])
                        engine.addHardHandleLocation(newpt)
                        engine.addSoftHandleLocation([0,0,0])
                    else:
                        engine.addHardHandleLocation([0,0,0])
                        newpt = softHandleLocs[i]
                        engine.addSoftHandleLocation(newpt)        
                rigidInit = engine.getPairRigidTransform(alignmentIx, self.chainHelixMapping[helixIx], self.chainHelixMapping[nextHelixIx])
                flatVertices = engine.Deform(self.neighborhoodSizeSpinBox.value(), rigidInit, doingRigid)
                finalVertices = [];
#                for j in range(0, len(flatVertices), 3):
#                    zz = tuple([flatVertices[j], flatVertices[j+1], flatVertices[j+2]]);
#                    finalVertices.append(zz);
                
                #Only change the non-helix locations.  Helices have to stay in original locations in order to accurately
                #calculate all transforms.  Helices get moved at the end    
                count = 0
                for k in range(helix1.startIndex, helix2.stopIndex+1):
                    if k not in range(helix1.startIndex, helix1.stopIndex+1) and k not in range(helix2.startIndex, helix2.stopIndex+1):
                        chain[k].getAtom('CA').setPosition(flatVertices[count])
                    count = count +1
                    
        #move the handle points
        for chain in self.cAlphaViewer.loadedChains:
            for helixIx, trans in transforms.items():
                helix = chain.helices[helixIx]
                for i in range(helix.startIndex, helix.stopIndex+1):
                    newpt = chain[i].getAtom('CA').getPosition() 
                    newpt = newpt.Transform(trans)                
                    chain[i].getAtom('CA').setPosition(newpt)        
                                  
        self.updateEngineFeatureLists(alignmentIx)
        
        #transforming side chains
        scTransforms = {}
        for chainIx in range(0, len(origChain)):
            chain1 = origChain[chainIx]
            chain2 = self.cAlphaViewer.loadedChains[chainIx]
            locations1 = []
            locations2 = []
            chainMin = min(origChain[chainIx].keys())
            chainMax = max(origChain[chainIx].keys())
            for i in range(chainMin,chainMax +1):
                locations1 = []
                locations2 = []
                minIndex = i-2
                maxIndex = i+2
                if minIndex < chainMin:
                    minIndex = chainMin
                if maxIndex > chainMax:
                    maxIndex = chainMax
                for j in range(minIndex, maxIndex+1):
                    locations1.append(chain1[i])
                    locations2.append(chain2[i].getAtom('CA').getPosition())
                trans = self.engine.getSideChainTransform(locations1, locations2)
                scTransforms[i] = trans;
            for i in range(chainMin,chainMax +1):
                atomTrans = scTransforms[i]
                for atomName in self.cAlphaViewer.loadedChains[chainIx][i].getAtomNames():                    
                    if atomName != 'CA':
                        chain2[i].getAtom(atomName).transform(atomTrans)                        
                        pass
        self.cAlphaViewer.emitModelChanged()
              
                            
                   
    def doInterpolation(self, alignmentIx):                    
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
                    
         
        transformedAtoms = {} 
        # Flexible deformation of each atom
        for chain in self.cAlphaViewer.loadedChains:
            for i in chain.residueRange():
                if i in chain.residueList:
                    if (prevHelices[i] == -1) and (nextHelices[i] == -1) : #No helices in structure
                        pass
                    elif (prevHelices[i] == nextHelices[i]) : # We are working with a helix atom
                        transform = transforms[prevHelices[i]]
                        transformedAtoms[i] = (chain[i].getAtom('CA').getPosition().Transform(transform))
                    elif (prevHelices[i] == -1): # We are working with the beginning of the sequence
                        transform = transforms[nextHelices[i]]
                        transformedAtoms[i] = (chain[i].getAtom('CA').getPosition().Transform(transform))
                    elif (nextHelices[i] == -1): # We are working with the end of the sequence
                        transform = transforms[prevHelices[i]]
                        transformedAtoms[i] = (chain[i].getAtom('CA').getPosition().Transform(transform))
                    else: # We are working with a loop segment between two helices                    
                        transform1 = transforms[prevHelices[i]]
                        transform2 = transforms[nextHelices[i]]
                        transformedAtoms[i] = (chain[i].getAtom('CA').getInterpolateTransformLocation(transform1, transform2, coeff[i]))    
        return transformedAtoms 
                                  
                    
    def reject(self):
        self.engine.resetEngine()
        self.chainHelixMapping = {}
        self.invChainHelixMapping = {}

        self.comboBoxAlignment.clear()           
        self.tabWidget.setCurrentIndex(0)
        self.tableWidget.clearContents()
        
        helixColor = self.sseViewer.getModelColor()
        for helixIx in range(self.sseViewer.renderer.getHelixCount()):
            self.sseViewer.renderer.setHelixColor(helixIx, helixColor.redF(), helixColor.greenF(), helixColor.blueF(), helixColor.alphaF())
        self.sseViewer.emitModelChanged()
                
        self.revertToBackupCopy()
        
        
        
    def on_pushButtonMatchHelices_clicked(self):
        sseHelices = self.sseViewer.renderer.getSelectedHelixIndices()
        calphaHelices = self.cAlphaViewer.renderer.getSelectedHelixIndices()
        if len(sseHelices) != 1 or len(calphaHelices) != 1:
            print "please select exactly one sseHelix and one cAlphaHelix"
        else:
            alignmentIx = int(self.comboBoxAlignment.itemText(self.comboBoxAlignment.currentIndex()))-1
            self.engine.addCorrespondence(alignmentIx, sseHelices[0], calphaHelices[0])
            self.updateTable(alignmentIx)
            crrs = self.engine.getAllCorrespondencesFlat(alignmentIx)
            self.cAlphaViewer.renderer.setHelixCorrs(crrs)
            self.sseViewer.renderer.setHelixCorrs(crrs)
            self.cAlphaViewer.emitModelChanged()
            self.sseViewer.emitModelChanged()
            
    def on_pushButtonMergeClusters_clicked(self):
        
        try:
            cluster1 = int(self.comboBoxMerge1.itemText(self.comboBoxMerge1.currentIndex()))-1
            cluster2 = int(self.comboBoxMerge2.itemText(self.comboBoxMerge2.currentIndex()))-1
            alignmentIx = int(self.comboBoxAlignment.itemText(self.comboBoxAlignment.currentIndex()))-1
        except:
            return
        if cluster1 != cluster2:
            self.engine.mergeClusters(alignmentIx, cluster1, cluster2)
        self.updateTable(alignmentIx)
        crrs = self.engine.getAllCorrespondencesFlat(alignmentIx)
        self.cAlphaViewer.renderer.setHelixCorrs(crrs)
        self.sseViewer.renderer.setHelixCorrs(crrs)
        self.sseViewer.emitModelChanged()
        self.cAlphaViewer.emitModelChanged()

        
        
    def ribbonClicked(self, subsceneIx, sseIx, subSseIx, event):
        self.processElementSelected()
        print "Ribbon", subsceneIx, sseIx, subSseIx
        
        
        
    def updateTable(self, alignmentIx):
        self.tableWidget.clearContents()
        self.comboBoxMerge1.clear()
        self.comboBoxMerge2.clear()
        self.comboBoxCluster.clear()
        if(self.comboBoxAlignment.count() > 0):
            for i in range(self.sseViewer.renderer.getHelixCount()):
                self.sseViewer.renderer.setHelixColor(i, 0.3, 0.3, 0.3, 1.0)
            
            clusterCount = self.engine.getClusterCount(alignmentIx)
            self.tableWidget.setRowCount(0)            
            for clusterIx in range(clusterCount):
                self.comboBoxCluster.addItem(str(clusterIx+1))
                self.comboBoxMerge1.addItem(str(clusterIx+1))
                self.comboBoxMerge2.addItem(str(clusterIx+1))
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
                        
                    self.tableWidget.setItem(rowIx, 1, QtGui.QTableWidgetItem(str(corr.getQIndex())))
                    self.tableWidget.setItem(rowIx, 2, QtGui.QTableWidgetItem(str(self.invChainHelixMapping[corr.getPIndex()])))
                                        
                    self.tableWidget.item(rowIx, 0).setBackgroundColor(clusterColor)
                    self.tableWidget.item(rowIx, 1).setBackgroundColor(clusterColor)
                    self.tableWidget.item(rowIx, 2).setBackgroundColor(clusterColor)
                    self.sseViewer.renderer.setHelixColor(corr.getQIndex(), clusterColor.redF(), clusterColor.greenF(), clusterColor.blueF(), clusterColor.alphaF())
                    self.cAlphaViewer.renderer.setHelixColor(corr.getPIndex(), clusterColor.redF(), clusterColor.greenF(), clusterColor.blueF())
                    firstRow = False
                    
    def processElementSelected(self):
        inds = self.sseViewer.renderer.getSelectedHelixIndices()
        inds2 = self.cAlphaViewer.renderer.getSelectedHelixIndices()
        try:
            alignmentIx = int(self.comboBoxAlignment.itemText(self.comboBoxAlignment.currentIndex()))-1
        except:
            return
        crrs = self.engine.getAllCorrespondencesFlat(alignmentIx)
        self.cAlphaViewer.renderer.setHelixCorrs(crrs)
        self.cAlphaViewer.renderer.setSelectedSSEHelices(inds)
        
        self.sseViewer.renderer.setHelixCorrs(crrs)
        self.sseViewer.renderer.setSelectedPDBHelices(inds2)
        self.sseViewer.emitModelChanged()
        self.cAlphaViewer.emitModelChanged()
        
        
    def sseRightClicked(self, subscene, index, event):
        print "right click registered"
        inds = self.sseViewer.renderer.getSelectedHelixIndices()
        if(index in inds):
            menu = QtGui.QMenu(self.tr("Flip SSE " + str(index) ))
            flipAction = QtGui.QAction(self.tr("Flip SSE # " + str(index) ), self)
            self.flippingIndex = index
            self.connect(flipAction, QtCore.SIGNAL("triggered()"), self.flipSSE)       
            menu.addAction(flipAction)
            menu.exec_(self.app.mainCamera.mapToGlobal(self.app.mainCamera.mouseDownPoint))
            self.app.mainCamera.updateGL()
            
    def flipSSE(self):
        print "flipping sse"
        print self.flippingIndex
        try:
            alignmentIx = int(self.comboBoxAlignment.itemText(self.comboBoxAlignment.currentIndex()))-1
        except:
            return
        self.engine.flipCorrespondencePair(alignmentIx, self.flippingIndex)
        flips = self.engine.getCorrespondenceFlips(alignmentIx)
        self.sseViewer.renderer.setSSEOrientationFlips(flips)
        self.sseViewer.emitModelChanged()
        
    def alignToCluster(self, index):
        try:
            alignmentIx = int(self.comboBoxAlignment.itemText(self.comboBoxAlignment.currentIndex()))-1
        except:
            return
        
        if index != -1:
            self.engine.saveCorrs()
            self.engine.resetEngine()           
              
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
            self.engine.loadSavedCorrs();
            self.viewer.renderer.transformAllAtomLocations(self.engine.getRigidTransform2(alignmentIx, index))
        
   
        self.cAlphaViewer.visualizationOptions.updateFromViewer()
        self.cAlphaViewer.emitModelChanged()

        
    def updateEngineFeatureLists(self, corrIx):
        self.engine.saveCorrs()
        self.engine.resetEngine()           
          
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
        self.engine.loadSavedCorrs();
        fvecs = self.engine.getAllCAlphaFeatureVecsFlat()
        self.cAlphaViewer.renderer.setFeatureVecs(fvecs)
