#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author: Mike Marsh (michael.marsh@bcm.edu)
# Class: ObservedHelix
# Class Description: Class that denotes a helix found by SSEHunter/Builder or any other routine that
#                    can find helices in EM maps.
#                    More info in: correspondence-doc.txt
#

class ObservedHelix:
  def __init__(self, label, x1, y1, z1, x2, y2, z2 ):

    # lesser is the vertex closer to 0,0,0
    if (x1*x1 + y1*y1 +z1*z2) <  (x2*x2 + y2*y2 + z2*z2):
      lesser =(x1,y1,z1)
      greater=(x2,y2,z2)
    else:
      lesser=(x2,y2,z2)
      greater=(x1,y1,z1)

    self.label = label
    
    self.x0=lesser[0]
    self.y0=lesser[1]
    self.z0=lesser[2]

    self.x1=greater[0]
    self.y1=greater[1]
    self.z1=greater[2]


  def __parseHelix(self, inputString):
    return ObservedHelix('a', 1.0, 2.0, 3.0,4.0, 5.0,6.0 )
