#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Residue
# Class Description: Class that models amino acid residues.  Residue objects aggregate Atom objects.
#                    More info in: seq_model-doc.txt
#

from libpyGORGON import PDBAtom,Vector3DFloat

class Residue:
  '''Residue objects have one-letter and three-letter abbreviations and contain PDBAtoms'''
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
  _aa_dict['W']='TRP'
  _aa_dict['X']='ANY'
  _aa_dict['Y']='TYR'

  _aa_dict['ANY']='X'
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
  
  residueTypes = {
        'nonpolar': ('GLY', 'ALA', 'VAL', 'LEU', 'ILE', 'PHE', 'TRP', 'MET', 'PRO'), 
        'polar': ('ASN','GLN','TYR','SER','THR'), 
        'negative': ('ASP',  'GLU'), 
        'positive': ('HIS',  'LYS',  'ARG'), 
        'sulfur': ('MET','CYS'), 
        'greasy': ('VAL','ILE','LEU','PRO','GLY','ALA','TRP','PRO')
        }
  residueTypes['neutral'] = residueTypes['nonpolar'] + residueTypes['polar']
  residueTypes['charged'] = residueTypes['negative'] + residueTypes['positive']
  
  size = {
                'GLY':1, 'PRO':1,  
                'ALA':2,'VAL':2,'THR':2,'CYS':2,  'SER':2, 
                'MET':3,'LEU':3,'ILE':3,'ASN':3,'GLN':3,'ASP':3,'GLU':3,
                'TRP':4,'HIS':4,'TYR':4,'PHE':4,'ARG':4,'LYS':4
            }
  
  def __init__(self, symbol, chain=None):
    symbol=symbol.upper()
    if len(symbol)==1:
      self.symbol1=symbol
      self.symbol3=Residue._aa_dict[symbol]

    elif len(symbol)==3:
      self.symbol3=symbol
      self.symbol1=Residue._aa_dict[symbol]

    else:
      raise ValueError, "Residue must be instantiated with either a 1-letter or 3-letter symbol" 

    self.__atoms={}
    self.chain=chain



  def getAtomNames(self):
    '''Returns the names of the residue's atoms'''
    return self.__atoms.keys()


  def getAtom(self, atomName):
    '''Returns the residue's PDBAtom given an atom name such as CA'''
    try:
        return self.__atoms[atomName]
    except KeyError:
        print "Residue.getAtom()--no %s atom." % atomName, 
    


  def addAtom(self, atomName, x, y, z, element=None, serialNo=None, occupancy=None, tempFactor=None ):
    '''Adds a PDBAtom to the residue.'''
    residueIndex=self.chain.findIndexForRes(self)
    rawAtom=PDBAtom(self.chain.getPdbID(), self.chain.getChainID() , residueIndex, atomName)
    rawAtom.setPosition(Vector3DFloat(x,y,z))
    rawAtom.setElement(element)
    #rawAtom.setOccupancy(occupancy)
    #rawAtom.setTempFactor(temp_factor)
    #rawAtom.setCharge('')
    self.__atoms[atomName]=rawAtom
    #print "%s PDBAtom added to %s Residue"  %(atomName,self)

    #Add atom to viewer and update
    #self.chain.getViewer().renderer.addAtom(rawAtom)
    '''
    if(not self.chain.getViewer().loaded):
        self.chain.getViewer().loaded = True
        self.chain.getViewer().emitModelLoaded()
    self.chain.getViewer().dirty = True
    self.chain.getViewer().emitModelChanged()
    '''
    return rawAtom

  def addAtomObject(self, atomObject):
      atomName = atomObject.getName()
      self.__atoms[atomName] = atomObject

  def clearAtoms(self):
    '''Removes all the PDBAtoms from the residue'''
    self.__atoms={}


  def __repr__(self):
    return self.symbol1
  
  def setCAlphaColorToDefault(self):
    ''''
    Sets the residue's C-alpha atom to the default color.  This is useful if the color has been changed somewhere.
    '''
    try:
        temp = PDBAtom('____',  '9', 1, 'CA')
        defaultColor = ( temp.getColorR(),  temp.getColorG(),  temp.getColorB(),  temp.getColorA() )
        del temp
        atom = self.getAtom('CA')
        atom.setColor(*defaultColor)
    except:
        pass
  
  def setCAlphaSizeToDefault(self):
    ''''
    Sets the residue's C-alpha atom to the default size.  This is useful if the size has been changed somewhere.
    '''
    try:
        temp = PDBAtom('____',  '9', 1, 'CA')
        defaultSize = temp.getAtomRadius()
        del temp
        atom = self.getAtom('CA')
        atom.setAtomRadius(defaultSize)
    except:
        pass
