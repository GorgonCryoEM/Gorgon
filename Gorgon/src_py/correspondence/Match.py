#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Match
# Class Description: Class that represents the mapping of a single predicted secel to an observed secel.
#		     Matches are aggregated to form a Correspondence object.
#                    More info in: correspondence-doc.txt
#

class Match:
  # A forward match aligns the N-terminus with lesser vertex, and C-terminus with greater vertex
  #    lesser vertex is vertex closer to 0,0,0
  FORWARD = 0
  REVERSE = 1  
  def __init__(self, observed, predicted, direction):
    self.observed=observed
    self.predicted=predicted
    self.direction=direction
    self.constrained = False
    self.directionConstrained = False

  def __repr__(self):
    if self.observed is None:
      return 0
    elif self.direction is self.FORWARD:
      return self.observed.label.lower()
    else:
      return self.observed.label.capitalize()
