#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  GAtom
# Class Description: extends seq_model.Atom to account for Gorgon Viewer interface 
#                    More info in: seq_model-doc.txt
#

from libpyGORGON import PDBAtom,Vector3DFloat
from seq_model.Atom import Atom

#--------------------------------------
class GAtom(Atom):
  def __init__(self, element, x,y,z, viewer, residue, serialNo=None, occupancy=1.0, temp_factor=1.0):
    Atom.__init__(self, element, x,y,z, residue, serialNo, occupancy=1.0, temp_factor=1.0)
    self.viewer=viewer

    #create C++ PDBAtom object and attributes
    self.rawAtom=PDBAtom()
    self.rawAtom.setElement(element)
    self.rawAtom.setPosition(Vector3DFloat(x,y,z))
    self.rawAtom.setOccupancy(occupancy)
    self.rawAtom.setTempFactor(temp_factor)
    self.rawAtom.setCharge('')

    #Add atom to viewer and update
    self.viewer.renderer.addAtom(self.rawAtom)
    if(not self.viewer.loaded):
        self.viewer.loaded = True
        self.viewer.emitModelLoaded()
    self.viewer.dirty = True
    self.viewer.emitModelChanged()
