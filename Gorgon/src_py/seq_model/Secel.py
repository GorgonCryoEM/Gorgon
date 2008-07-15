#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Secel
# Class Description: extends seq_model.Atom to account for Gorgon Viewer interface 
#                    More info in: seq_model-doc.txt
#

class Secel(object):
  def __init__(self, chain, label, start_index, stop_index):
    self.chain=chain
    self.label=label
    self.start_index=start_index
    self.stop_index=stop_index



  def __str__(self):
    s = self.type
    s = s + '('
    s = s + str(self.start_index)
    s = s + ','
    s = s + str(self.stop_index)
    s = s + ')'
    s = s + ':'
    residues = self.chain.residue_list[self.start_index-1:self.stop_index-1]

    for residue in residues:
      s = s + residue.symbol1
    return s
