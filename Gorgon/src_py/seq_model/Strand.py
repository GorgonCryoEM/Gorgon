#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Strand
# Class Description: Class that models beta strands
#                    More info in: seq_model-doc.txt
#

try:
    from PyQt4 import QtCore, QtGui
    qtEnabled=True
except:
    qtEnabled=False

from seq_model.Secel import Secel

class Strand(Secel):
    def __init__(self, chain, strandNo, label, startIndex, stopIndex, color=None):
        if qtEnabled and color==None:
          color=QtGui.QColor(0,180,50)
        Secel.__init__(self, chain, strandNo, label, startIndex, stopIndex, color)
        self.type="strand"
        self.strandNo=strandNo
    
        #chain.addStrand(label,self)

    def toPDB(self,sheetID,sheet):
        """
This returns a 'SHEET' line for this strand for a PDB file.
        """
        startResName=self.chain.residueList[self.startIndex].symbol3
        startChainID=self.chain.chainID
        stopResName=self.chain.residueList[self.stopIndex].symbol3
        nStrands=len(sheet.strandList)
    
    
        s= "SHEET "
        s=s+ str(self.strandNo).rjust(4) +' '
        s=s+ sheetID.rjust(3) +' '
        s=s+ str(nStrands).rjust(1) +' '  #this might break for (nStrands > 9)
    
        s=s+ startResName.rjust(3) +' '
        s=s+ startChainID.rjust(1) +' '
        s=s+ str(self.startIndex).rjust(3) +' '  #this might break for (startIndex > 999)
    
        s=s+ stopResName.rjust(4) +' '
        s=s+ startChainID.rjust(1) +' '
        s=s+ str(self.stopIndex).rjust(3) +' '   #this might break for (stopIndex > 999)
    
        # NYI:  Sense (direction) of strand
        # NYI:  Registration atom             (current strand)
        # NYI:  Registration residue name     (current strand)
        # NYI:  Registration chainID          (current strand)
        # NYI:  Registration residue index    (current strand)
        # NYI:  Registration insertion code   (current strand)
        # NYI:  Registration atom             (previous strand)
        # NYI:  Registration residue name     (previous strand)
        # NYI:  Registration chainID          (previous strand)
        # NYI:  Registration residue index    (previous strand)
        # NYI:  Registration insertion code   (previous strand)
    
        s=s.ljust(80)+ '\n'
    
        return s
