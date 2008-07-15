#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Strand
# Class Description: extends seq_model.Atom to account for Gorgon Viewer interface 
#                    More info in: seq_model-doc.txt
#

from Secel import Secel

class Strand(Secel):
  def __init__(self, chain, label, start_index, stop_index):
    Secel.__init__(self, chain, label, start_index, stop_index)
    self.type="strand"

  def to_pdb(self):
    print "SHEET"
    return ''
