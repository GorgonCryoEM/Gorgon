#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class: Correspondence
# Class Description: Class that models the correspondence between 
#                    observed and predicted secondary structure elements.
#                    More info in: correspondence-doc.txt
#

class Correspondence:
  def __init__(self, library, matchList, score, comments=None, chain=None):
    self.matchList=matchList
    self.score=score
    self.library=library
    self.comments=None
    self.chain=None

  def __repr__(self):
    s=''
    # print index, score, matchList
    #    matchList is ordered by the order of secels as they occur in StructurePrediction
    #    b,A,0,C
    #    StructurePrediction 0:   StructureObservation b (forward)
    #    StructurePrediction 1:   StructureObservation a (reverse)
    #    StructurePrediction 2:   no match
    #    StructurePrediction 3:   StructureObservation c (reverse)
    #    
    for match in self.matchList:
      s=s+match
    s=s+'\n'
    if self.chain:
      s=s+self.chain
    return s
