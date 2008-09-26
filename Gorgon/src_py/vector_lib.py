# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A utility module for vector manipulation 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.4  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from cmath import *   
    
def vectorDistance(v1, v2):
    return abs(sqrt(pow(v1[0]-v2[0], 2) + pow(v1[1]-v2[1], 2) + pow(v1[2]-v2[2], 2)))
        
def vectorSize(v):        
    return abs(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]))
        
def vectorNormalize(v):
    base = abs(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]))
    if(base == 0) :
        return v
    else :
        return [float(v[0]/base), float(v[1]/base), float(v[2]/base)]
    
def vectorCrossProduct(v1, v2):
    return [v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]]

def vectorAdd(v1, v2):
    return [v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]]

def vectorSubtract(v1, v2):
    return [v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]]

def vectorScalarMultiply(s, v):
    return [s*v[0], s*v[1], s*v[2]]
