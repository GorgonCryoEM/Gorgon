#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  StructurePrediction
# Class Description: Class that represents all of the secels predicted to exist based on primary sequence.
#                    More info in: correspondence-doc.txt
#

from seq_model.Helix import Helix
from seq_model.Coil import Coil
from seq_model.Strand import Strand
from seq_model.Chain import Chain
from seq_model.Residue import Residue
from libpyGORGON import SeqReader, SeqFileData

try:
    from PyQt4 import QtCore, QtGui
    qtEnabled=True
    baseClass=QtCore.QObject
except:
    qtEnabled=False
    baseClass=object

class StructurePrediction(baseClass):  #results of secondary-structure prediction
    """
This class contains the sequence of a chain and the residue numbers 
where SSEs are predicted to be based on the sequence.
    """
    def __init__(self, secelDict, chain, params=None, comments=None, qparent=None, secelType=None):
        if qparent and qtEnabled:
            super(QtCore.QObject,self).__init__(qparent)
        self.secelDict=secelDict
        self.helixDict = {}
        self.strandDict = {}
        self.__updateDicts()
        self.selectedSecel = None
        self.chain=chain
        self.params=params
        self.comments=comments
        self.app = qparent
        # list of types of secels
        self.secelType = secelType

    def __updateDicts(self):
        self.strandDict = {}
        self.helixDict = {}
        iH = 0
        iS = 0
        for i in range(len(self.secelDict)):
            if self.secelDict[i].type == 'helix':
                self.helixDict[iH] = self.secelDict[i]
                iH += 1
            if self.secelDict[i].type == 'strand':
                self.strandDict[iS] = self.secelDict[i]
                iS += 1
        return
    


    @classmethod
    def load(cls, filename, qparent=None, withStrands=0):
        '''
Sequence files are a file type we defined. The first line gives the 
one-letter abbreviations for the sequence. The line below it shows the 
predicted secondary structure element for each residue as "H" for 
helix, "E" for strand, and "-" otherwise.  
Ex 1:
GAPCSTLARFKEI
HHHHHH--EEEE
Ex 2:
START 45
SAPQRVPELYC
EEEHHHHHH-

The first line may give a start residue (useful for post-translational 
modifications). That line will be interpreted and removed. Linebreaks 
are then removed. Finally, the first half of the remaining characters 
are interpreted as sequence, and the second half are treated as 
structure predictions. The actual file reading and interpreting is 
handled in C++.
        '''
        print "StructurePrediction.load called. cls = " + str(cls) + ", filename = " + str(filename) + "qparent = " + str(qparent)

        #secelIndex=0
        secelDict={}
        secelType={}
        params=None
        comments=None
        
        if filename.split('.')[-1].lower() == 'seq':
            # data is a c++ SEQReader object
            data = SeqReader.loadFile(filename)
            startIndex = data.getStartResNo()
            sequence = data.getSequenceString()
            if startIndex == 1:
                chain = Chain(sequence, qparent)
            else:
                chain = Chain('', qparent)
                n = 0
                for char in sequence:
                    chain[startIndex+n] = Residue(char, chain)
                    n += 1                

            numSSEs = data.getNumberOfStructures()
            for sseIx in range(numSSEs):
                # cppSse is a c++ SecondaryStructure object
                cppSse = data.getStructure(sseIx)

                if cppSse.isHelix(): 
                    # create python Helix object using info from c++ SecondaryStructure object
                    pyHelix = Helix(chain, sseIx, str(cppSse.getSecondaryStructureID()), cppSse.getStartPosition(), cppSse.getEndPosition())
                    secelDict[sseIx] = pyHelix                                
                    secelType[sseIx] = 'helix'     
                elif cppSse.isSheet():
                    # create python strand object using info from c++ SecondaryStructure object
                    pyStrand = Strand(chain, sseIx, str(cppSse.getSecondaryStructureID()), cppSse.getStartPosition(), cppSse.getEndPosition())
                    secelDict[sseIx] = pyStrand
                    secelType[sseIx] = 'strand'                                
                    pass
                
            # create new python StructurePrediction object and return it
            return StructurePrediction(secelDict, chain, params, comments, qparent, secelType)
        elif filename.split('.')[-1].lower() == 'pdb':
            # create python chain object using the python pdb file loader method
            chain = Chain.load(filename, qparent)
            i = 0
            #for helixKey in chain.helices.keys():
            #    print "helixKey " + str(helixKey)
            #    secelDict[i] = chain.helices[helixKey]
            #    i += 1
            #print "done adding helices. i=" + str(i)
            #
            #for sheetKey in chain.sheets.keys():
            #    print "sheetKey " + str(sheetKey)
            #    for strandKey in chain.sheets[sheetKey].strandList.keys():
            #        print "strandKey " + str(strandKey)
            #        secelDict[i] = chain.sheets[sheetKey].strandList[strandKey]
            #        # self.sheets[sheetID].strandList[strandNo]=strand
            #        #secelDict[i] = chain.sheets[sheetKey]
            #        i += 1
            #print "done adding sheets. i=" + str(i)
            
            # create secelDict from chain
            #for index in chain.residueRange()[::-1]:
            #    if chain.residueList[index] is inputRes:
            #        return index

            lastSecel = -1;

            #iterate over all secels. sort is needed because chain.secelList is a dict
            for index in sorted(chain.secelList):
                if chain.secelList[index] == lastSecel:
                    pass
                    #print "same as last secel at " + str(index) + " (" + str(chain.secelList[index]) + ")" 
                else:
                    #print "found new secel " + str(index) #+ " (" + str(chain.secelList[index]) + ")"
                    if chain.secelList[index].type == 'helix':
                        #print "helix at index " + str(index)
                        secelDict[i] = chain.secelList[index]
                        secelType[i] = 'helix'
                        lastSecel = chain.secelList[index]
                        i += 1
                    if chain.secelList[index].type == 'strand' and withStrands == 1:
                        #print "strand at index " + str(index)
                        secelDict[i] = chain.secelList[index]
                        secelType[i] = 'strand'
                        lastSecel = chain.secelList[index]
                        i += 1
                
                 
            #chain.helices = {}
            #chain.sheets = {}
            #chain.secelList = {}
            #chain.orphanStrands = {}
            #chain.atoms = {}
            for resIndex in chain.residueRange():
                chain[resIndex].clearAtoms()
            # create new python StructurePrediction object and return it
            return StructurePrediction(secelDict, chain, params, comments, qparent, secelType)
        
        
    def getSecelByIndex(self, index):
        """
Given a residue number (often referred to as residue index), this 
returns the Helix or Strand that contains it or a single-residue Coil
if it is not a part of a Helix or Strand.
        """
        lastSecelEnd = self.chain.getFirstResidueIndex()-1
        nextSecelStart = self.chain.getLastResidueIndex()+1
        
        for key in self.secelDict.keys():
            secel = self.secelDict[key]
            if secel.startIndex <= index and index <= secel.stopIndex:
                return secel
            if secel.stopIndex < index and secel.stopIndex > lastSecelEnd:
                lastSecelEnd = secel.stopIndex
            if secel.startIndex > index and secel.startIndex < nextSecelStart:
                nextSecelStart = secel.startIndex
                                
        return Coil(self.chain, 0, 'no-label', lastSecelEnd + 1, nextSecelStart - 1)
        
    def setSecelSelection(self, secel):
        if not secel in self.secelDict.values():
            return
        self.__selectedSecel = secel
    def getSecelSelection(self):
        return self.__selectedSecel
