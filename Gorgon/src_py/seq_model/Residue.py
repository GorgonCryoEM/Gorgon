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
    _aa_dict={
        'A':'ALA', 'C':'CYS', 'D':'ASP', 'E':'GLU', 'F':'PHE', 'G':'GLY', 'H':'HIS', 'I':'ILE', 
        'K':'LYS', 'L':'LEU', 'M':'MET', 'N':'ASN', 'P':'PRO', 'Q':'GLN', 'R':'ARG', 'S':'SER', 
        'T':'THR', 'V':'VAL', 'W':'TRP', 'X':'ANY', 'Y':'TYR',
        'ANY':'X', 'ALA':'A', 'CYS':'C', 'ASP':'D', 'GLU':'E', 'PHE':'F', 'GLY':'G', 'HIS':'H', 
        'ILE':'I', 'LYS':'K', 'LEU':'L', 'MET':'M', 'ASN':'N', 'PRO':'P', 'GLN':'Q', 'ARG':'R', 
        'SER':'S', 'THR':'T', 'VAL':'V', 'TYR':'Y', 'TRP':'W'
        }

    residueTypes = {
        'nonpolar': ('GLY', 'ALA', 'VAL', 'LEU', 'ILE', 'PHE', 'TRP', 'MET', 'PRO'), 
        'polarNoSulfur': ('ASN','GLN','TYR','SER','THR'), 
        'negative': ('ASP',  'GLU'), 
        'positive': ('HIS',  'LYS',  'ARG'), 
        'sulfur': ('MET','CYS'), 
        'greasy': ('VAL','ILE','LEU','PRO','GLY','ALA','TRP','PRO')
        }
    residueTypes['polar'] = residueTypes['polarNoSulfur'] + ('CYS', )
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
            try:
                self.symbol1=symbol
                self.symbol3=Residue._aa_dict[symbol]
            except KeyError:
                self.symbol1 = 'X'
                self.symbol3='ANY'                

        elif len(symbol)==3:
            try:
                self.symbol3=symbol
                self.symbol1=Residue._aa_dict[symbol]
            except KeyError:
                self.symbol1 = 'X'
                self.symbol3 = 'ANY'

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
            return False
    
    def addAtom(self, atomName, x, y, z, element="", serialNo=None, occupancy=None, tempFactor=None, charge="" ):
        '''Adds a new PDBAtom to the residue.'''
        residueIndex=self.chain.findIndexForRes(self)
        rawAtom=PDBAtom(self.chain.getPdbID(), self.chain.getChainID() , residueIndex, atomName)
        rawAtom.setPosition(Vector3DFloat(x,y,z))
        if not element:
            element = atomName[0]
        rawAtom.setElement(element)
        if serialNo != None:
            rawAtom.setSerial(serialNo)
        if occupancy != None:
            rawAtom.setOccupancy(occupancy)
        if tempFactor != None:
            rawAtom.setTempFactor(tempFactor)
        if charge:
            rawAtom.setCharge(charge)
        self.__atoms[atomName]=rawAtom
        #print "%s PDBAtom added to %s Residue"  %(atomName,self)
        '''
        #Add atom to viewer and update --Mike's addition
        #self.chain.getViewer().renderer.addAtom(rawAtom)
        '''
        '''
        if(not self.chain.getViewer().loaded):
            self.chain.getViewer().loaded = True
            self.chain.getViewer().emitModelLoaded()
        self.chain.getViewer().dirty = True
        self.chain.getViewer().emitModelChanged()
        '''
        return rawAtom

    def addAtomObject(self, atomObject):
        '''Adds an existing PDBAtom object to the residue.'''
        atomName = atomObject.getName()
        self.__atoms[atomName] = atomObject
    
    def clearAtom(self, atomName):
        '''Removes the PDBAtom specified by name from the residue'''
        self.__atoms.pop(atomName)
    def clearAtoms(self):
        '''Removes all the PDBAtoms from the residue'''
        self.__atoms={}
  
    '''
    #Mike's version
    def clearAtom(self, atomName):
    hashKey=self.getAtom(atomName).getHashKey()
    self.chain.getViewer().renderer.deleteAtom(hashKey)
    self.__atoms.pop(atomName)
  
    def clearAtoms(self):
        for atomName in self.getAtomNames():
            self.clearAtom(atomName)
    '''
  
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
