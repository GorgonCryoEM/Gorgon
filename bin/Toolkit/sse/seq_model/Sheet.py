#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Sheet
# Class Description: Class that models beta sheet, which act as containers for beta strand objects
#                    More info in: seq_model-doc.txt
#

from seq_model.Strand import Strand

class Sheet:
    def __init__(self, initialStrand=None):
        self.strandList={}
        if initialStrand is not None:
            self.strandList[initialStrand.strandNo]=initialStrand


    @classmethod
    def parsePDB(cls,line,chain):
        """
Given a line from a PDB and a chain, this creates a Sheet object and 
adds it to the chain.
        """
        strandNo     =         int(line[7:10].strip())
        sheetID     =         line[11:14].strip()
        #chainID     =         line[21:22]
        start     =         int(line[22:26].strip())
        stop     =         int(line[33:37].strip())
    
        label = 'S' + sheetID
        strand = Strand (chain,strandNo,label,start,stop)
    
        if sheetID not in chain.sheets.keys():
            #chain.sheets[sheetID]=Sheet(strand)
            sheet=Sheet(strand)
            chain.addSheet(sheetID, sheet)
            # add the first strand for this new sheet:
            chain.addStrand(strand, strandNo, sheetID)

        else:
            #chain.sheets[sheetID].strandList[strandNo] = strand
            chain.addStrand(strand, strandNo, sheetID)


    def toPDB(self,sheetID):
        """
This generates a 'SHEET' line from this sheet for a PDB file.
        """
        s=''
        for strandNo in sorted(self.strandList.keys()):
            strand=self.strandList[strandNo]
            s=s+ strand.toPDB(sheetID,self)
        return s
