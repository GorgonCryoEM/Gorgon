#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  Atom
# Class Description: Class that models atoms with atomic coordinates.
#                    More info in: seq_model-doc.txt
#

class Atom:
  def __init__(self,element,x,y,z, occupancy=1.0,temp_factor=1.0):
		self.element=element
		self.x=x
		self.y=y
		self.z=z
		self.occupancy=occupancy
		self.temp_factor=temp_factor
		print x,y,z
