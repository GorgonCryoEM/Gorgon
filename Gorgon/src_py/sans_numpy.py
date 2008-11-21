#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author: Ross Coleman (racolema@bcm.edu)
from math import *

def scalarTimesVector(a, v):
    assert isinstance(a, (int, float) )
    assert len(v) > 1
    res = [None]* len(v)
    for i in xrange(len(v)):
        res[i] = a*v[i]
    return tuple(res)

def dotProduct(u, v):
    assert len(u) == len(v)
    res = 0
    for i in xrange(len(u)):
        res += u[i]*v[i]
    return res

def crossProduct(u, v):
    assert len(u) == len(v)  ==3
    return (u[1]*v[2]-u[2]*v[1], u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0])

def vectorAdd(u, v):
    assert len(u) == len(v)
    res = [None]*len(u)
    for i in xrange(len(u)):
        res[i] = u[i]+v[i]
    return tuple(res)

def vectorMagnitude(v):
    magSquared = 0
    for i in xrange(len(v)):
        magSquared += v[i]**2
    return sqrt(magSquared)

def normalizeVector(v):
    mag = vectorMagnitude(v)
    if mag == 0:
        print 'Vector magnitude is zero!'
        raise ValueError, v
        return
    else:
        res = [None]*len(v)
        for i in xrange(len(v)):
            res[i] = v[i]/mag
        return tuple(res)
