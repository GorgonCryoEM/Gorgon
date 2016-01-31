from math import *


def cross_product(a,b):
	"""Cross product of two 3-d vectors. from http://www-hep.colorado.edu/~fperez/python/python-c/weave_examples.html
	"""
	cross = [0]*3
	cross[0] = a[1]*b[2]-a[2]*b[1]
	cross[1] = a[2]*b[0]-a[0]*b[2]
	cross[2] = a[0]*b[1]-a[1]*b[0]
	return cross


def get_angle(patoms, origin, p1, p2):
	v1=(patoms[p1][0]-patoms[origin][0],patoms[p1][1]-patoms[origin][1],patoms[p1][2]-patoms[origin][2])
	lengthV1=sqrt(v1[0]**2+v1[1]**2+v1[2]**2)
	v1n=(v1[0]/lengthV1,v1[1]/lengthV1,v1[2]/lengthV1)
	v2=(patoms[p2][0]-patoms[origin][0],patoms[p2][1]-patoms[origin][1],patoms[p2][2]-patoms[origin][2])
	lengthV2=sqrt(v2[0]**2+v2[1]**2+v2[2]**2)
	v2n=(v2[0]/lengthV2,v2[1]/lengthV2,v2[2]/lengthV2)

	dp = v1n[0]*v2n[0] + v1n[1]*v2n[1] + v1n[2]*v2n[2] #dot product
	if dp > 1:
		dp=1
	if dp<-1:
		dp=-1
	angle=acos(dp)*(180/pi)
	if angle>90:
		angle=180-angle
	return angle

