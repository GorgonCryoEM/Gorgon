#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Secel
# Class Description: 
#                    More info in: seq_model-doc.txt
#

from Secel import Secel
class Coil(Secel):
  def __init__(self, chain, label, start_index, stop_index):
    Secel.__init__(self, chain, label, start_index, stop_index)
    self.type="loop"

  def to_pdb(self):
    return ''
