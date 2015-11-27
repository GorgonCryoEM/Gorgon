#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Chain
# Class Description: Class that models polypeptide chains, whih are sequences of Residue objects.
#                    More info in: seq_model-doc.txt
#
from seq_model.Residue import Residue
from seq_model.Helix import Helix
from seq_model.Sheet import Sheet
from seq_model.Coil import Coil
from seq_model.Strand import Strand
from libpyGORGON import PDBBond

try:
  from PyQt4 import QtCore, QtGui
  qtEnabled=True
  baseClass=QtCore.QObject
except:
  qtEnabled=False
  baseClass=object


class Chain(baseClass):
  '''
Chain objects represent single polypeptide chains, which are sequences
of Residue objects
  '''
  
  sideChainConnectivity = {
        'ALA':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB']],
        'ARG':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD'], ['CD', 'NE'], ['NE', 'CZ'], ['CZ', 'NH1'], ['CZ', 'NH2']],
        'ASP':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'OD1'], ['CG', 'OD2']],
        'ASN':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'OD1'], ['CG', 'ND2']],
        'CYS':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'SG']],
        'GLY':[['N','CA'], ['CA','C'], ['C','O']],
        'GLN':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD'], ['CD', 'NE2'], ['CD', 'OE1']],
        'GLU':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD'], ['CD', 'OE1'], ['CD', 'OE2']],
        'HIS':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD2'], ['CD2', 'NE2'], ['NE2', 'CE1'], ['CG','ND1'], ['ND1','CE1']],
        'ILE':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG1'], ['CG1', 'CD1'], ['CB', 'CG2']],
        'LEU':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD1'], ['CG', 'CD2']],
        'LYS':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD'], ['CD', 'CE'], ['CE', 'NZ']],
        'MET':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'SD'], ['SD', 'CE']],
        'PHE':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD1'], ['CG', 'CD2'], ['CD1', 'CE1'], ['CD2', 'CE2'], ['CE1', 'CZ'], ['CE2', 'CZ']],
        'PRO':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD'], ['CD', 'N']],
        'SER':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'OG']],
        'THR':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'OG1'], ['CB', 'CG2']],
        'TRP':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD1'], ['CG', 'CD2'], ['CD1', 'NE1'], ['NE1', 'CE2'], ['CD2', 'CE2'], ['CD2', 'CE3'], ['CE3', 'CZ3'], ['CZ3', 'CH2'], ['CH2', 'CZ2'], ['CE2', 'CZ2']],
        'TYR':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG'], ['CG', 'CD1'], ['CG', 'CD2'], ['CD1', 'CE1'], ['CD2', 'CE2'], ['CE1', 'CZ'], ['CE2', 'CZ'], ['CZ', 'OH']],
        'VAL':[['N','CA'], ['CA','C'], ['C','O'], ['CA', 'CB'], ['CB', 'CG1'], ['CB', 'CG2']]
        }
      
  chainsDict = {}
  __lastAuto_pdbID = 0
  #__selectedChainKey = None

  #Chain Constructor
  def __init__(self,char_string=None,qparent=None, pdbID=None, chainID='A'):
    #CURRENT: If the chain with a given (pdbID, chainID) key already exists, reference that
    #TODO: perhaps we should change this behavior, in case someone modified a chain object then imported a PDB with a chain of the same name
    if qparent and qtEnabled:
        super(QtCore.QObject,self).__init__(qparent)
        '''
        #Mike's addition:
        self.parent=qparent
        self.setViewer(qparent.viewers['calpha'])
        print 'yes viewer was set'
        '''

    self.residueList={}
    self.secelList={}
    self.selectedResidues=[]
    self.atoms = {}

    self.helices = {}
    self.sheets = {}
    self.coils = {}
    self.orphanStrands = {}
    if (pdbID, chainID) in Chain.getChainKeys():  #What to do if the key already exists
        self = Chain.getChain( (pdbID, chainID) )
        return
    if pdbID:
        self.pdbID = pdbID
    else:
        self.pdbID = self.__createUniquePDBID()
    self.chainID = chainID
    self.key = (self.pdbID, self.chainID)

    i=1
    for char in char_string:
        self.residueList[i]=Residue(char,self)
        i += 1
    Chain.chainsDict[self.key]=self


  @classmethod
  def __createUniquePDBID(cls):
    '''
If no PDB ID is specified when creating a chain, this method will 
create one that is simply a number preceded by enough underscores to
give a 4 character identifier.
    '''
    #TODO:We might want to modify this to use any unused numbers (after a rename)
    pdbNum = cls.__lastAuto_pdbID + 1
    numUnderscores = 4 - len(str(pdbNum))
    assert numUnderscores >= 0
    #TODO: We need to figure out how to handle this exception
    pdbID = '_'*numUnderscores + str(pdbNum)
    cls.__lastAuto_pdbID += 1
    return pdbID

  @classmethod
  def __loadFromFASTA (cls,filename,qparent=None):
    '''
This creates a Chain object from a FASTA file.
    '''
    infile=open(filename,'U')
    lines=infile.readlines()

    charString=''
    for line in lines:
      if line[0] != '>':
        charString=charString+line.strip()

    if qparent and qtEnabled:
      result=Chain(charString, qparent=qparent)
    else:
      result=Chain(charString)
    #Chain.setSelectedChainKey(result.getIDs())
    return result


  @classmethod
  def __loadFromPDB (cls,filename,qparent=None, whichChainID=None):
    '''
This loads the specified chain ID from a PDF file and returns a Chain 
object. If no chain ID is specified, it loads the first chain.
    '''
    #print Chain.getChainKeys()
    if qparent and qtEnabled:
        result = Chain('', qparent=qparent)
    else:
        result = Chain('')
    
    header = open(filename, 'U')
    headerLine = header.read()
    if headerLine[:6] == 'HEADER':
        pdbID = headerLine[62:66]
    else:
        pdbID = cls.__createUniquePDBID()
    header.close()
    
    residue = None
    firstChain = None
    for line in open(filename, 'U'):	#calls the iterator for the file object each time the loop is run - don't have to load entire file into memory
        if line[0:4]=='ATOM':
            chainID = line[21:22]
            if chainID == ' ':
                chainID = 'A'
            if whichChainID and chainID != whichChainID:	#Search for the specified chainID (if one is specified), otherwise we find the first chain.
                continue
            if not firstChain:	#Sets the value of the first and only chain we will store
                firstChain = chainID
                ####if the chain key already exists, point to that chain object
                ####perhaps this should be modified
                if not (pdbID, firstChain) in cls.getChainKeys():
                    result.setIDs(pdbID, firstChain)
                else:
                    result = cls.getChain( (pdbID, firstChain) )
                    break
            if firstChain and chainID != firstChain:		#If we've gone past the only chain we want to store, we will break out of the for loop
                break
            
            residueIndex = int( line[22:26] )
            if residueIndex not in result.residueRange():
                residue = Residue( line[17:20].strip(), result ) 
                result[residueIndex] = residue
            else:
                residue = result[residueIndex]
            
            serialNo    = int( line[6:11].strip() )
            atomName    = line[12:16].strip()
            element     = line[76:78].strip()
            try:
                tempFactor  = float( line[60:66].strip() )
            except ValueError:
                tempFactor = None
            try:
                occupancy   = float( line[54:60].strip() )
            except ValueError:
                occupancy = None
            try: 
                x = float( line[30:38] )
                y = float( line[38:46] )
                z = float( line[46:54] )
                
                atom = residue.addAtom(atomName, x,y,z, element, serialNo, occupancy, tempFactor)
                #residue.atoms[atomName]=atom            
                result.atoms[serialNo]=atom
                #Chain.chainsDict[result.key] = result
            except ValueError:
                print 'Chain.__loadFromPDB--no coordinates', 

        elif line[0:6].strip()=='HELIX':
            Helix.parsePDB(line,result)
        elif line[0:6].strip()=='SHEET':
            Sheet.parsePDB(line,result)
            
    #Setting up coils
    startList = []
    endList = []
    for secelIx, secel in result.secelList.items():
        startList.append(secel.startIndex)
        endList.append(secel.stopIndex)
    startList.sort()
    endList.sort()
    
    if result.residueRange():
        startPt = min(result.residueRange())
        coilIx = 1
        for i in range(len(startList)):
            if startPt < startList[i] :
                result.addCoil(coilIx, Coil(result, coilIx, 'L' + str(coilIx), startPt, startList[i]-1))
                coilIx = coilIx + 1
            startPt = endList[i] + 1
        
        if startPt < max(result.residueRange()):
            result.addCoil(coilIx, Coil(result, coilIx, 'L' + str(coilIx), startPt, max(result.residueRange())))        
            
                
        Chain.chainsDict[result.key] = result
        #Chain.setSelectedChainKey(result.getIDs())
        
    return result

  @classmethod
  def __loadFromSeq(cls, filename, qparent=None):
        '''
Sequence files are a file type we defined. The first line gives the 
one-letter abbreviations for the sequence. The line below it shows the
predicted secondary structure element for each residue as "H" for
helix, "E" for strand, and "-" otherwise.  
Ex 1:
GAPCSTLARFKEI
HHHHHH--EEEE
Ex 2:
START 45
SAPQRVPELYC
EEEHHHHHH-

The first line may give a start residue (useful for post-translational
modifications). That line will be interpreted and removed. Linebreaks 
are then removed. Finally, the first half of the remaining characters 
are interpreted as sequence, and the second half are treated as 
structure predictions.  
        '''
        F = open(filename)
        lines = []
        for line in F:
            line = line.strip()
            lines.append(line)
        if lines[0][:5].upper() == 'START':
            firstLine = lines.pop(0)
            startIndex = int(firstLine.split()[-1])
            print startIndex
        else:
            startIndex = 1
        stopIndex = None  
        
        lines = ''.join(lines)
        linesSize = len(lines)
        try:
            assert (linesSize % 2 == 0)
        except:
            "The file does not have an equal number of reisdues and secondary structure indicators."
        
        sequence = lines[:(linesSize//2)]
        print sequence
        structure = lines[(linesSize//2):]
        print structure
        
        if startIndex == 1:
            newChain = Chain(sequence,  qparent)
        else:
            newChain = Chain('', qparent)
            n = 0
            for char in sequence:
                newChain[startIndex+n] = Residue(char, newChain)
                n += 1
                
        helix = 'H'
        strand = 'E'
        coil = '-'
        elementNum, helixSerialNum, strandSerialNum, coilSerialNum = 1, 1, 1, 1
        currentElement = structure[0]
        assert currentElement in (helix, strand, coil)
        i = startIndex+1
        for character in structure[1:]:
            if character:
                assert character in (helix, strand, coil)
                if character == currentElement:
                    i += 1
                    #continue   #redundant right now
                else:
                    stopIndex = i - 1
                    if currentElement == helix:
                        newHelix = Helix(chain=newChain, serialNo=helixSerialNum, 
                                    label = 'H' + str(elementNum),
                                    startIndex=startIndex, stopIndex=stopIndex)
                        newChain.addHelix(serialNo = helixSerialNum, helix = newHelix)
                        helixSerialNum += 1
                        elementNum += 1
                    elif currentElement == strand:
                        newStrand = Strand(chain=newChain, strandNo=strandSerialNum, 
                                        label = 'S' + str(elementNum),
                                        startIndex=startIndex, stopIndex=stopIndex)
                        newChain.addStrand(strand = newStrand, strandNo=strandSerialNum)
                        strandSerialNum += 1
                        elementNum += 1
                    elif currentElement == coil:
                        newCoil = Coil(chain=newChain, serialNo=coilSerialNum, 
                                        label = 'Coil', startIndex=startIndex, stopIndex=stopIndex)
                    else:
                        pass
                    startIndex = i
                    stopIndex = None
                    currentElement = character
                    i += 1
        return newChain

  @classmethod
  def __nextChainID(cls):
    '''
If a chain ID isn't specified, this function generatres one.
    '''
    #TODO: make sure this will not eventually overwrite chain IDs from a PDB file that doesn't start at A.
    Chain.__lastChainID=Chain.__lastChainID+1
    return chr(Chain.__lastChainID)

  @classmethod
  def getChain(cls, key):
    '''
Given a (pdbID, chainID) key, returns a chain object.
    '''
    return cls.chainsDict.get(key)	#{}.get() can handle non-existent key errors

  @classmethod
  def getChainIDsFromPDB(cls, filename, qparent=None):
    """
This only finds the first list of chains in the PDB file.  If the file 
defines multiple molecules, this would only find the chains for the 
first one.
    """
    extension = filename.split('.')[-1].lower()
    if extension == 'pdb':
        linelist = []
        for line in open(filename, 'U'):
            if line[:6] == 'COMPND' and line[10:70].split(':')[0].strip() == 'CHAIN':
                linelist = line[17:].split(', ')
                linelist[0] = linelist[0].strip()
                if ';' in linelist[-1]:
                    linelist[-1] = linelist[-1].split(';')[0]	#removes the terminating semicolon and extra whitespace
                    while True:
                        try: linelist.remove('NULL')
                        except: break
                return linelist
        if linelist == []:
            return []
    else:
      raise NotImplementedError, 'NYI'

  #@classmethod
  #def getSelectedChainKey(cls):
  #  '''
  #  If we have multiple chains loaded into memory, this returns the key for the one that is currently selected.
  #  '''
  #  return cls.__selectedChainKey

  @classmethod
  def getChainKeys(cls):
      '''
This returns the keys (PDB ID, Chain ID) for all the chains loaded into
memory.
      '''
      return cls.chainsDict.keys()

  @classmethod
  def getViewer(cls):
    '''
This returns the viewer associated with the chain class.
    '''
    return Chain.__viewer

  @classmethod
  def load (cls,filename,qparent=None, whichChainID=None):
    '''
This calls the correct load method based on the file extension.
    '''
    extension = filename.split('.')[-1].lower()
    if extension == 'pdb':
      return Chain.__loadFromPDB(filename,qparent, whichChainID)
    elif extension == 'fasta' or extension=='fa' or extension=='fas':
      return Chain.__loadFromFASTA(filename,qparent)
    elif extension == 'seq':
         return Chain.__loadFromSeq(filename,qparent)
    else:
      raise NotImplementedError, 'NYI'

  @classmethod
  def loadAllChains(cls, filename, qparent=None):
    '''
This loads all the chains specified in a PDB file.  
    '''
    chain = None
    pdbID = None
    chains = []
    chainIDs = cls.getChainIDsFromPDB(filename,qparent)
    while True:
        try: chanIDs.remove('NULL')
        except: break
    if not chainIDs:
        chainIDs = [None]
    for whichChainID in chainIDs:
        chain = Chain.load(filename, qparent, whichChainID)
        if not pdbID: 
            pdbID = chain.getPdbID()
        elif pdbID != chain.getPdbID():
            chain.setIDs(pdbID, chain.getChainID())  #Needed if PDB ID is auto-generated
        chains.append(chain.getIDs())
        cls.chainsDict[chain.key] = chain
    return chains
  
  #@classmethod
  #def setSelectedChainKey(cls, key):
  #  '''
  #  If there are multiple chains loaded into memory, this can be used to set which chain is currently selected.
  #  '''
  #  Chain.__selectedChainKey = key
  @classmethod
  def setViewer(cls, viewer):
    '''
This sets the viewer for the Chain class.
    '''
    Chain.__viewer=viewer
    


  def __convertNegativeIndex(self,i):
    '''
Makes indexing a Chain with a negative index behave similar to lists, 
tuples, and strings. mychain[-1] returns the last residue of the chain.
    '''
    return len(self)+i+1

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
      #Sasakthi: Removed reverse indexing functionality as residues can have negative indices.
      #elif i.start < 0:
      #  #start=len(self)+i.start+1
      #  start=self.__convertNegativeIndex(i.start)

      if i.stop is None: 
        stop=len(self)
      
      #Sasakthi: Removed reverse indexing functionality as residues can have negative indices.
      #elif i.stop < 0:
      #  #stop=len(self)+i.stop+1
      #  stop=self.__convertNegativeIndex(i.stop)
      return self.__slicehelper(start,stop)

    # branch for non-slices
    else:
      #Sasakthi: Removed reverse indexing functionality as residues can have negative indices.
      #if i<0:
      #  i=self.__convertNegativeIndex(i)
      ##rint 'getitem(%i)' %i
      return self.residueList[i]

  def __iter__(self):
    # must handle discontinuous residueList keys such as {3,4,5,6,7,8, 13,14,15,16, 21,22,23}
    #keys=self.residueList.keys().sort()
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

  # len(my_chain) returns the length of residueList
  def __len__(self):
    '''
This returns the length of the residueList.
    '''
    return_value= len(self.residueList)
    return return_value

  def __repr__(self):
    s=''
    residueRange=self.residueRange()
    for index in residueRange:
      if (index!=1)  and not (index-1) in residueRange:
        s=s+'...'
      s=s + self[index].symbol1
    return s

  def __setitem__(self,i, residue):
    if isinstance(residue,Residue):
      self.residueList[i]=residue
    elif isinstance(residue,str):
      self.residueList[i]=Residue(residue,self)
    else:
      raise TypeError

  def __slicehelper(self,i,j):
    #print 'slice_helper(%s,%s)' %(i,j)
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

  def addCalphaBonds(self):
    '''
This adds the Calpha bonds for all the residues in a chain. If there is
a gap in the sequence of residues, this will not place a C-alpha 
bond.setAtom0Ix--one doesn't want a bond between the 97th and 103rd 
residue.
    '''
    cnt = 0
    try: 
        viewer = Chain.getViewer()
    except:
        print 'Error: No viewer is set for Chain!'
        return
    for res0num in self.residueRange():
        atom0 = self[res0num].getAtom('CA')
        if not atom0:
            continue
        if res0num + 1 in self.residueRange():
            atom1 = self[res0num+1].getAtom('CA')
            if not atom1:
                continue
            bond = PDBBond()
            bond.setAtom0Ix(atom0.getHashKey())
            bond.setAtom1Ix(atom1.getHashKey())
            viewer.renderer.addBond(bond)
            cnt = cnt + 1
            
  def addSideChainBonds(self):
    cnt = 0
    try: 
        viewer = Chain.getViewer()
    except:
        print 'Error: No viewer is set for Chain!'
        return
    for i in self.residueRange():
        if self[i].symbol3 in self.sideChainConnectivity:
            for j in range(len(self.sideChainConnectivity[self[i].symbol3])):
                atom0 = self[i].getAtom(self.sideChainConnectivity[self[i].symbol3][j][0])
                atom1 = self[i].getAtom(self.sideChainConnectivity[self[i].symbol3][j][1])
                if atom0 and atom1 :
                    bond = PDBBond()
                    bond.setAtom0Ix(atom0.getHashKey())
                    bond.setAtom1Ix(atom1.getHashKey())
                    viewer.renderer.addSideChainBond(bond)
        # Connecting the C of this residue to the N of the next residue
        if i+1 in self.residueRange():
            atom0 = self[i].getAtom('C')
            atom1 = self[i+1].getAtom('N')
            if atom0 and atom1 :
                bond = PDBBond()
                bond.setAtom0Ix(atom0.getHashKey())
                bond.setAtom1Ix(atom1.getHashKey())
                viewer.renderer.addSideChainBond(bond)
            
                                  
    

  def addSecel(self, secel):
    '''
This adds a secel object to the chain.
    '''
    for index in range(secel.startIndex, secel.stopIndex+1):
      self.secelList[index]=secel

  def addCoil(self, coilNo, coil):
      self.coils[coilNo]=coil
      self.addSecel(coil)

  def addHelix(self, serialNo, helix):
    '''
This adds a helix object to the chain.
    '''
    self.helices[serialNo]=helix
    self.addSecel(helix)

  def addStrand(self, strand, strandNo, sheetID=None):
    '''
This adds a strand object to the chain.
    '''
    if sheetID is None:
      self.orphanStrands[strandNo]=strand
    else:
      self.sheets[sheetID].strandList[strandNo]=strand
    self.addSecel(strand)

  def addSheet(self, sheetID, sheet):
    '''
This adds a sheet object to the chain.
    '''    
    if not self.sheets.has_key(sheetID):
      self.sheets[sheetID]=sheet

  def append(self,residue):
    '''
This appends a residue to the end of the Chain.
    '''
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

  def clear_coordinates(self):
    for index in self.residueRange():
      self[index].clearAtoms()

  def findIndexForRes (self, inputRes):
    '''
This finds the index for a residue object.
    '''
#    Generator objects turn out to be a bit slower than the for loop, which surprises me (Ross).
#    indexGenerator = (index for index in self.residueRange()[::-1] if self.residueList[index] is inputRes) #Generator Object
#    return indexGenerator.next()
    for index in self.residueRange()[::-1]:
      if self.residueList[index] is inputRes:
        return index
    
  def fillGaps(self):
    '''
If there are missing residues in a chain, this inserts "X" for unknown
residues.
    '''
    for i in self.residueRange():
      while i+1 not in self.residueRange():
        if i+1>self.residueRange()[-1]:
          break
        self[i+1]=Residue('X')
        i=i+1

  def getChainID(self):
    return self.chainID

  def getIDs(self):
    """
This returns (pdbID, chainID) for a chain instance.
    """
    return (self.getPdbID(), self.getChainID())

  def getPdbID(self):
    '''
This returns the PDB ID for the Chain.
    '''
    return self.pdbID

  def getSecelByIndex(self,i):
    '''
This returns the Secel present at residue number i, or a 
single-residue Coil if no Secel is present at that residue number.
    '''
    if self.secelList.has_key(i):
      return self.secelList[i]
    else:
      return Coil(self,0, 'no-label',i,i)

  def getSelection(self):
    '''
This returns the list of selected residues. The selection is a list
object where each element is an index in the Chain.
    '''
    return self.selectedResidues

  def getFirstResidueIndex(self):
    keys=self.residueList.keys()
    keys.sort()
    return keys[0]

  def getLastResidueIndex(self):
    keys=self.residueList.keys()
    keys.sort()
    return keys[len(keys)-1]


  def removeSecel(self, secel):
    '''
This removes a Secel from the Chain.
    '''
    type = secel.__class__.__name__
    if type == 'Helix':
        if secel in self.helices.values():
            for key in self.helices.keys():
                if self.helices[key] is secel:
                    self.helices.pop(key)
    elif type == 'Strand':
        if secel in self.orphanStrands.values():
            for key in self.orphanStrands.keys():
                if self.orphanStrands[key] is secel:
                    self.orphanStrands.pop(secel)
    for i in range(secel.startIndex, 1+secel.stopIndex):
        self.secelList.pop(i)
    
  def residueRange(self):
    '''
This returns a list of all the residue numbers.
    '''
    return sorted(self.residueList.keys())

  def saveToPDB(self, filename):
    s = self.toPDB()
    outfile=open(filename,'w')
    outfile.write(s)
    outfile.flush()
    outfile.close()

  def setIDs(self, new_pdbID, new_chainID):
    """
This changes the pdbID and chainID attributes of a Chain instance.
    """
    #### We need to figure out how to handle the possible exception
    assert (new_pdbID, new_chainID) != self.key
    value = self.chainsDict.pop(self.key)
    self.pdbID = new_pdbID
    self.chainID = new_chainID
    self.key = (self.pdbID, self.chainID)
    Chain.chainsDict[self.key] = value

  def setSelection(self, newSelection=None, removeOne=None, addOne=None, addRange=None):
    '''
Sets the selection attribute to reflect a new set of selected residues
The selection is a list object of indices in the Chain
  REPLACE the existing selection: 'newSelection' parameter
  REMOVE ONE RESIDUE from the existing selection: 'removeOne' parameter
  ADD ONE RESIDUE to the existing selection: use 'addOne' parameter
  ADD A RANGE OF RESIDUES to the existing selection: use 'addRange' 
  parameter
    '''
    selectionToClear = self.getSelection()
    for i in selectionToClear:
        if(i in self.residueRange()):
            atom = self[i].getAtom('CA')
            if atom:
                atom.setSelected(False)    
                
    if newSelection is not None:
      self.selectedResidues=newSelection

    elif removeOne is not None:
      if removeOne in self.selectedResidues:
        self.selectedResidues.remove(removeOne)

    elif addOne is not None:
      if addOne not in self.selectedResidues:
        self.selectedResidues.append(addOne)

    elif addRange is not None:
      self.selectedResidues.extend(addRange)

    selectionToAdd = self.getSelection()
    for i in selectionToAdd:
        if(i in self.residueRange()):
            atom = self[i].getAtom('CA')
            if atom:
                atom.setSelected(True)

    if qtEnabled:
      self.emit( QtCore.SIGNAL('selection updated'))

  #Return a pdb-compliant string
  def toPDB(self, backboneOnly=False, CAlphaPlaceholders=True,  verbose=True): 
    """
This decomposes chain into constituent residues and atoms to render pdb
coordinate model.
If backboneOnly=True, only C-alpha atoms are saved. Otherwise, all 
atoms are saved.
If CAlphaPlaceholders=True, the PDB file will have 'CA' ATOM entries
with whitespace where coordinates would go.
If CAlphaPlaceholders=False, residues with no atoms will be ignored.
    """
    #Change: uses a C-alpha placeholder atom entry with no x,y,z coordinates if the residue has no atoms
    #Not Thread-Safe
    if self.getPdbID()[0] == '_':
        header = ''
    else:
        header = ('HEADER' + ' '*56 + self.getPdbID()).ljust(80) + '\n'
    atom_index=1
    Helix.serialNo=0  #This is what makes it not thread-safe
    dateTime = str(QtCore.QDateTime.currentDateTime().toString())
    gorgonLine1 = 'REMARK   5'.ljust(80) + '\n'
    gorgonLine2 = ('REMARK   5 Gorgon (C) 2005-2008 output on %s' % dateTime).ljust(80) + '\n'
    s = header + gorgonLine1 + gorgonLine2
    
    i = 1
    residueIndices = self.residueRange()
    for index in residueIndices[::13]:
        resList = ['   ']*13
        for n in range(13):
            try:
                resList[n] = self[index+n].symbol3
            except (KeyError,  IndexError):
                continue                
        line = 'SEQRES %s %s %s  %s %s %s %s %s %s %s %s %s %s %s %s %s' % ( str(i).rjust(3), self.getChainID(), str(len(residueIndices)).rjust(4), 
                                                                            resList[0], resList[1], resList[2], resList[3], resList[4], resList[5], resList[6], 
                                                                            resList[7], resList[8], resList[9], resList[10], resList[11], resList[12] )
        line += ' '*10 + '\n'
        s += line
        i += 1
    
    for serialNo in sorted(self.helices.keys()):
      helix=self.helices[serialNo]
      s=s+helix.toPDB()

    for sheetID in sorted(self.sheets.keys()):
      sheet=self.sheets[sheetID]
      s=s+sheet.toPDB(sheetID)

    #TODO: figure out how to handle orphan strands
    for strandID in sorted(self.orphanStrands.keys()):
      strand = self.orphanStrands[strandID]
      s=s+strand.toPDB()

    for residue_index in self.residueRange():
      residue=self[residue_index]

      if backboneOnly:
        atoms = ['CA']
      else:
        atoms=residue.getAtomNames()

      try:
        serial = str(atom_index).rjust(5)
        altLoc = ' '
        resName = residue.symbol3
        chainID = self.chainID
        resSeq = str(residue_index).rjust(4)
        iCode = ' '
        occupancy = ' '*6 #"%6.2f " %atom.getOccupancy()
        tempFactor = ' '*6 #"%6.2f " %atom.getTempFactor()
        
        if not atoms:
            if CAlphaPlaceholders:
                name = 'CA'.center(4)
                x = ' '*8
                y = ' '*8
                z = ' '*8
                element = ' C'
                charge = '  '
                
                line = 'ATOM  %s %s%s%s %s%s%s   %s%s%s%s%s          %s%s\n' % (serial,  name,  altLoc,  resName,  chainID,  
                                                                                resSeq,  iCode,  x,  y,  z,  occupancy,  tempFactor,  element,  charge)
                s = s + line
                atom_index += 1
        for atom_name in atoms:
            atom=residue.getAtom(atom_name)
            
            serial = str(atom_index).rjust(5)
            
            name = str(atom_name).center(4)
            x = "%8.3f" %atom.getPosition().x()
            if len(x) > 8:
                raise ValueError
            y = "%8.3f" %atom.getPosition().y()
            if len(y) > 8:
                raise ValueError
            z = "%8.3f" %atom.getPosition().z()
            if len(z) > 8:
                raise ValueError
            element = atom.getElement().rjust(2)
            charge = '  '
            line = 'ATOM  %s %s%s%s %s%s%s   %s%s%s%s%s          %s%s\n' % (serial,  name,  altLoc,  resName,  chainID,  
                                                                            resSeq,  iCode,  x,  y,  z,  occupancy,  tempFactor,  element,  charge)
            s = s + line
            atom_index += 1
            
            #Mike's method below:
            '''
            atom=residue.getAtom(atom_name)
            s=s+ "ATOM" + ' '
            s=s+ str(atom_index).rjust(6) + ' '
            atom_index=atom_index+1 
            s=s+ atom_name.rjust(3) + ' '
            s=s+ residue.symbol3.rjust(4) + ' '
            s=s+ self.chainID.rjust(1) + ' ' #chainID
            s=s+ str(residue_index).rjust(3) + ' '
            s=s+ "%11.3f " %atom.getPosition().x()
            s=s+ "%7.3f " %atom.getPosition().y()
            s=s+ "%7.3f " %atom.getPosition().z()
            s=s+ "%5.2f " %atom.getOccupancy()
            s=s+ "%5.2f " %atom.getTempFactor()
            s=s+ atom.getElement().rjust(11) + '  ' +"\n"
            '''
      except KeyError:
        if verbose:
          print "Chain.toPDB() warning:  No atom record for %s in %s%s." %(atom_name,residue_index,residue.symbol3)
    if self.residueRange():
      s=s+ "TER\n"

    return s
    
  def toSEQ(self):
    '''
This returns a string in the format of an SEQ file for the Chain.
    '''
    lines = []
    startIndex = min(self.residueRange())
    if startIndex != 1:
        lines.append("START " + str(startIndex))
    lines.append( repr(self).lstrip('.') )
    structure = []
    for i in range( startIndex, max(self.residueRange()) + 1):
        secel = self.getSecelByIndex(i)
        if secel.type == "loop":
            structure.append('-')
        elif secel.type == "helix":
            structure.append('H')
        elif secel.type == "strand":
            structure.append('E')
    lines.append( ''.join(structure) )
    return '\n'.join(lines)

  def getSequence(self):
    startIndex = min(self.residueRange())
    return repr(self).lstrip('.')      

if __name__ == '__main__':
    mychain = Chain.load('1KPO.pdb')
    print mychain
