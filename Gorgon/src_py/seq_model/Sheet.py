#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Sheet
# Class Description: Class that models beta sheet, which act as containers for beta strand objects
#                    More info in: seq_model-doc.txt
#

from Strand import Strand

class Sheet:
  def __init__(self, initialStrand=None):
    self.strandList={}
    if initialStrand is not None:
      self.strandList[initialStrand.strandNo]=initialStrand


  @classmethod
  def parsePDB(cls,line,chain):
    strandNo     =         int(line[7:10].strip())
    sheetID     =         line[11:14].strip()
    #chainID     =         line[21:22]
    start     =         int(line[22:26].strip())
    stop     =         int(line[33:37].strip())

    strand = Strand (chain,strandNo,sheetID,start,stop)

    if sheetID not in chain.sheets.keys():
      chain.sheets[sheetID]=Sheet(strand)
    else:
      chain.sheets[sheetID].strandList[strandNo] = strand


  def toPDB(self,sheetID):
    s=''
    for strandNo in sorted(self.strandList.keys()):
      strand=self.strandList[strandNo]
      s=s+ strand.toPDB(sheetID,self)
    return s
