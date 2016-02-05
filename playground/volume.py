#!/usr/bin/env python

from Core import Volume

v = Volume(2,2,2,3.0)
v1 = Volume()

print v.getSize()
v.out()

print v1.getSize()
v1.out()

print v.cmp(v1)

i, j, k = 0, 1, 0

print v[i,j,k]

v[i,j,k] = 555.0

print v[i,j,k]

v.out()
