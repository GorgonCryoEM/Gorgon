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
        self.chain=chain
        self.params=params
        self.comments=comments
        self.app = qparent


    @classmethod
    def load(cls, filename,  qparent=None):
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
            and the second half are treated as structure predictions.  
        '''
        secelIndex=0
        secelDict={}
        params=None
        comments=None
        
        F = open(filename)
        lines = []
        for line in F:
            line = line.strip()
            lines.append(line)
        if lines[0][:5].upper() == 'START':
            firstLine = lines.pop(0)
            startIndex = int(firstLine.split()[-1])
            #print startIndex
        else:
            startIndex = 1
        stopIndex = None  
        
        lines = ''.join(lines)
        linesSize = len(lines)
        try:
            assert (linesSize % 2 == 0)
        except:
            "The file does not have an equal number of reisdues and secondary structure indicators."
        
        sequence = lines[:(linesSize//2)]
        #print sequence
        predictionsStr = lines[(linesSize//2):]
        #print predictionsStr
        
        if startIndex == 1:
            chain = Chain(sequence, qparent)
        else:
            chain = Chain('', qparent)
            n = 0
            for char in sequence:
                chain[startIndex+n] = Residue(char, chain)
                n += 1 
        
        # loop pre-conditions
        # i is the next char in the predictionStr
        current= predictionsStr[0]
        start=startIndex
        helixCount=0
        minHelixLength = 6
        strandCount=0
        coilCount=0
        for nextChar,index in zip(predictionsStr,range(startIndex,len(predictionsStr)+startIndex)):
    
            prev=current
            current=nextChar
            
            if prev != current:
                stop=index-1
                length = stop - start + 1
        
                if prev=='H':
                    if length >= minHelixLength:
                        helixCount=helixCount+1
                        label='H' + str(helixCount)
                        secel = Helix(chain,None, label,start,stop)
                        #chain.secel_list.append(secel)
        
                elif prev=='E':
                    strandCount=strandCount+1
                    label='E' + str(strandCount)
                    secel = Strand(chain,None, label,start,stop)
                    #chain.secel_list.append(secel)
        
                elif prev=='-':
                    coilCount=coilCount+1
                    label='C' + str(coilCount)
                    secel = Coil(chain,None, label,start,stop)
                    #chain.secel_list.append(secel)
        
                else:
                    errString="got %s when expecting 'H','E', or '-'" %prev
                    raise ValueError(errString)
                start = index
                print secel
                secelDict[secelIndex]=secel
                secelIndex = secelIndex+1        
        
        print chain
        #print chain.toPDB()
        print secelDict
        return StructurePrediction(secelDict,chain, params, comments, qparent)
        
    def getSecelByIndex(self, index):
        for key in self.secelDict.keys():
            secel = self.secelDict[key]
            if secel.startIndex <= index and index <= secel.stopIndex:
                return secel
                
        return Coil(self.chain, 0, 'no-label', index, index)
