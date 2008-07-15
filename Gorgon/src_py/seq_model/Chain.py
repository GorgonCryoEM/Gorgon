#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Chain
# Class Description: Class that models polypeptide chains, whih are sequences of Residue objects.
#                    More info in: seq_model-doc.txt
#

from Residue import Residue
from Helix import Helix
from Strand import Strand
from Coil import Coil
class Chain(object):
  """Chain objects represent single polypeptide chains, which are sequences of Residue objects"""
  chains_dict = {}

  #Chain ID management
  __last_chain_id = 64  # chr(64) is A;  chr(65) is B, etc.
  def __next_chain_id(self):
    Chain.__last_chain_id=Chain.__last_chain_id+1
    return chr(Chain.__last_chain_id)




  #Chain Constructor
  def __init__(self,char_string):
    self.secel_list=[]
    self.residue_list={}
    self.chain_id = self.__next_chain_id()

    i=1
    for char in char_string:
      self.residue_list[i]=Residue(char)
      i=i+1

    Chain.chains_dict[self.chain_id]=self



  def __repr__(self):
    s=''
    residue_range=self.residue_range()
    for index in residue_range:
			if (index!=1)  and not (index-1) in residue_range:
				s=s+'...'
			s=s + self[index].symbol1
    return s




  def __iter__(self):
    # suppose residue_list keys are {3,4,5,6,7,8, 13,14,15,16, 21,22,23}
    #eys=self.residue_list.keys().sort()
    keys=self.residue_list.keys()
    key_index=0
    next_residue_key=keys[key_index]


    while True:
      yield self.residue_list[next_residue_key]
      key_index=key_index+1
      try:
        next_residue_key=keys[key_index]
      except IndexError:
        raise StopIteration 



  def __setitem__(self,i, residue):
    self.residue_list[i]=residue



  # my_chain[7] returns the seventh residue (assuming that indexing starts at 1
  def __getitem__(self,i):
  #  when invoked with two indices (i,j), a new chain is returned that is inclusive of both i and j
  #  NYI:  Slicing results should return a chain WITH secel properties

    # branch for slices
    if hasattr(i,'start'):
      start=i.start
      stop=i.stop
      if i.start is None: 
        start=1
      elif i.start < 0:
        start=len(self)+i.start+1

      if i.stop is None: 
        stop=len(self)
      elif i.stop < 0:
        stop=len(self)+i.stop+1
      return self.__slicehelper(start,stop)

    # branch for non-slices
    else:
      #rint 'getitem(%i)' %i
      return self.residue_list[i]


  def __slicehelper(self,i,j):
    #rint 'slice_helper(%s,%s)' %(i,j)
    keys=self.residue_list.keys()
    keys.sort()
    start=keys.index(i)
    stop=keys.index(j)
    del keys[stop+1:]
    del keys[:start]

    new_chain=Chain('')
    for index in keys:
      new_chain[index]=self[index]

    for secel in self.secel_list:
      need_to_duplicate_secel=False
      
      # Secel start_index
      if new_chain.residue_list.has_key(secel.start_index):
	start_index=secel.start_index
        need_to_duplicate_secel=True
      else: 
	keys=new_chain.residue_list.keys()
	keys.sort()
	start_index=keys[0]

      # Secel stop_index
      if new_chain.residue_list.has_key(secel.stop_index):
	stop_index=secel.stop_index
        need_to_duplicate_secel=True
      else: 
	keys=new_chain.residue_list.keys()
	keys.sort()
	stop_index=keys[-1]

      if need_to_duplicate_secel:
        if isinstance(secel,Helix):  cls=Helix
        if isinstance(secel,Strand): cls=Strand
        if isinstance(secel,Coil):   cls=Coil
        new_chain.secel_list.append(cls(new_chain, new_chain.chain_id, start_index, stop_index))
        #rint 'secel.__class__=%s' %secel.__class__

    return new_chain



  # len(my_chain) returns the length of residue_list
  def __len__(self):
    return_value= len(self.residue_list)
    return return_value



  def clear_coordinates(self):
    for index in self.residue_range():
      self[index].atoms={}



  def append(self,residue):
    if len(self.residue_list)==0:
      self.residue_list[1]=residue
    else:
      keys=self.residue_list.keys()
      keys.sort()
      key=1+keys[-1]
      self.residue_list[key]=residue
      #elf.residue_list.append(residue)



  def residue_range(self):
    return sorted(self.residue_list.keys())



  #Return a pdb-compliant string
  def to_pdb(self, backbone_only=False): 
    """decomposes chain into constituent residues and atoms to render pdb coordinate model.  residue is ommitted if it has no atoms."""
    #Not Thread-Safe
    atom_index=1
    Helix.serial_num=0  #This is what makes it not thread-safe

    s=''

    for secel in self.secel_list:
      s=s+secel.to_pdb()

    for residue_index in self.residue_range():
     #rint residue_index
     residue=self[residue_index]

     if backbone_only:
      atoms = ['CA']
     else:
      atoms=residue.atoms.keys()

     try:
       for atom_name in atoms:
	  atom=residue.atoms[atom_name]
	  s=s+ "ATOM" + ' '
	  s=s+ str(atom_index).rjust(6) + ' '
	  atom_index=atom_index+1 
	  s=s+ atom_name.rjust(3) + ' '
	  s=s+ residue.symbol3.rjust(4) + ' '
	  s=s+ self.chain_id.rjust(1) + ' ' #chainID
	  s=s+ str(residue_index).rjust(3) + ' '
	  s=s+ "%11.2f " %atom.x
	  s=s+ "%7.2f " %atom.y
	  s=s+ "%7.2f " %atom.z
	  s=s+ "%5.2f " %atom.occupancy
	  s=s+ "%5.2f " %atom.temp_factor
	  s=s+ atom.element.rjust(11) + "\n"
     except KeyError, e:
        print "Chain.to_pdb() warning:  No atom record for %s in %s%s." %(atom_name,residue_index,residue.symbol3)
    if len(atoms) > 0:
      s=s+ "TER\n"

    return s
