#!/usr/bin/python

#seq_model.py
#Sequence Models  python module
#
# models protein sequences
# Chain
# Secel (Helix, Strand/Sheet, Coil)
# Residue
# Atom

import random
from libpyGORGON import PDBAtom,Vector3DFloat

#--------------------------------------
class Atom:
  def __init__(self,element,x,y,z, occupancy=1.0,temp_factor=1.0):
		self.element=element
		self.x=x
		self.y=y
		self.z=z
		self.occupancy=occupancy
		self.temp_factor=temp_factor
		print x,y,z



#--------------------------------------
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


#--------------------------------------
class Residue:
  _aa_dict={}
  _aa_dict['A']='ALA'
  _aa_dict['C']='CYS'
  _aa_dict['D']='ASP'
  _aa_dict['E']='GLU'
  _aa_dict['F']='PHE'
  _aa_dict['G']='GLY'
  _aa_dict['H']='HIS'
  _aa_dict['I']='ILE'
  _aa_dict['K']='LYS'
  _aa_dict['L']='LEU'
  _aa_dict['M']='MET'
  _aa_dict['N']='GLN'
  _aa_dict['P']='PRO'
  _aa_dict['Q']='GLN'
  _aa_dict['R']='ARG'
  _aa_dict['S']='SER'
  _aa_dict['T']='THR'
  _aa_dict['V']='VAL'
  _aa_dict['Y']='TYR'
  _aa_dict['W']='TRP'

  _aa_dict['ALA']='A'
  _aa_dict['CYS']='C'
  _aa_dict['ASP']='D'
  _aa_dict['GLU']='E'
  _aa_dict['PHE']='F'
  _aa_dict['GLY']='G'
  _aa_dict['HIS']='H'
  _aa_dict['ILE']='I'
  _aa_dict['LYS']='K'
  _aa_dict['LEU']='L'
  _aa_dict['MET']='M'
  _aa_dict['ASN']='N'
  _aa_dict['PRO']='P'
  _aa_dict['GLN']='Q'
  _aa_dict['ARG']='R'
  _aa_dict['SER']='S'
  _aa_dict['THR']='T'
  _aa_dict['VAL']='V'
  _aa_dict['TYR']='Y'
  _aa_dict['TRP']='W'


  def __init__(self, symbol):
		symbol=symbol.upper()
		if len(symbol)==1:
			self.symbol1=symbol
			self.symbol3=Residue._aa_dict[symbol]

		elif len(symbol)==3:
			self.symbol3=symbol
			self.symbol1=Residue._aa_dict[symbol]

		else:
			raise ValueError, "Residue must be instantiated with either a 1-letter or 3-letter symbol" 

		self.atoms={}



  def __str__(self):
    return self.symbol1



#--------------------------------------
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



#--------------------------------------
class Helix(Secel):
  serial_num=0
  def __init__(self, chain, label, start_index, stop_index):
    Secel.__init__(self, chain, label, start_index, stop_index)
    self.type="helix"
		     

  def to_pdb(self):
    Helix.serial_num=Helix.serial_num+1
    helix_id="H1"
    #nit_res_name=self.chain.residue_list[self.start_index-1].symbol3
    init_res_name=self.chain.residue_list[self.start_index].symbol3
    init_chain_id=self.chain.chain_id
    init_seq_num=self.start_index
    #nd_res_name=self.chain.residue_list[self.stop_index-1].symbol3
    end_res_name=self.chain.residue_list[self.stop_index].symbol3
    end_seq_num=self.stop_index

    '''
    print "HELIX",
    print str(Helix.serial_num).rjust(4),
    print self.label.rjust(3),
    print init_res_name.rjust(3),
    print init_chain_id.rjust(1),
    print str(init_seq_num).rjust(4),

    print end_res_name.rjust(4),
    print init_chain_id.rjust(1),
    print str(end_seq_num).rjust(4),
    print str(1).rjust(1),

    print str(self.stop_index-self.start_index).rjust(36)
    '''

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

  
  


#--------------------------------------
class Coil(Secel):
  def __init__(self, chain, label, start_index, stop_index):
    Secel.__init__(self, chain, label, start_index, stop_index)
    self.type="loop"

  def to_pdb(self):
    return ''
  


#--------------------------------------
class Strand(Secel):
  def __init__(self, chain, label, start_index, stop_index):
    Secel.__init__(self, chain, label, start_index, stop_index)
    self.type="strand"

  def to_pdb(self):
    print "SHEET"
    return ''
  


#--------------------------------------
class Sheet:
  def __init__(self, strand_list):
    self.strand_list=strand_list



  def add(strand):
    self.strand_list.append(strand)



  def remove(strand):
    self.strand_list.remove(strand)



#--------------------------------------
def main(argv):

  # Instantiate my_chain
  print '\n\n\n********************** executing main() ********************\n'
  my_chain = Chain("ACEFGHIKLACEFGHIKLMNPYVWQPMIKESMATTHEW")
  print 'my_chain=%s' %my_chain

  my_chain.secel_list.append (Coil (my_chain,'l0',1,9))
  my_chain.secel_list.append (Helix(my_chain,'h1',10,20))
  my_chain.secel_list.append (Coil (my_chain,'l2',21,25))
  my_chain.secel_list.append (Helix(my_chain,'h3',26,30))
  my_chain.secel_list.append (Coil (my_chain,'l4',31,38))


  # Instantiate sub_chain
  sub_chain=my_chain[14:20]
  print 'sub_chain=%s' %sub_chain


  # Add coordinate records to my_chain
  for residue in my_chain[3:-5]:
  #or residue in my_chain:
      x=random.gauss(20.0,5.0)
      y=random.gauss(20.0,5.0)
      z=random.gauss(20.0,5.0)
      residue.atoms['CA']=Atom('C',x,y,z)


  # Test to_pdb on both chains
  print '\nmy_chain.to_pdb\n' + my_chain.to_pdb(backbone_only=True)

  print '\nsub_chain.to_pdb\n' + sub_chain.to_pdb(backbone_only=True)


 
  '''
  # Test chain-slicing
  #ew_chain = Chain.read('inputfile.txt')
  print "my_chain = %s" %my_chain

  print "\nmy_chain[:10]"
  print my_chain[:10]

  print "\nmy_chain[:-3]"
  print my_chain[:-3]

  print "\nmy_chain[-1:]"
  print my_chain[-1:]

  print "\nmy_chain[-3:]"
  print my_chain[-3:]

  print "\nmy_chain[3:7]"
  print my_chain[3:7]
  '''



if __name__=="__main__":
  from sys import argv
  main(argv)

