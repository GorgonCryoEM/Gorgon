#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Secel
# Class Description: Superclass that models secondary structure elements.  Subclassed by Helix, Strand, Coil
#                    More info in: seq_model-doc.txt
#

class Secel(object):
  def __init__(self, chain, serialNo, label, startIndex, stopIndex):
    self.chain=chain
    self.serialNo=serialNo
    self.label=label
    self.startIndex=startIndex
    self.stopIndex=stopIndex



  def __str__(self):
    s = self.type
    s = s + '('
    s = s + str(self.startIndex)
    s = s + ','
    s = s + str(self.stopIndex)
    s = s + ')'
    s = s + ':'
    residues = self.chain.residueList[self.startIndex-1:self.stopIndex-1]

    for residue in residues:
      s = s + residue.symbol1
    return s
