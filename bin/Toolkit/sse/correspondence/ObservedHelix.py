import math
from Explorer.libs import Vec3

class ObservedHelix:

    def __init__(self, label, beginningCoord, endCoord ):
        """
Note that the label is currently an integer, not a string.
        """
        self.beginningCoord = Vec3(beginningCoord)
        self.endCoord = Vec3(endCoord)
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
        return (self.beginningCoord+self.endCoord)*0.5

    def getUnitVector(self):
        '''
Returns a vector along the helical axis pointing toward the endpoint 
farther from the origin.
        '''
        length = self.getLength()
        return (self.beginningCoord-self.endCoord)/length

    def getLength(self):
        """
This returns the length of the observed helix in angstroms (coordinates
should be in angstroms).
        """
        return pow( (self.beginningCoord[0] - self.endCoord[0])**2 + (self.beginningCoord[1] - self.endCoord[1])**2 + (self.beginningCoord[2] - self.endCoord[2])**2, 0.5)
