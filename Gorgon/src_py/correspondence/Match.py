#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Match
# Class Description: Class that represents the mapping of a single predicted secel to an observed secel.
#		     Matches are aggregated to form a Correspondence object.
#                    More info in: correspondence-doc.txt
#

class Match:
  FOREWARD=0
  REVERSE=0
  def __init__(self, observed, predicted, direction):
    # A forward match aligns the N-terminus with lesser vertex, and C-terminus with greater vertex
    #    lesser vertex is vertex closer to 0,0,0
    self.observed=observed
    self.predicted=predicted
    self.direction=direction
