#!/usr/bin/python


import unittest
import urllib
import os

import random
from libpyGORGON import PDBAtom,Vector3DFloat
from seq_model.Chain import Chain


class SeqModelTests(unittest.TestCase):

  DELETE_PDB_FILES=False

  def setUp(self):
    if not os.path.exists('groel.pdb'):
      urllib.urlretrieve('http://www.rcsb.org/pdb/download/downloadFile.do?fileFormat=pdb&compression=NO&structureId=1JON','groel.pdb')
    #if not os.path.exists('1IRK.pdb'):  
        #urllib.urlretrieve('http://www.rcsb.org/pdb/download/downloadFile.do?fileFormat=pdb&compression=NO&structureId=1IRK', '1IRK.pdb')
    #if not os.path.exists('3B8E.pdb'):
        #urllib.urlretrieve('http://www.rcsb.org/pdb/download/downloadFile.do?fileFormat=pdb&compression=NO&structureId=3B8E',  '3B8E.pdb')
    if not os.path.exists('1KPO.pdb'):
        urllib.urlretrieve('http://www.rcsb.org/pdb/download/downloadFile.do?fileFormat=pdb&compression=NO&structureId=1KPO', '1KPO.pdb')
  def tearDown(self):
    if self.DELETE_PDB_FILES:
      os.remove('groel.pdb')
      os.remove('groel-out.pdb')

  def test01_ChainInitializers(self):
    chain=Chain('ACDEFGH')
    self.assertEquals(chain.__repr__(), 'ACDEFGH')

    chain=Chain('')
    self.assertEquals(chain.__repr__(),'')

    chain[5]='M'
    self.assertEquals(chain.__repr__(), '...M')

    chain=Chain('MIKE')
    chain[6]='M'
    for res in ['A','R','S','H']:
      chain.append(res)
    self.assertEquals(chain.__repr__(), 'MIKE...MARSH')


  def test02_ChainAccessors(self):
    chain=Chain('ACDEFGH')
    self.assertEquals(chain.__repr__(), 'ACDEFGH')
    self.assertEqual(chain[1].symbol1, 'A')
    self.assertEqual(chain[1].symbol3, 'ALA')
    self.assertEqual(chain[1].__repr__(), 'A')
    self.assertEqual(chain[7].symbol1, 'H')
    self.assertEqual(chain[7].symbol3, 'HIS')
    self.assertEqual(chain[7].__repr__(), 'H')
    self.assertEqual(chain[-1].__repr__(), 'H')
    self.assertRaises(KeyError, chain.__getitem__, 0)
    self.assertRaises(KeyError, chain.__getitem__, 8)

    chain=Chain('')
    chain[5]='N'
    chain[-1]='M'
    chain.append('I')
    self.assertEquals(chain[6].__repr__(), 'I')

  def test03_ChainSlicing(self):
    chain=Chain('MARSHANDMILMAN')

    mike=chain[1:5]
    self.assertEquals('MARSH',mike.__repr__())

    conjunction=chain[6:8]
    self.assertEquals('...AND',conjunction.__repr__())

    rebecca=chain[-6:-1]
    self.assertEquals('...MILMAN',rebecca.__repr__())


  def __assertHelix(self, chain, serialNo, helixID, startResName, startChainID, startIndex, stopResName, stopChainID, stopIndex):
    self.assert_(chain.helices.has_key(helixID))

    self.assertEquals(chain.helices[helixID].startIndex, startIndex)
    self.assertEquals(chain[startIndex].symbol3, startResName)
    
    self.assertEquals(chain.helices[helixID].stopIndex, stopIndex)
    self.assertEquals(chain[stopIndex].symbol3, stopResName)

    self.assertEquals( str(chain.helices[helixID]).split(':')[1], chain[startIndex:stopIndex].__repr__().strip('...'))



  def __assertStrand(self, chain, strandNo, sheetID, nStrands, startResName, startChainID, startIndex, stopResName, stopChainID, stopIndex):
    self.assert_(chain.sheets.has_key(sheetID))
    
    self.assertEquals(chain.sheets[sheetID].strandList[strandNo].startIndex, startIndex)
    self.assertEquals(chain[startIndex].symbol3, startResName)

    self.assertEquals(chain.sheets[sheetID].strandList[strandNo].stopIndex, stopIndex)
    self.assertEquals(chain[stopIndex].symbol3, stopResName)
    
    self.assertEquals( str(chain.sheets[sheetID].strandList[strandNo]).split(':')[1], chain[startIndex:stopIndex].__repr__().strip('...'))


  def __validateSecels(self, chain):
    self.__assertHelix(chain, 1, 1, 'PRO', 'A', 202, 'PHE', 'A', 204)
    self.__assertHelix(chain, 2, 2, 'ILE', 'A', 230, 'ALA', 'A', 243)
    self.__assertHelix(chain, 3, 3, 'GLY', 'A', 256, 'ARG', 'A', 268)
    self.__assertHelix(chain, 4, 4, 'GLY', 'A', 282, 'THR', 'A', 296)
    self.__assertHelix(chain, 5, 5, 'LEU', 'A', 309, 'LYS', 'A', 311)
    self.__assertHelix(chain, 6, 6, 'LEU', 'A', 314, 'ASP', 'A', 316)
    
    self.__assertStrand(chain, 1, 'A', 4, 'MET', 'A', 193 ,'PHE', 'A', 195)
    self.__assertStrand(chain, 2, 'A', 4, 'THR', 'A', 330 ,'ILE', 'A', 333)
    self.__assertStrand(chain, 3, 'A', 4, 'ARG', 'A', 322 ,'ILE', 'A', 325)
    self.__assertStrand(chain, 4, 'A', 4, 'VAL', 'A', 213 ,'GLU', 'A', 216)
    self.__assertStrand(chain, 1, 'B', 3, 'PHE', 'A', 219 ,'ALA', 'A', 223)
    self.__assertStrand(chain, 2, 'B', 3, 'LEU', 'A', 247 ,'ALA', 'A', 251)
    self.__assertStrand(chain, 3, 'B', 3, 'VAL', 'A', 273 ,'LYS', 'A', 277)


  def test04_PDB_IO(self):
    chain=Chain.load('groel.pdb')
    chain.saveToPDB('groel-out.pdb')
    chain2=Chain.load('groel-out.pdb')
    
    atoms = {}
    atoms2 = {}
    
    for resIndex in chain.residueRange():
        for atomName in chain[resIndex].getAtomNames():
            atom = chain[resIndex].getAtom(atomName)
            key = ( atom.getPDBId(), atom.getChainId(), atom.getResSeq(), atom.getName())
            atoms[key] = atom

    for resIndex in chain2.residueRange():
        for atomName in chain2[resIndex].getAtomNames():
            atom = chain2[resIndex].getAtom(atomName)
            key = ( atom.getPDBId(), atom.getChainId(), atom.getResSeq(), atom.getName() )
            atoms2[key] = atom

    #Validate atom records
    for key in atoms.keys():
        self.assertTrue (key in atoms2.keys())

    #Validate residue records
    for index in chain.residueRange():
        residue1=chain[index]
        residue2=chain2[index]
        self.assertEquals( chain[index].symbol1, chain2[index].symbol1)
        atomKeys1=residue1.getAtomNames()
        for atomLabel in atomKeys1:
            self.assertAlmostEqual(residue1.getAtom(atomLabel).getPosition().x(), residue2.getAtom(atomLabel).getPosition().x(),3)
            self.assertAlmostEqual(residue1.getAtom(atomLabel).getPosition().y(), residue2.getAtom(atomLabel).getPosition().y(),3)
            self.assertAlmostEqual(residue1.getAtom(atomLabel).getPosition().z(), residue2.getAtom(atomLabel).getPosition().z(),3)

    #Validate secel records
    self.__validateSecels(chain)



  def test05_MultiChainPDB(self):
    filename = '1KPO.pdb'
    mychain = Chain.load(filename) #gets the first one
    mychain_origKey = mychain.getIDs()
    mychain.setIDs('mypdb', 'A')
    self.assertEqual( mychain.getIDs(), ('mypdb', 'A') )
    Chain.loadAllChains(filename)
    chainIDset = set([ x[1] for x in Chain.getChainKeys() ])
    chainIDsetPDB = set(Chain.getChainIDsFromPDB(filename))
    self.assert_( chainIDsetPDB.issubset(chainIDset) ) #Check for subset because we have chain objects from previous tests that are not in a PDB file
    loadAllChains_mychain = Chain.getChain( mychain_origKey ) #the original key is the one given by the PDB file
    self.assertEqual( loadAllChains_mychain.__repr__(), mychain.__repr__() )
    ####Ross's question: why are chain objects from other test??_* functions in Chain.getChainIDs()?
    ####Aren't they deleted after each function is tested?
    
  def test06_PDBAtoms(self):
    my_chain = Chain("ACEFGHIKLACEFGHIKLMNPYVWQPMIKESMATTHEW")
    '''
    my_chain.secelList.append (Coil (my_chain,'l0',1,9))
    my_chain.secelList.append (Helix(my_chain,'h1',10,20))
    my_chain.secelList.append (Coil (my_chain,'l2',21,25))
    my_chain.secelList.append (Helix(my_chain,'h3',26,30))
    my_chain.secelList.append (Coil (my_chain,'l4',31,38))
    '''


    # Instantiate sub_chain
    sub_chain=my_chain[14:20]
    'sub_chain=%s' %sub_chain


    # Add coordinate records to my_chain
    #for residue in my_chain[3:-5]:
    for residue in my_chain:
      x=random.gauss(20.0,5.0)
      y=random.gauss(20.0,5.0)
      z=random.gauss(20.0,5.0)
      residue.addAtom('CA', x,y,z, 'C')


    # Test toPDB on both chains
    '\nmy_chain.PDB\n' + my_chain.toPDB(backboneOnly=True, verbose=False)
    '\nsub_chain.PDB\n' + sub_chain.toPDB(backboneOnly=True, verbose=False)

#TODO: Add a test for input of *.seq files
