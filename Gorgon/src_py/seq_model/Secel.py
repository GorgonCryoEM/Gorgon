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
    self.chain=chain
    self.serialNo=serialNo
    self.label=label
    self.startIndex=startIndex
    self.stopIndex=stopIndex
    self.color=color
    if qtEnabled and color==None:
      self.color=QtGui.QColor(0,0,0)

  def __repr__(self):
    res = ''
    if self.label[0] == 'H':
        res = 'Helix#'
    elif self.label[0] == 'S':
        res = 'Strand#'
    elif self.label[0] == 'C':
        res = 'Coil#'
    res = res + str(self.serialNo) + ':' + str(self.startIndex) + '-' + str(self.stopIndex)
    return res

  def setColor(self, newColor):
    self.color=newColor

  def getColor(self):
    return self.color

  def __str__(self):
    s = self.type
    s = s + '('
    s = s + str(self.startIndex)
    s = s + ','
    s = s + str(self.stopIndex)
    s = s + ')'
    s = s + ':'
    residues = self.chain.residueList[self.startIndex-1:self.stopIndex-1]

    for residue in residues:
      s = s + residue.symbol1
    return s
