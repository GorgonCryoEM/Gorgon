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
    def __init__(self, secelDict, chain, params=None, comments=None, qparent=None):
        if qparent and qtEnabled:
            super(QtCore.QObject,self).__init__(qparent)
        self.secelDict=secelDict
        self.selectedSecel = None
        self.chain=chain
        self.params=params
        self.comments=comments
        self.app = qparent


    @classmethod
    def load(cls, filename, qparent=None):
        '''
            Sequence files are a file type we defined.  The first line gives the one-letter abbreviations for the sequence.  
            The line below it shows the predicted secondary structure element for each residue as "H" for helix, "E" for strand, and "-" otherwise.  
            Ex 1:
            GAPCSTLARFKEI
            HHHHHH--EEEE
            Ex 2:
            START 45
            SAPQRVPELYC
            EEEHHHHHH-
            
            The first line may give a start residue (useful for post-translational modifications).  That line will be interpreted and removed.  
            Linebreaks are then removed.  Finally, the first half of the remaining characters are interpreted as sequence, 
            and the second half are treated as structure predictions.  The actual file reading and interpreting is handled in C++.
        '''
        #secelIndex=0
        secelDict={}
        params=None
        comments=None
        
        if filename.split('.')[-1].lower() == 'seq':
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
                cppSse = data.getStructure(sseIx)
                if cppSse.isHelix(): 
                    pyHelix = Helix(chain, sseIx, str(cppSse.getSecondaryStructureID()), cppSse.getStartPosition(), cppSse.getEndPosition())
                    secelDict[sseIx] = pyHelix                                
                elif cppSse.isSheet():
                    #TODO: Add Sheet support
                    secelDict[sseIx] = None
                    pass
            return StructurePrediction(secelDict, chain, params, comments, qparent)
        elif filename.split('.')[-1].lower() == 'pdb':
            chain = Chain.load(filename, qparent)
            i = 0
            for helixKey in chain.helices.keys():
                secelDict[i] = chain.helices[helixKey]
                i += 1
            chain.helices = {}
            chain.sheets = {}
            chain.secelList = {}
            chain.orphanStrands = {}
            chain.atoms = {}
            for resIndex in chain.residueRange():
                chain[resIndex].clearAtoms()
            return StructurePrediction(secelDict, chain, params, comments, qparent)
        
        
    def getSecelByIndex(self, index):
        for key in self.secelDict.keys():
            secel = self.secelDict[key]
            if secel.startIndex <= index and index <= secel.stopIndex:
                return secel
                
        return Coil(self.chain, 0, 'no-label', index, index)
        
    def setSecelSelection(self, secel):
        if not secel in self.secelDict.values():
            return
        self.__selectedSecel = secel
    def getSecelSelection(self):
        return self.__selectedSecel
