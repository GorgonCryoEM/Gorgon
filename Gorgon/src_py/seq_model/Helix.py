#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Helix
# Class Description: Class that models alpha helices
#                    More info in: seq_model-doc.txt
#

try:
  from PyQt4 import QtCore, QtGui
  qtEnabled=True
except:
  qtEnabled=False

from seq_model.Secel import Secel

class Helix(Secel):
    serialNo=0
    def __init__(self, chain, serialNo, label, startIndex, stopIndex, color=None):
        if qtEnabled and color==None:
          color=QtGui.QColor(51,208,208)
        Secel.__init__(self, chain, serialNo, label, startIndex, stopIndex, color)
        self.type="helix"
        self.axisPoint1 = None
        self.axisPoint2 = None

    @classmethod
    def parsePDB(cls,line,chain):
        serialNo     =         int(line[7:10].strip())
        helixID     =     line[11:14].strip()
        #chainID     =         line[19:20]
        start     =         int(line[21:25].strip())
        stop     =         int(line[33:37].strip())
    
        if helixID in chain.helices.keys():
            raise ValueError, 'Duplicate Helix entries in PDB'
        else:
            #chain.helices[serialNo]=Helix(chain,serialNo,helixID,start,stop)
            helix=Helix(chain,serialNo,'H' + helixID,start,stop)
            chain.addHelix(serialNo, helix)
    
    def flipHelix(self):
        '''
        This moves the C-alpha atoms so the one at the first residue of the helix is at the last residue of the helix, 
        the atom at the second residue is now at the second from the last residue, etc.  
        The "modelChanged()" signal must be emitted elsewhere.
        '''
        self.axisPoint1, self.axisPoint2 = self.axisPoint2, self.axisPoint1
        atomList = []
        for resNum in range(self.startIndex, 1+self.stopIndex):
            CAatom = self.chain[resNum].getAtom('CA')
            if CAatom:
                atomList.append(CAatom)
            else:
                atomList.append(None)
            self.chain[resNum].clearAtoms()
        i = 0
        assert len(atomList) == 1+self.stopIndex-self.startIndex
        for CAatom in atomList[::-1]:
            if CAatom:
                self.chain[startIndex+i].addAtomObject(CAatom)
            i += 1
        
    def getAngstromLength(self):
        return 1.5*(1+self.stopIndex-self.startIndex)
    
    def getAxisPoints(self):
        return (self.axisPoint1, self.axisPoint2)
    
    def setAxisPoints(self, point1, point2):
        self.axisPoint1 = point1
        self.axisPoint2 = point2
    
    def toPDB(self):
        Helix.serialNo=Helix.serialNo+1
        init_res_name=self.chain.residueList[self.startIndex].symbol3
        init_chainID=self.chain.chainID
        init_seq_num=self.startIndex
        end_res_name=self.chain.residueList[self.stopIndex].symbol3
        end_seq_num=self.stopIndex
    
        s= "HELIX  "
        s=s+ str(Helix.serialNo).rjust(3) +' '
        s=s+ self.label.lstrip('H').rjust(3) +' '
        s=s+ init_res_name.rjust(3) +' '
        s=s+ init_chainID.rjust(1) +' '
        s=s+ str(init_seq_num).rjust(4) +' '
    
        s=s+ end_res_name.rjust(4) +' '
        s=s+ init_chainID.rjust(1) +' '
        s=s+ str(end_seq_num).rjust(4) +' '
        s=s+ str(1).rjust(1) +' '
    
        s=s+ str(self.stopIndex-self.startIndex + 1 ).rjust(36) + "    \n"
    
        return s
