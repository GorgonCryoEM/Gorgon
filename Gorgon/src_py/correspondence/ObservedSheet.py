#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  ObservedSheet
# Class Description: Class that denotes a sheet found by SSEHunter/Builder or any other routine that
#                    can find helices in EM maps.
#                    More info in: correspondence-doc.txt
#

class ObservedSheet:
  #def __init__(self, params):
  def __init__(self, label, cornerList):
    #raise NotImplementedError, 'Not Yet Implemented: ObservedSheet contructor'
    """Note that the label is currently an integer, not a string."""
    self.cornerList = cornerList
    self.label = label #Note: currently an integer, not a string
    self.sseType = 'sheet'






  def __parseSheet(self, inputString):
    pass
