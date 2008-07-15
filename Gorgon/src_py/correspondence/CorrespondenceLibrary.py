#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class: CorrespondenceLibrary
# Class Description: Class that stores a set of correspondences and the parameters used to compute them.
#                    More info in: correspondence-doc.txt
#

class CorrespondenceLibrary:
  def __init__(self, sp=None, so=None, algo=None, params=None, correspondenceList=None, comments=None, filename=None):
    if (filename):
      self.load(filename)
    else:
      self.structurePrediction=sp
      self.structureObservation=sp
      self.algorithm=algo
      self.params=params
      self.correspondenceList=corresondenceList
      self.comments=comments
      # sort correspondences by score

  def save(self, filename):
    # write observed & SSE parameters
    # write StructurePrediction & params
    # write algorithm, params, comments
    # write correspondence list by calling Correspondence.__repr__
    pass
  
  def load(self, filename):
    pass
