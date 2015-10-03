#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author: Mike Marsh (michael.marsh@bcm.edu)
# Class: ObservedHelix
# Class Description: Class that denotes a helix found by SSEHunter/Builder or any other routine that
#                    can find helices in EM maps.
#                    More info in: correspondence-doc.txt
#
import math

class ObservedHelix:
    def __init__(self, label, beginningCoord, endCoord ):
        """
Note that the label is currently an integer, not a string.
        """
        self.beginningCoord = beginningCoord
        self.endCoord = endCoord
        self.label = label #Note: currently an integer, not a string
        self.sseType = 'helix'

    def __parseHelix(self, inputString):
        """
This is not yet implemented.
        """
        return ObservedHelix('a', (1.0, 2.0, 3.0),(4.0, 5.0,6.0) )
        
    def getMidpoint(self):
        '''
Returns the midpoint of the two ends of the helical axis.
        '''
        return ( 0.5*(self.beginningCoord[0]+self.endCoord[0]), 0.5*(self.beginningCoord[1]+self.endCoord[1]), 0.5*(self.beginningCoord[2]+self.endCoord[2]) )

    def getUnitVector(self):
        '''
Returns a vector along the helical axis pointing toward the endpoint 
farther from the origin.
        '''
        length = self.getLength()
        return ( (self.beginningCoord[0]-self.endCoord[0])/length, (self.beginningCoord[1]-self.endCoord[1])/length, (self.beginningCoord[2]-self.endCoord[2])/length )

    def getLength(self):
        """
This returns the length of the observed helix in angstroms (coordinates
should be in angstroms).
        """
        return pow( (self.beginningCoord[0] - self.endCoord[0])**2 + (self.beginningCoord[1] - self.endCoord[1])**2 + (self.beginningCoord[2] - self.endCoord[2])**2, 0.5) 
