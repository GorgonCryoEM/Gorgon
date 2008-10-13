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
    typeOfSecel = str(type(self)).split('.')[-1]
    typeOfSecel = typeOfSecel.split("'")[0]
    return "%s(chain=Chain.getChain(%s), serialNo=%s, label=%s, startIndex=%i, stopIndex=%i)" % (typeOfSecel,  
                                self.chain.getIDs(),  self.serialNo,  self.label,  self.startIndex, self.stopIndex)

  def setColor(self, newColor):
    self.color=newColor

  def getColor(self):
    return self.color

  def __str__(self):
    residues = str(self.chain[self.startIndex:self.stopIndex]).strip('...')
    return "%s(%i,%i):%s" % (self.type, self.startIndex, self.stopIndex, residues)
