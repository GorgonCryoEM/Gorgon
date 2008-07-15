#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Helix
# Class Description: extends seq_model.Atom to account for Gorgon Viewer interface 
#                    More info in: seq_model-doc.txt
#

from Secel import Secel

class Helix(Secel):
  serial_num=0
  def __init__(self, chain, label, start_index, stop_index):
    Secel.__init__(self, chain, label, start_index, stop_index)
    self.type="helix"


  def to_pdb(self):
    Helix.serial_num=Helix.serial_num+1
    init_res_name=self.chain.residue_list[self.start_index].symbol3
    init_chain_id=self.chain.chain_id
    init_seq_num=self.start_index
    end_res_name=self.chain.residue_list[self.stop_index].symbol3
    end_seq_num=self.stop_index

    s= "HELIX"
    s=s+ str(Helix.serial_num).rjust(4) +' '
    s=s+ self.label.rjust(3) +' '
    s=s+ init_res_name.rjust(3) +' '
    s=s+ init_chain_id.rjust(1) +' '
    s=s+ str(init_seq_num).rjust(4) +' '

    s=s+ end_res_name.rjust(4) +' '
    s=s+ init_chain_id.rjust(1) +' '
    s=s+ str(end_seq_num).rjust(4) +' '
    s=s+ str(1).rjust(1) +' '

    s=s+ str(self.stop_index-self.start_index).rjust(36) + "\n"

    return s
