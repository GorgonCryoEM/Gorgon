#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Secel
# Class Description: Superclass that models secondary structure elements.  Subclassed by Helix, Strand, Coil
#                    More info in: seq_model-doc.txt
#

try:
  from PyQt4 import QtGui
  qtEnabled=True
except:
  qtEnabled=False

        
class Secel(object):
  def __init__(self, chain, serialNo, label, startIndex, stopIndex, color=None):
    self.chain=chain    #Actual chain instance object
    self.serialNo=serialNo
    self.label=label
    self.startIndex=startIndex  #The number (there is no zero residue) of the first residue of that secondary structure element
    self.stopIndex=stopIndex    #The number of the last residue of that secondary structure element on the chain
    self.color=color
    if qtEnabled and color==None:
        self.color=QtGui.QColor(0,0,0)

  def __repr__(self):
    return self.__str__()

  def setColor(self, newColor):
    self.color=newColor

  def getColor(self):
    return self.color

  def __str__(self):
    s = self.type + '(' + str(self.startIndex) + ',' + str(self.stopIndex) + ')' + ':'
    residues = self.chain[self.startIndex:self.stopIndex]
    residues = residues.__repr__().split('...')
    noPeriods = []
    for item in residues:
        if item == '': 
            continue
        noPeriods.append(item)
    residues = ''.join(noPeriods)
    return s + residues
