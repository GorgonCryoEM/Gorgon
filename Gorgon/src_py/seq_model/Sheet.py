#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Sheet
# Class Description: extends seq_model.Atom to account for Gorgon Viewer interface 
#                    More info in: seq_model-doc.txt
#

class Sheet:
  def __init__(self, strand_list):
    self.strand_list=strand_list



  def add(strand):
    self.strand_list.append(strand)



  def remove(strand):
    self.strand_list.remove(strand)
