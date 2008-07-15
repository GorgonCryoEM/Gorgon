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
    self.matchList=[]
    self.score=0.0
    self.libary=library
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
    if chain:
      s=s+chain
    return s
