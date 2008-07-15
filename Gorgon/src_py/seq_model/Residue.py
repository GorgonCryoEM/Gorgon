#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Residue
# Class Description: Class that models amino acid residues.  Resiudes aggregate Atom objects.
#                    More info in: seq_model-doc.txt
#

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
