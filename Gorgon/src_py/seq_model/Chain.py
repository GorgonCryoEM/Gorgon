#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Chain
# Class Description: Class that models polypeptide chains, whih are sequences of Residue objects.
#                    More info in: seq_model-doc.txt
#

from Residue import Residue
from Atom import Atom
from Helix import Helix
from Sheet import Sheet
#from Strand import Strand
from Coil import Coil

class Chain(object):
  """Chain objects represent single polypeptide chains, which are sequences of Residue objects"""
  chainsDict = {}

  #Chain ID management
  __lastChainID = 64  # chr(64) is A;  chr(65) is B, etc.

  @classmethod
  def __nextChainID(cls):
    Chain.__lastChainID=Chain.__lastChainID+1
    return chr(Chain.__lastChainID)




  #Chain Constructor
  def __init__(self,char_string=None):
    self.residueList={}
    self.secelList={}
    self.chainID = Chain.__nextChainID()
    self.atoms = {}

    self.helices = {}
    self.sheets = {}
    self.orphanStrands = {}

    i=1
    for char in char_string:
      self.residueList[i]=Residue(char,self)
      i=i+1

    Chain.chainsDict[self.chainID]=self



  @classmethod
  def __loadFromPDB (cls,filename):
    infile=open(filename,'U')
    lines=infile.readlines()

    result=Chain('')
    residue=None
    for line in lines:
      if line[0:4]=='ATOM':
  
        residueIndex=    int (line[22:26])
        if residueIndex not in result.residueRange():
          residue   = Residue(line[17:20].strip(),result)
      
        chainID     =         line[21:22]
        serialNo    =    int (line[6:11].strip())
      
        atomName    =         line[12:16].strip()
        element     =         line[76:78].strip()
        tempFactor  =  float (line[60:66].strip())
        occupancy   =  float (line[54:60].strip())
      
        x           =  float (line[30:38])
        y           =  float (line[38:46])
        z           =  float (line[46:54])
      
        if residueIndex not in result.residueRange():
          result[residueIndex]=residue
        atom=Atom(element, x,y,z, residue, serialNo, occupancy ,tempFactor)
      
        residue.atoms[atomName]=atom
      
        result.atoms[serialNo]=atom

      elif line[0:6].strip()=='HELIX':
        Helix.parsePDB(line,result)

      elif line[0:6].strip()=='SHEET':
        Sheet.parsePDB(line,result)

    infile.close()
    return result



  @classmethod
  def load (cls,filename):
    extension = filename.split('.')[-1].lower()
    if extension == 'pdb':
      return Chain.__loadFromPDB(filename)
    else:
      raise NotImplementedError, 'NYI'


  def __repr__(self):
    s=''
    residueRange=self.residueRange()
    for index in residueRange:
      if (index!=1)  and not (index-1) in residueRange:
        s=s+'...'
      s=s + self[index].symbol1
    return s




  def __iter__(self):
    # suppose residueList keys are {3,4,5,6,7,8, 13,14,15,16, 21,22,23}
    #eys=self.residueList.keys().sort()
    keys=self.residueList.keys()
    key_index=0
    next_residue_key=keys[key_index]


    while True:
      yield self.residueList[next_residue_key]
      key_index=key_index+1
      try:
        next_residue_key=keys[key_index]
      except IndexError:
        raise StopIteration 



  def __convertNegativeIndex(self,i):
    return len(self)+i+1

  def __setitem__(self,i, residue):
    if isinstance(residue,Residue):
      self.residueList[i]=residue
    elif isinstance(residue,str):
      self.residueList[i]=Residue(residue,self)
    else:
      raise TypeError


  def addSecel(self, secel):
    for index in range(secel.startIndex, secel.stopIndex+1):
      self.secelList[index]=secel

  def addHelix(self, serialNo, helix):
    self.helices[serialNo]=helix
    self.addSecel(helix)

  def addStrand(self, strand, strandNo, sheetID=None):
    if sheetID is None:
      self.orphanStrands[strandNo]=strand
    else:
      self.sheets[sheetID].strandList[strandNo]=strand
    self.addSecel(strand)

  def addSheet(self, sheetID, sheet):
    if not self.sheets.has_key(sheetID):
      self.sheets[sheetID]=sheet

  def getSecelByIndex(self,i):
    if self.secelList.has_key(i):
      return self.secelList[i]
    else:
      return Coil(self,0, 'no-label',i,i)


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
        #start=len(self)+i.start+1
        start=self.__convertNegativeIndex(i.start)

      if i.stop is None: 
        stop=len(self)
      elif i.stop < 0:
        #stop=len(self)+i.stop+1
        stop=self.__convertNegativeIndex(i.stop)
      return self.__slicehelper(start,stop)

    # branch for non-slices
    else:
      if i<0:
        i=self.__convertNegativeIndex(i)
      #rint 'getitem(%i)' %i
      return self.residueList[i]


  def __slicehelper(self,i,j):
    #rint 'slice_helper(%s,%s)' %(i,j)
    keys=self.residueList.keys()
    keys.sort()
    start=keys.index(i)
    stop=keys.index(j)
    del keys[stop+1:]
    del keys[:start]

    new_chain=Chain('')
    for index in keys:
      new_chain[index]=self[index]


    # Slice output should be annotated for helix, strand, coil
    # ???  what about strands that become orphaned?
    '''
    for label in self.secelList.keys():
      secel=self.secelList[label]
      need_to_duplicate_secel=False
      
      # Secel startIndex
      if new_chain.residueList.has_key(secel.startIndex):
  startIndex=secel.startIndex
        need_to_duplicate_secel=True
      else: 
  keys=new_chain.residueList.keys()
  keys.sort()
  startIndex=keys[0]

      # Secel stopIndex
      if new_chain.residueList.has_key(secel.stopIndex):
  stopIndex=secel.stopIndex
        need_to_duplicate_secel=True
      else: 
  keys=new_chain.residueList.keys()
  keys.sort()
  stopIndex=keys[-1]

      if need_to_duplicate_secel:
        if isinstance(secel,Helix):  cls=Helix
        if isinstance(secel,Strand): cls=Strand
        if isinstance(secel,Coil):   cls=Coil
        new_chain.secelList[label]=cls(new_chain, new_chain.chainID, startIndex, stopIndex)
        #rint 'secel.__class__=%s' %secel.__class__
    '''

    return new_chain



  # len(my_chain) returns the length of residueList
  def __len__(self):
    return_value= len(self.residueList)
    return return_value



  def clear_coordinates(self):
    for index in self.residueRange():
      self[index].clearAtoms()



  def append(self,residue):
    if len(self.residueList)==0:
      #self.residueList[1]=residue
      self.__setitem__(1,residue)
      return 1
    else:
      keys=self.residueList.keys()
      keys.sort()
      key=1+keys[-1]

      #self.residueList[key]=residue
      #this fails unit tests if residue is a str, must delegate to __setitem__ intead

      self.__setitem__(key,residue)
      return key



  def residueRange(self):
    return sorted(self.residueList.keys())


  def saveToPDB(self, filename):
    s = self.toPDB()
    outfile=open(filename,'w')
    outfile.write(s)
    outfile.flush()
    outfile.close()

  #Return a pdb-compliant string
  def toPDB(self, backboneOnly=False, suppressWarning=False): 
    """decomposes chain into constituent residues and atoms to render pdb coordinate model.  residue is ommitted if it has no atoms."""
    #Not Thread-Safe
    atom_index=1
    Helix.serialNo=0  #This is what makes it not thread-safe

    s=''

    for serialNo in sorted(self.helices.keys()):
      helix=self.helices[serialNo]
      s=s+helix.toPDB()

    for sheetID in sorted(self.sheets.keys()):
      sheet=self.sheets[sheetID]
      s=s+sheet.toPDB(sheetID)

    for strand in self.orphanStrands:
      s=s+strand.toPDB()

    for residue_index in self.residueRange():
      residue=self[residue_index]

      if backboneOnly:
        atoms = ['CA']
        #print 'writing CA only'
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
          s=s+ self.chainID.rjust(1) + ' ' #chainID
          s=s+ str(residue_index).rjust(3) + ' '
          s=s+ "%11.3f " %atom.x
          s=s+ "%7.3f " %atom.y
          s=s+ "%7.3f " %atom.z
          s=s+ "%5.2f " %atom.occupancy
          s=s+ "%5.2f " %atom.tempFactor
          s=s+ atom.element.rjust(11) + "\n"
      except KeyError:
        if not suppressWarning:
          print "Chain.toPDB() warning:  No atom record for %s in %s%s." %(atom_name,residue_index,residue.symbol3)
    if len(atoms) > 0:
      s=s+ "TER\n"

    return s
