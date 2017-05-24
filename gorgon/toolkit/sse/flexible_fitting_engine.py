from ..libpytoolkit import FlexibleFittingEngine, IBackEnd, SkeletonRenderer, PDBBond, Vec3F
from ...toolkit.sse.seq_model.Residue import Residue
from ..sse.seq_model.Helix import Helix
from ..sse.seq_model.Sheet import Sheet
from ..sse.seq_model.Coil import Coil
from ..sse.seq_model.Strand import Strand
from ...toolkit.sse.seq_model.Chain import Chain
import time


class FFEngine(object):
    __lastAuto_pdbID = 0

    def __init__(self, args):
        self.skeleton = args.skeleton
        self.correspondenceFile = args.correspondence
        self.helix = args.helix
        self.template = args.template
        self.sourceFragments = []
        self.outputFile = 'output.pdb'
        self.skeletonIterations = None
        if args.skeletonIterations is not None:
            self.skeletonIterations = int(args.skeletonIterations)
        self.helixDistortionTerm = None
        if args.helixDTerm is not None:
            self.helixDistortionTerm = float(args.helixDTerm)
        self.helixFittingTerm = None
        if args.helixFTerm is not None:
            self.helixFittingTerm = float(args.helixFTerm)
        self.skeletonDistance = None
        if args.skeletonDistance is not None:
            self.skeletonDistance = float(args.skeletonDistance)
        self.skeletonDistortionTerm = None
        if args.skeletonDTerm is not None:
            self.skeletonDistortionTerm = float(args.skeletonDTerm)
        self.skeletonFittingTerm = None
        if args.skeletonFTerm is not None:
            self.skeletonFittingTerm = float(args.skeletonFTerm)
        self.laplacianNeighborhoodSize = None
        if args.laplacianNeighborhoodSize is not None:
            self.laplacianNeighborhoodSize = int(args.laplacianNeighborhoodSize)
        self.transformationNeighborhoodSize = None
        if args.transformationNeighborhoodSize is not None:
            self.transformationNeighborhoodSize = int(args.transformationNeighborhoodSize)
        self.chain = None
        self.flexibleFittingEngine = FlexibleFittingEngine()
        self.main_chain = []
        self.loadedChains = []
        self.chainsDict = {}
        self.bonds = []
        self.chains = []
        self.pdbBonds = []
        self.backupChainAtomPositions = {}
        self.constants = IBackEnd()
        self.correspondenceLoaded = False
        self.correspondence = {'pIndex':[], 'qIndex':[], 'isForward':[]}
        self.correspondencePIndexToCalphaHelixIndex = {}

    def loadCurveFragments(self, sourceFragments):
        filename = unicode(self.skeleton)
        extension = filename.split('.')[-1].lower()
        if extension == 'off':
            lineNum = 0
            oldLine  = []
            vertices = []
            lineSeg  = []
            for line in open(filename, 'U'):
                lineNum += 1
                # Skip .off file header
                if lineNum < 3:
                    continue

                line = line.split(' ')
                # Store vertex coord
                if len(line) == 3:
                    vertices.append([float(line[0]), float(line[1]), float(line[2])])
                # Store Line seg
                elif len(line) == 5:
                    lineSeg.append(vertices[int(line[1])])
                # Done with one line seg
                elif len(line) == 1 and len(oldLine) == 5:
                    lineSeg.append(vertices[int(oldLine[3])])
                    sourceFragments.append(lineSeg)
                    lineSeg = []

                oldLine = line
        else:
            raise IOError, 'Wrong extension for source fragment file...'

    def loadSourceFragments(self):
        self.loadCurveFragments(self.sourceFragments)

    def loadOffFile(self):
        filename = unicode(self.skeleton)
        self.flexibleFittingEngine.loadOffFile(self.skeleton)

    def getChainIDsFromPDB(self, filename):
        """
        This only finds the first list of chains in the PDB file.  If the file 
        defines multiple molecules, this would only find the chains for the 
        first one.
        """
        extension = filename.split('.')[-1].lower()
        if extension == 'pdb':
            linelist = []
            for line in open(filename, 'U'):
                if line[:6] == 'COMPND' and line[10:70].split(':')[0].strip() == 'CHAIN':
                    linelist = line[17:].split(', ')
                    linelist[0] = linelist[0].strip()
                    if ';' in linelist[-1]:
                        linelist[-1] = linelist[-1].split(';')[
                            0]  # removes the terminating semicolon and extra whitespace
                        while True:
                            try:
                                linelist.remove('NULL')
                            except:
                                break
                    return linelist
            if linelist == []:
                return []
        else:
            return []

    def __createUniquePDBID(self):
        '''
        If no PDB ID is specified when creating a chain, this method will 
        create one that is simply a number preceded by enough underscores to
        give a 4 character identifier.
        '''
        # TODO:We might want to modify this to use any unused numbers (after a rename)
        pdbNum = self.__lastAuto_pdbID + 1
        numUnderscores = 4 - len(str(pdbNum))
        assert numUnderscores >= 0
        # TODO: We need to figure out how to handle this exception
        pdbID = '_' * numUnderscores + str(pdbNum)
        self.__lastAuto_pdbID += 1
        return pdbID

    def getChainKeys(self):
        '''
        This returns the keys (PDB ID, Chain ID) for all the chains loaded into
        memory.
        '''
        return self.chainsDict.keys()

    def setIDs(self, new_pdbID, new_chainID):
        """
        This changes the pdbID and chainID attributes of a Chain instance.
        """
        #### We need to figure out how to handle the possible exception
        assert (new_pdbID, new_chainID) != self.key
        value = self.chainsDict.pop(self.key)
        self.pdbID = new_pdbID
        self.chainID = new_chainID
        self.key = (self.pdbID, self.chainID)
        self.chainsDict[self.key] = value

    def getChain(self, key):
        '''
        Given a (pdbID, chainID) key, returns a chain object.
        '''
        return self.chainsDict.get(key)

    def __loadFromPDB(self, filename, whichChainID=None):
        '''
        This loads the specified chain ID from a PDF file and returns a Chain 
        object. If no chain ID is specified, it loads the first chain.
        '''
        result = Chain('')

        header = open(filename, 'U')
        headerLine = header.read()
        if headerLine[:6] == 'HEADER':
            pdbID = headerLine[62:66]
        else:
            pdbID = self.__createUniquePDBID()
        header.close()

        residue = None
        firstChain = None
        for line in open(filename,'U'):  # calls the iterator for the file object each time the loop is run - don't have to load entire file into memory
            if line[0:4] == 'ATOM':
                chainID = line[21:22]
                if chainID == ' ':
                    chainID = 'A'
                if whichChainID and chainID != whichChainID:  # Search for the specified chainID (if one is specified), otherwise we find the first chain.
                    continue
                if not firstChain:  # Sets the value of the first and only chain we will store
                    firstChain = chainID
                    ####if the chain key already exists, point to that chain object
                    ####perhaps this should be modified
                    if not (pdbID, firstChain) in self.getChainKeys():
                        result.setIDs(pdbID, firstChain)
                    else:
                        result = self.getChain((pdbID, firstChain))
                        break
                if firstChain and chainID != firstChain:  # If we've gone past the only chain we want to store, we will break out of the for loop
                    break

                residueIndex = int(line[22:26])
                if residueIndex not in result.residueRange():
                    residue = Residue(line[17:20].strip(), result)
                    result[residueIndex] = residue
                else:
                    residue = result[residueIndex]

                serialNo = int(line[6:11].strip())
                atomName = line[12:16].strip()
                element = line[76:78].strip()
                try:
                    tempFactor = float(line[60:66].strip())
                except ValueError:
                    tempFactor = None
                try:
                    occupancy = float(line[54:60].strip())
                except ValueError:
                    occupancy = None
                try:
                    x = float(line[30:38])
                    y = float(line[38:46])
                    z = float(line[46:54])
                    atom = residue.addAtom(atomName, x, y, z, element, serialNo, occupancy, tempFactor)
                    # residue.atoms[atomName]=atom
                    result.atoms[serialNo] = atom
                    # Chain.chainsDict[result.key] = result
                except ValueError:
                    print 'Chain.__loadFromPDB--no coordinates',

                result.differenceIndex = residueIndex - serialNo
            
            elif line[0:6].strip() == 'HELIX':
                Helix.parsePDB(line, result)
            elif line[0:6].strip() == 'SHEET':
                Sheet.parsePDB(line, result)

        # Setting up coils
        startList = []
        endList = []
        for secelIx, secel in result.secelList.items():
            startList.append(secel.startIndex)
            endList.append(secel.stopIndex)
        startList.sort()
        endList.sort()

        if result.residueRange():
            startPt = min(result.residueRange())
            coilIx = 1
            for i in range(len(startList)):
                if startPt < startList[i]:
                    result.addCoil(coilIx, Coil(result, coilIx, 'L' + str(coilIx), startPt, startList[i] - 1))
                    coilIx = coilIx + 1
                startPt = endList[i] + 1

            if startPt < max(result.residueRange()):
                result.addCoil(coilIx, Coil(result, coilIx, 'L' + str(coilIx), startPt, max(result.residueRange())))

            self.chainsDict[result.key] = result
            # Chain.setSelectedChainKey(result.getIDs())

        return result

    def load(self, filename, whichChainID=None):
        '''
        This calls the correct load method based on the file extension.
        '''
        extension = filename.split('.')[-1].lower()
        if extension == 'pdb':
            return self.__loadFromPDB(filename, whichChainID)
        else:
            return []

    def loadAllChains(self,filename):
        '''
        This loads all the chains specified in a PDB file.  
        '''
        chain = None
        pdbID = None
        chains = []
        chainIDs = self.getChainIDsFromPDB(filename)
        while True:
            try:
                chainIDs.remove('NULL')
            except:
                break
        if not chainIDs:
            chainIDs = [None]
        for whichChainID in chainIDs:
            chain = self.load(filename, whichChainID)
            if not pdbID:
                pdbID = chain.getPdbID()
            elif pdbID != chain.getPdbID():
                chain.setIDs(pdbID, chain.getChainID())  # Needed if PDB ID is auto-generated
            chains.append(chain.getIDs())
            self.chainsDict[chain.key] = chain
        return chains

    def getPdbID(self):
        '''
        This returns the PDB ID for the Chain.
        '''
        return self.pdbID

    def loadData(self, fileName):
        def setupChain(mychain):
            self.main_chain = mychain
            self.loadedChains.append(mychain)
            #Chain.setSelectedChainKey(mychain.getIDs())
            addedCAlpha = mychain.addCalphaBondsNoViewer()
            for i in range(len(addedCAlpha)):
                self.pdbBonds.append(addedCAlpha[i])

            addedSideChain = mychain.addSideChainBondsNoViewer()
            for i in range(len(addedSideChain)):
                self.pdbBonds.append(addedSideChain[i])
            for i in mychain.residueRange():
                for atomName in mychain[i].getAtomNames():
                    atom = mychain[i].getAtom(atomName)
                    if atom:
                        mychain[i].addAtomObject(atom)


        self.fileName = self.template
        fileNameTemp = self.fileName
        self.whichChainID = None
        filename = unicode(self.fileName)
        if filename.split('.')[-1].lower() == 'pdb':
            self.whichChainID = 'ALL'
            self.fileName = fileNameTemp
            
            if self.whichChainID == 'ALL':
                #mychainKeys = Chain.loadAllChains(str(self.fileName))
                #for chainKey in mychainKeys:
                #    setupChain(Chain.getChain(chainKey))
                mychains = Chain.loadAllActualChains(str(self.fileName))
                for j in range(len(mychains)):
                    setupChain(mychains[j])
            else:
                mychain = load(str(self.fileName), whichChainID=self.whichChainID)
                setupChain(mychain)
        self.chain = self.loadedChains[0]
        

    def loadFixedCorrespondence(self, filename, dropHelicesNum):

        extension = filename.split('.')[-1].lower()
        if extension == 'cor':
            for line in open(filename, 'U'):
                line = line.split(' ')
                if line[0] =='pIndex':
                    for i in line[1:]:
                        self.correspondence['pIndex'].append(int(i))
                elif line[0] == 'qIndex':
                    for i in line[1:]:
                        self.correspondence['qIndex'].append(int(i))
                elif line[0] == 'isForward':
                    for i in line[1:]:
                        self.correspondence['isForward'].append(bool(int(i)))
                else:
                    raise IOError, 'Invalid prefix in correspondence file...'
                    sys.exit(0)
        else:
            raise IOError, 'Wrong extension for correspondence file'
            sys.exit(0)

        if dropHelicesNum > 0:
            # Unifomrly drop the helices
            #print 'before:', self.correspondence['pIndex']
            n = len(self.correspondence['isForward'])
            k = dropHelicesNum
            # True means we choose to keep that match pair
            delta = n/k
            dropFlag = True
            mask = [True] * len(self.correspondence['isForward'])
            if delta < 2:
                dropFlag = False
                k = n-k

            remainOffset = n%(k+1)
            delta = n/(k+1)
            i = 0 if delta < 2 else delta

            while i < len(self.correspondence['isForward']) and k > 0:
                mask[i] = False
                i = i+delta+1 if remainOffset > 0 else i+delta
                remainOffset -= 1
                k -= 1

            if dropFlag:
                self.correspondence['isForward'] = [x for i, x in enumerate(self.correspondence['isForward']) if mask[i]]
                self.correspondence['pIndex'] = [x for i, x in enumerate(self.correspondence['pIndex']) if mask[i]]
                self.correspondence['qIndex'] = [x for i, x in enumerate(self.correspondence['qIndex']) if mask[i]]
            else:
                self.correspondence['isForward'] = [x for i, x in enumerate(self.correspondence['isForward']) if not mask[i]]
                self.correspondence['pIndex'] = [x for i, x in enumerate(self.correspondence['pIndex']) if not mask[i]]
                self.correspondence['qIndex'] = [x for i, x in enumerate(self.correspondence['qIndex']) if not mask[i]]

    def setParams(self):
        self.constants.setConstantString("SEQUENCE_FILE_NAME", self.template)
        self.constants.setConstantString("SEQUENCE_FILE_TYPE", "PDB")
        self.constants.setConstantString("VRML_HELIX_FILE_NAME", self.helix)


    def loadDefaultParams(self):
        # Graph Settings tab
        self.BorderMarginThreshold = 5
        self.EuclideanDistance = 0.0
        self.IncludeSheets = True
    
        self.ShowHelixCorners = False
        self.ShowSheetCorners = False
        self.ShowAllPaths = False
    
        # Matching Settings tab
        self.EuclideanToPDBRatio = 10.0
        self.AbsoluteDifference = True
        self.NormalizedDifference = False
        self.QuadraticError = False
    
        self.LoopImportance = 0.2
        self.EuclideanLoopUsedPenalty = 5.0
    
        self.HelixImportance = 1.0
        self.MissingHelices = False
        self.MissingHelixCount = 0
        self.HelixMissingPenalty = 5.0
        self.HelixMissingPenaltyScaled = 0.0
        self.EndHelixMissingPenalty = 5.0
    
        self.SheetImportance = 1.0
        self.MissingSheets = False
        self.MissingSheetCount = 0
        self.SheetMissingPenalty = 5.0
        self.SheetMissingPenaltyScaled = 0.0
        # 
        # # Results tab
        # self.ui.tableWidgetCorrespondenceList.clearContents()
        # self.ui.tabWidget.setCurrentIndex(0)
        # self.ui.comboBoxCorrespondences.setCurrentIndex(-1)
        
        # Sheet related
        self.MinSheetSize        = 10
        self.MaxSheetDistance    = 5.0
        self.SheetSelfLoopLength = 5.0
        self.SheetMergeThreshold = 3.0

        self.setParams()

    def accept(self):
        self.setParams()
        self.loadOffFile()
        self.loadData(self.template)

        self.loadFixedCorrespondence(self.correspondenceFile, 0)
        self.performFlexibleDeformation()
        pdbString = self.chain.toPDB(CAlphaPlaceholders=False)
        F = open(self.outputFile, 'w')
        F.write(pdbString)
        F.close()
        self.flexibleFittingEngine.exportSkeletonAsMRC()

    def backupChain(self):
        self.backupChainAtomPositions = {}
        for residueIndex in self.chain.residueRange():
            if residueIndex not in self.backupChainAtomPositions:
                self.backupChainAtomPositions[residueIndex] = {}

            for atomName in self.chain[residueIndex].getAtomNames():
                self.backupChainAtomPositions[residueIndex][atomName] = self.chain[residueIndex].getAtom(atomName).getPosition()

    def calculateMissingResidueIndices(self):
        missingResidueIndices      = {}
        missingResidueIndexMapping = {}

        mappedResidueIndex = 0
        for originalResidueIndex in self.chain.residueRange():
            calphaAtom = self.chain[originalResidueIndex].getAtom('CA')
            if calphaAtom:
                missingResidueIndexMapping[originalResidueIndex] = mappedResidueIndex
                mappedResidueIndex += 1
            else:
                missingResidueIndices[originalResidueIndex] = True

        for missingResidueIndex in missingResidueIndices.keys():
            for direction in (-1, 1):
                residueIndexIterator = missingResidueIndex
                while (residueIndexIterator >= self.chain.getFirstResidueIndex()):
                    residueIndexIterator += direction
                    if residueIndexIterator in missingResidueIndexMapping:
                        missingResidueIndexMapping[missingResidueIndex] = missingResidueIndexMapping[residueIndexIterator]
                        break

                if missingResidueIndex in missingResidueIndexMapping:
                    break

        return (missingResidueIndices, missingResidueIndexMapping)

    def calculateCorrespondences(self, temp=None):
        # Create Correspondence Index Mapping
        #print "in calculating correspondences"
        self.correspondencePIndexToCalphaHelixIndex = {}
        self.flexibleFittingEngine.loadHelixFileRenderer(self.helix)
        #print "helix file loaded"
        # Load Calpha Helixes
        
        correspondenceIndex = 0
        for helixIndex, helix in self.chain.helices.items():
            # Get Atom Positions
            atomPositions = []
            for residueIndex in range(helix.startIndex, helix.stopIndex + 1):
                if residueIndex in self.chain.residueRange():
                    atomPositions.append(self.chain[residueIndex].getAtom('CA').getPosition())

            if atomPositions:
                # Calculate Best Fit Line
                startPosition = Vec3F(0, 0, 0)
                endPosition   = Vec3F(0, 0, 0)
                #self.linearSolver.findBestFitLine(startPosition, endPosition, atomPositions)
                #print "before fitting"
                self.flexibleFittingEngine.findFitLine(startPosition, endPosition, atomPositions)
                #print "after fitting"
                # Add to Correspondence Index Mapping 
                self.correspondencePIndexToCalphaHelixIndex[correspondenceIndex] = helixIndex
                correspondenceIndex += 1

                # Add to Engine
                self.flexibleFittingEngine.addCorrespondenceSourceFeature(startPosition, endPosition)

                # print helixIndex,' (start, end):',startPosition.x(),',',startPosition.y(),',',startPosition.z()
                # print '                ',endPosition.x(),',',endPosition.y(),',',endPosition.z()
        #print self.flexibleFittingEngine.getHelixCount()
        for helixIndex in range(self.flexibleFittingEngine.getHelixCount()):
            startPosition = self.flexibleFittingEngine.getHelixCorner(helixIndex, 0)
            endPosition = self.flexibleFittingEngine.getHelixCorner(helixIndex, 1)
            print helixIndex,' (start, end):',startPosition.x(),',',startPosition.y(),',',startPosition.z()
            print '                ',endPosition.x(),',',endPosition.y(),',',endPosition.z()
            self.flexibleFittingEngine.addCorrespondenceTargetFeature(startPosition, endPosition)

        """
        # Load Helix Annotations
        for helixIndex in range(self.sseViewer.renderer.getHelixCount()):
            # Get Volume Helix Line
            startPosition = self.sseViewer.renderer.getHelixCorner(helixIndex, 0)
            endPosition   = self.sseViewer.renderer.getHelixCorner(helixIndex, 1)

            # print helixIndex,' (start, end):',startPosition.x(),',',startPosition.y(),',',startPosition.z()
            # print '                ',endPosition.x(),',',endPosition.y(),',',endPosition.z()

            # Add to Engine
            self.flexibleFittingEngine.addCorrespondenceTargetFeature(startPosition, endPosition)

        # Initialize Correspondence Options
        self.flexibleFittingEngine.setCorrespondenceOptions(
            self.spinBoxMaximumAlignments.value(), 
            self.spinBoxHelixLengthDifference.value(), 
            self.spinBoxHelixCentroidDifference.value(), 
            self.spinBoxJointAngleDifference.value(), 
            self.spinBoxDihedralAngleDifference.value()
        )
        """
        self.flexibleFittingEngine.calculateCorrespondences()
        for i in range(len(self.correspondence['pIndex'])):
            self.flexibleFittingEngine.addCorrespondencesByReadIn(self.correspondence['pIndex'][i], self.correspondence['qIndex'][i], self.correspondence['isForward'][i])
        

    def calculateCorrespondenceTransformationsWithClusterWSVD(self):
        correspondences               = {}
        correspondenceTransformations = {}
        correspondenceTargets         = {}
        selectedAlignmentIndex = int(self.comboBoxAlignment.currentText())
        clusterCount = self.flexibleFittingEngine.getCorrespondenceClusterCount(selectedAlignmentIndex)

        # Precompute the weight list for each correspondence
        self.flexibleFittingEngine.computeCorrespondencePairWiseWeight(selectedAlignmentIndex)
        for clusterIndex in range(clusterCount):
            featureCount = self.flexibleFittingEngine.getCorrespondenceFeatureCount(selectedAlignmentIndex, clusterIndex)
            for featureIndex in range(featureCount):
                # Get Correspondence
                correspondence = self.flexibleFittingEngine.getCorrespondence(selectedAlignmentIndex, clusterIndex, featureIndex)

                # Get Indices
                calphaHelixIndex = self.correspondencePIndexToCalphaHelixIndex[correspondence.getPIndex()]
                volumeHelixIndex = correspondence.getQIndex()

                # Get Transformation
                correspondenceTransformation = self.flexibleFittingEngine.getCorrespondenceFeatureTransformationWSVD(selectedAlignmentIndex, clusterIndex, featureIndex)

                # Cache Correpondence
                correspondences[calphaHelixIndex] = volumeHelixIndex

                # Cache Correspondence Transformation
                correspondenceTransformations[calphaHelixIndex] = correspondenceTransformation

                # Cache Correspondence Targets
                helix = self.chain.helices[calphaHelixIndex]
                for residueIndex in range(helix.startIndex, helix.stopIndex + 1):
                    if residueIndex in self.chain.residueRange():
                        if calphaHelixIndex not in correspondenceTargets:
                            correspondenceTargets[calphaHelixIndex] = {}

                        correspondenceTargets[calphaHelixIndex][residueIndex] = self.chain[residueIndex].getAtom('CA').getPosition().Transform(correspondenceTransformation)

        return (correspondences, correspondenceTransformations, correspondenceTargets)


    def calculateCorrespondenceTransformations(self):
        correspondences               = {}
        correspondenceTransformations = {}
        correspondenceTargets         = {}
        selectedAlignmentIndex = 0
        #selectedAlignmentIndex = int(self.comboBoxAlignment.currentText())
        #clusterCount = self.flexibleFittingEngine.getCorrespondenceClusterCount(selectedAlignmentIndex)
        clusterCount = self.flexibleFittingEngine.getClusterCount(selectedAlignmentIndex)
        for clusterIndex in range(clusterCount):
            featureCount = self.flexibleFittingEngine.getCorrespondenceFeatureCount(selectedAlignmentIndex, clusterIndex)
            for featureIndex in range(featureCount):
                # Get Correspondence
                correspondence = self.flexibleFittingEngine.getCorrespondenceNode(selectedAlignmentIndex, clusterIndex, featureIndex)
                # Get Indices
                calphaHelixIndex = self.correspondencePIndexToCalphaHelixIndex[correspondence.getPIndex()]
                
                volumeHelixIndex = correspondence.getQIndex()
                # Get Transformation
                correspondenceTransformation = self.flexibleFittingEngine.getCorrespondenceFeatureTransformation(selectedAlignmentIndex, clusterIndex, featureIndex)
                # Cache Correpondence
                correspondences[calphaHelixIndex] = volumeHelixIndex

                # Cache Correspondence Transformation
                correspondenceTransformations[calphaHelixIndex] = correspondenceTransformation
               
                # Cache Correspondence Targets
                helix = self.chain.helices[calphaHelixIndex]
                for residueIndex in range(helix.startIndex, helix.stopIndex + 1):
                    if residueIndex in self.chain.residueRange():
                        if calphaHelixIndex not in correspondenceTargets:
                            correspondenceTargets[calphaHelixIndex] = {}

                        correspondenceTargets[calphaHelixIndex][residueIndex] = self.chain[residueIndex].getAtom('CA').getPosition().Transform(correspondenceTransformation)
        return (correspondences, correspondenceTransformations, correspondenceTargets)

    def ntuples(self, lst, n):
        tuples = zip(*[lst[i:]+lst[:i] for i in range(n)])
        # If there is only one element "e" in the lst, we return (e,e)
        if len(tuples) > 1:
            tuples.pop()

        return tuples

    def getClosestVertex(self, vertex, otherVertices, threshold, oneToOne):
        closestVertexIndex    = None
        closestVertex         = None
        closestVertexDistance = None
        for otherVertexIndex, otherVertex in enumerate(otherVertices):
            vertexDistance = (vertex - otherVertex).length()

            if (threshold == None or vertexDistance <= threshold) and (closestVertexDistance == None or vertexDistance < closestVertexDistance):
                closestVertexIndex    = otherVertexIndex
                closestVertex         = otherVertex
                closestVertexDistance = vertexDistance

        if oneToOne and closestVertexIndex != None:
            otherVertices.pop(closestVertexIndex)

        return closestVertex

    def calculateRigidInitializationTransformations(self, correspondences, correspondenceTransformations, correspondenceTargets, missingResidueIndexMapping):
        rigidInitializationTransformations = {}
        
        correspondedCalphaHelixIndices = sorted(correspondences.keys())
        
        # Loops
        for currentCalphaHelixIndex, nextCalphaHelixIndex in self.ntuples(correspondedCalphaHelixIndices, 2):
            # Get Helices
            currentCalphaHelix = self.chain.helices[currentCalphaHelixIndex]
            nextCalphaHelix    = self.chain.helices[nextCalphaHelixIndex]

            # Get Source/Target Positions
            sourceCalphaHelixPositions = []
            targetCalphaHelixPositions = []
            for currentResidueIndex in range(currentCalphaHelix.startIndex, currentCalphaHelix.stopIndex + 1):
                if currentResidueIndex in self.chain.residueRange():
                    sourceCalphaHelixPositions.append(self.chain[currentResidueIndex].getAtom('CA').getPosition())
                    targetCalphaHelixPositions.append(correspondenceTargets[currentCalphaHelixIndex][currentResidueIndex])
            for nextResidueIndex in range(nextCalphaHelix.startIndex, nextCalphaHelix.stopIndex + 1):
                if nextResidueIndex in self.chain.residueRange():
                    sourceCalphaHelixPositions.append(self.chain[nextResidueIndex].getAtom('CA').getPosition())
                    targetCalphaHelixPositions.append(correspondenceTargets[nextCalphaHelixIndex][nextResidueIndex])

            # Calculate Rigid Initialization Transformation
            rigidInitializationTransformation = self.flexibleFittingEngine.findRotationTranslation(sourceCalphaHelixPositions, targetCalphaHelixPositions)

            # Store Rigid Initialization Transformation
            if currentCalphaHelixIndex == correspondedCalphaHelixIndices[0]:
                startResidueIndex = self.chain.getFirstResidueIndex()
            else:
                startResidueIndex = currentCalphaHelix.startIndex

            if nextCalphaHelixIndex == correspondedCalphaHelixIndices[-1]:
                stopResidueIndex = self.chain.getLastResidueIndex() + 2
            else:
                stopResidueIndex = nextCalphaHelix.startIndex

            for residueIndex in range(startResidueIndex, stopResidueIndex + 1):
                if residueIndex in self.chain.residueRange():
                    rigidInitializationTransformations[residueIndex] = rigidInitializationTransformation

        # Helices
        for calphaHelixIndex in correspondedCalphaHelixIndices:
            # Get Helix
            calphaHelix = self.chain.helices[calphaHelixIndex]

            # Get Helix Transformation
            helixTransformation = correspondenceTransformations[calphaHelixIndex]

            # Store Rigid Initialization Transformation
            for residueIndex in range(calphaHelix.startIndex, calphaHelix.stopIndex + 1):
                if residueIndex in self.chain.residueRange():
                    rigidInitializationTransformations[residueIndex] = helixTransformation

        # SheetscomboBoxAlignment
        for sheetNo, sheet in sorted(self.chain.sheets.items()):
            # Get Connected Helices
            connectedCalphaHelixIndices = set()
            for strandNo, strand in sheet.strandList.items():
                for currentCalphaHelixIndex, nextCalphaHelixIndex in self.ntuples(correspondedCalphaHelixIndices, 2):
                    currentCalphaHelix = self.chain.helices[currentCalphaHelixIndex]
                    nextCalphaHelix    = self.chain.helices[nextCalphaHelixIndex]

                    if currentCalphaHelixIndex == correspondedCalphaHelixIndices[0] and strand.startIndex >= self.chain.getFirstResidueIndex() and strand.stopIndex < nextCalphaHelix.stopIndex:
                        connectedCalphaHelixIndices.add(currentCalphaHelixIndex)
                        connectedCalphaHelixIndices.add(nextCalphaHelixIndex)
                    
                    if nextCalphaHelixIndex == correspondedCalphaHelixIndices[-1] and strand.startIndex > currentCalphaHelix.startIndex and strand.stopIndex <= self.chain.getLastResidueIndex():
                        connectedCalphaHelixIndices.add(currentCalphaHelixIndex)
                        connectedCalphaHelixIndices.add(nextCalphaHelixIndex)

                    if strand.startIndex > currentCalphaHelix.stopIndex and strand.stopIndex < nextCalphaHelix.startIndex:
                        connectedCalphaHelixIndices.add(currentCalphaHelixIndex)
                        connectedCalphaHelixIndices.add(nextCalphaHelixIndex)
            if not connectedCalphaHelixIndices:
                continue

            # Get Source/Target Positions
            sourceCalphaHelixPositions = []
            targetCalphaHelixPositions = []
            for calphaHelixIndex in connectedCalphaHelixIndices:
                calphaHelix = self.chain.helices[calphaHelixIndex]

                for residueIndex in range(calphaHelix.startIndex, calphaHelix.stopIndex + 1):
                    if residueIndex in self.chain.residueRange():
                        sourceCalphaHelixPositions.append(self.chain[residueIndex].getAtom('CA').getPosition())
                        targetCalphaHelixPositions.append(correspondenceTargets[calphaHelixIndex][residueIndex])

            # Calculate Rigid Initialization Transformation
            rigidInitializationTransformation = self.flexibleFittingEngine.findRotationTranslation(sourceCalphaHelixPositions, targetCalphaHelixPositions)

            # Store Rigid Initialization Transformation
            for strand in sheet.strandList.values():
                for residueIndex in range(strand.startIndex, strand.stopIndex + 1):
                    if residueIndex in self.chain.residueRange():
                        rigidInitializationTransformations[residueIndex] = rigidInitializationTransformation

        return rigidInitializationTransformations

    def performFlexibleDeformation(self, temp=None):
        self.calculateCorrespondences()
        # Backup Chain
        self.backupChain()
        
        # Calculate Missing Residue Indices
        (missingResidueIndices, missingResidueIndexMapping) = self.calculateMissingResidueIndices()
        
        # Calculate Missing Residue Bonds
        
        missingResidueBonds = []
        for currentResidueIndex, nextResidueIndex in self.ntuples(self.chain.residueRange(), 2):
            bond = (currentResidueIndex, nextResidueIndex)
            if bond not in self.chain.bonds and currentResidueIndex not in missingResidueIndices and nextResidueIndex not in missingResidueIndices:
                missingResidueBonds.append(bond)
        #print missingResidueBonds
        #if self.checkBoxWSVD.isChecked():
        #    (correspondences, correspondenceTransformations, correspondenceTargets) = self.calculateCorrespondenceTransformationsWithClusterWSVD()
        #else:
        (correspondences, correspondenceTransformations, correspondenceTargets) = self.calculateCorrespondenceTransformations()


        # Set Deformation Parameters
        #self.flexibleFittingEngine.setDeformationNeighborhoodSizes(self.spinBoxLaplacianNeighborhoodSize.value(), self.spinBoxTransformationNeighborhoodSize.value(), 3)
        #self.flexibleFittingEngine.setDeformationNeighborhoodSizes(1, 1, 3)
        if self.laplacianNeighborhoodSize != None:
            if self.transformationNeighborhoodSize != None:
                self.flexibleFittingEngine.setDeformationNeighborhoodSizes(self.laplacianNeighborhoodSize, self.transformationNeighborhoodSize, 3)
            else:
                self.flexibleFittingEngine.setDeformationNeighborhoodSizes(self.laplacianNeighborhoodSize, 10, 3)
        else:
            if self.transformationNeighborhoodSize != None:
                self.flexibleFittingEngine.setDeformationNeighborhoodSizes(10, self.transformationNeighborhoodSize, 3)
            else:
                self.flexibleFittingEngine.setDeformationNeighborhoodSizes(10, 10, 3)
        if self.helixDistortionTerm != None:
            if self.helixFittingTerm != None:
                self.flexibleFittingEngine.setDeformationWeights(self.helixFittingTerm, self.helixDistortionTerm)
            else:
                self.flexibleFittingEngine.setDeformationWeights(1.0, self.helixDistortionTerm)
        else:
            if self.helixFittingTerm != None:
                self.flexibleFittingEngine.setDeformationWeights(self.helixFittingTerm, 1.0)

        
        # Add Deformation Original Vertices
        for residueIndex in self.chain.residueRange():
            if residueIndex not in missingResidueIndices: 
                self.flexibleFittingEngine.addDeformationOriginalVertex(self.chain[residueIndex].getAtom('CA').getPosition())
                # print self.chain[residueIndex].getAtom('CA').getPosition().x()

        # Add Deformation Edges and Calculate Neighbor Vertex Indices Cache
        numResidues      = len(self.chain) - len(missingResidueIndices)
        deformationEdges = set()
        for bond in self.chain.bonds:
            deformationEdges.add((missingResidueIndexMapping[bond[0]], missingResidueIndexMapping[bond[1]]))

        for bond in missingResidueBonds:
            deformationEdges.add((missingResidueIndexMapping[bond[0]], missingResidueIndexMapping[bond[1]]))

        for sheet in self.chain.sheets.values():
            for bond in sheet.bonds:
                deformationEdges.add((missingResidueIndexMapping[bond[0]], missingResidueIndexMapping[bond[1]]))

        for deformationEdge in deformationEdges:
            self.flexibleFittingEngine.addDeformationEdge(deformationEdge[0], deformationEdge[1])

        self.flexibleFittingEngine.calculateNeighborVertexIndicesCaches(numResidues)

        # Set Deformation Vertices
        self.flexibleFittingEngine.setDeformationVerticesToOriginalVertices()

        # Only to test fitting weight and distortion weigh effect
        #self.flexibleFittingEngine.setDeformationWeights(5.0, 1.0)

        start = time.clock()
        """
        # Calculate and Add Rigid Initialization Transformations
        self.flexibleFittingEngine.setDeformationRigidInitialization(self.checkBoxRigidInitialization.isChecked())
        if self.checkBoxRigidInitialization.isChecked():
            rigidInitializationTransformations = self.calculateRigidInitializationTransformations(correspondences, correspondenceTransformations, correspondenceTargets, missingResidueIndexMapping)
            
            for residueIndex in self.chain.residueRange():
                if residueIndex not in missingResidueIndices:
                    self.flexibleFittingEngine.addDeformationRigidInitializationTransformation(rigidInitializationTransformations[residueIndex])
        """
        rigidInitializationTransformations = self.calculateRigidInitializationTransformations(correspondences, correspondenceTransformations, correspondenceTargets, missingResidueIndexMapping)
            
        for residueIndex in self.chain.residueRange():
            if residueIndex not in missingResidueIndices:
                self.flexibleFittingEngine.addDeformationRigidInitializationTransformation(rigidInitializationTransformations[residueIndex])


        
        # Add Deformation Handles
        for calphaHelixIndex in sorted(correspondenceTargets.keys()):
            #print calphaHelixIndex
            for residueIndex in sorted(correspondenceTargets[calphaHelixIndex].keys()):
                self.flexibleFittingEngine.addDeformationHandle(missingResidueIndexMapping[residueIndex], correspondenceTargets[calphaHelixIndex][residueIndex])



        # Perform Deformation
        self.flexibleFittingEngine.deformLaplacian()

        #self.flexibleFittingEngine.deformWSVD()

        elapsed = (time.clock() - start)
        print "helix-guided time: ", elapsed
        ### End Helix Fitting ###

        ### Start Skeleton Fitting ###
        
        # Set Deformation Parameters
        #self.flexibleFittingEngine.setDeformationRigidInitialization(False)
        #self.flexibleFittingEngine.setDeformationWeights(self.doubleSpinBoxSkeletonFittingWeight.value(), self.doubleSpinBoxSkeletonDistortionWeight.value())
        #skeleton fitting 1.0
        #skeleton distortion 10.0
        self.flexibleFittingEngine.setDeformationWeights(1.0, 10.0);
        if self.skeletonDistortionTerm != None:
            if self.skeletonFittingTerm != None:
                self.flexibleFittingEngine.setDeformationWeights(self.skeletonFittingTerm, self.skeletonDistortionTerm)
            else:
                self.flexibleFittingEngine.setDeformationWeights(1.0, self.skeletonDistortionTerm)
        else:
            if self.skeletonFittingTerm != None:
                self.flexibleFittingEngine.setDeformationWeights(self.skeletonFittingTerm, 10.0)

        # Iterative Skeleton Fitting
        #skeletonMesh                       = self.skeletonViewer.renderer.getMesh()
        #originalSkeletonSurfaceVertices    = skeletonMesh.getSurfaceVertices(4)
        originalSkeletonSurfaceVertices    = self.flexibleFittingEngine.getSurfaceVertices(4)
        #originalSkeletonNonSurfaceVertices = skeletonMesh.getNonSurfaceVertices()
        originalSkeletonNonSurfaceVertices = self.flexibleFittingEngine.getNonSurfaceVertices()
        #skeletonFittingIterations          = self.spinBoxSkeletonFittingIterations.value()
        #skeletonFittingDistanceThreshold   = self.spinBoxSkeletonFittingDistanceThreshold.value()
        skeletonFittingIterations          = 5
        if self.skeletonIterations != None:
            skeletonFittingIterations = self.skeletonIterations
        skeletonFittingDistanceThreshold   = 10
        if self.skeletonDistance != None:
            skeletonFittingDistanceThreshold = self.skeletonDistance

        
        realIterationRound = 0;
        start = time.clock()

        # skeletonFittingIterations serves as the most iteration round if "auto stop" box is checked
        for skeletonFittingIteration in range(skeletonFittingIterations):

            realIterationRound +=1

            # Get Deformed Vertices
            deformedCalphaPositions = self.flexibleFittingEngine.getDeformedVertices()

            # Set Deformation Vertices
            self.flexibleFittingEngine.setDeformationVerticesToDeformedVertices()
            
            # Reset Deformation Handles
            self.flexibleFittingEngine.resetDeformationHandles()
 
            # Copy Original Skeleton Vertices
            skeletonSurfaceVertices    = list(originalSkeletonSurfaceVertices)
            skeletonNonSurfaceVertices = list(originalSkeletonNonSurfaceVertices)
            
            # Add Skeleton Deformation Handles
            for residueIndex in self.chain.residueRange():
                if residueIndex not in missingResidueIndices:
                    isHelix = False
                    isSheet = False
                    for helixIndex, helix in self.chain.helices.items():
                        if helixIndex in correspondenceTargets and helix.startIndex <= residueIndex and residueIndex <= helix.stopIndex:
                            isHelix                  = True
                            correspondenceHelixIndex = helixIndex
                    for sheet in self.chain.sheets.values():
                        for strand in sheet.strandList.values():
                            if strand.startIndex <= residueIndex and residueIndex <= strand.stopIndex:
                                isSheet = True

                    calphaAtomPosition = deformedCalphaPositions[missingResidueIndexMapping[residueIndex]]
                   
                    if isHelix:
                        closestSkeletonVertex = correspondenceTargets[correspondenceHelixIndex][residueIndex]
                    elif isSheet:
                        closestSkeletonVertex = self.getClosestVertex(calphaAtomPosition, skeletonSurfaceVertices, skeletonFittingDistanceThreshold, False)
                        if closestSkeletonVertex == None:
                            closestSkeletonVertex = self.getClosestVertex(calphaAtomPosition, skeletonNonSurfaceVertices, skeletonFittingDistanceThreshold, False)
                    else:
                        closestSkeletonVertex = self.getClosestVertex(calphaAtomPosition, skeletonNonSurfaceVertices, skeletonFittingDistanceThreshold, False)

                    if closestSkeletonVertex != None:
                        self.flexibleFittingEngine.addDeformationHandle(missingResidueIndexMapping[residueIndex], closestSkeletonVertex)
                        
                        #if skeletonFittingIteration == (skeletonFittingIterations - 1):
                        #    self.calphaViewer.renderer.addSkeletonFittingVector(self.chain[residueIndex].getAtom('CA').getPosition(), closestSkeletonVertex)

            # Perform Deformation
            self.flexibleFittingEngine.deformLaplacian()

            # If RMSD difference between deformed and pre-deformed model is below the threshold, stop
            #if self.checkBoxAutoStop.isChecked() and self.flexibleFittingEngine.getShapeDistance() < self.doubleSpinBoxStopThreshold.value():
            #    break
            

        elapsed = (time.clock() - start)
        print "skeleton-guided time: ", elapsed
        print "skeleton iteration round: ", realIterationRound

        start = time.clock()
        
        
        # Calculate Deformation Transformations
        self.flexibleFittingEngine.calculateDeformationTransformations(numResidues)

        # Transform Backbone and Side Chains
        for residueIndex in self.chain.residueRange():
            deformationTransformation = self.flexibleFittingEngine.getDeformationTransformation(missingResidueIndexMapping[residueIndex])
            for atomName in self.chain[residueIndex].getAtomNames():
                self.chain[residueIndex].getAtom(atomName).transform(deformationTransformation)

