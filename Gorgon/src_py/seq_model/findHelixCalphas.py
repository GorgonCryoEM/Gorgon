#!/usr/bin/env python
###	based on dejavu2pdb by Matthew Baker 06/2003

import math

def cross_product(a,b):
	"""Cross product of two 3-d vectors. from http://www-hep.colorado.edu/~fperez/python/python-c/weave_examples.html
	"""
	cross = [0]*3
	cross[0] = a[1]*b[2]-a[2]*b[1]
	cross[1] = a[2]*b[0]-a[0]*b[2]
	cross[2] = a[0]*b[1]-a[1]*b[0]
	return cross

def dot_product(a,b):
	if len(a) != len(b): return
	length = len(a)
	total = 0
	for i in range(length):
		total += a[i]*b[i]
	return total

def normalize(a):
	try:
		r2 = math.sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])
		return [a[0]/r2, a[1]/r2, a[2]/r2]
	except:
		return a

def helixEndpointsToCAlphaPositions(position1, position2, apix=1):
    """
This takes coordinates for two points that define the endpoints of a 
helical axis and calculates the coordinates of all the C-alpha atoms in
the helix.  It returns that list of coordinates for C-allpha atoms. 
This function may also take the angstroms per pixel ratio argument.
    """
    ######calulates phi and theta for each helix
    x1f = position1[0]
    y1f = position1[1]
    z1f = position1[2]

    x2f = position2[0]
    y2f = position2[1]
    z2f = position2[2]
    
    dx=x2f - x1f
    dy=y2f - y1f
    dz=z2f - z1f
    
    length=math.sqrt(dx*dx + dy*dy + dz*dz)
    intlength=int( math.ceil(length/1.5) )
        
    xyplane=math.sqrt((dx*dx)+(dy*dy))

    print "\nhelix:"
    if dy >= 0:
        phi=math.atan2(dy,dx) + math.pi/2.0
    else:
        phi = math.atan2(dy,dx) + math.pi/2.0
    new_dy = -dx * math.sin(phi) + dy * math.cos(phi)

    if new_dy <= 0: 
        theta=math.atan2(xyplane,dz)
    else:
        theta=math.atan2(xyplane,dz) + math.pi/2.0
    print dx,dy,dz
    print "phi   : %f"%phi
    print "theta : %f"%theta
    print "length: %d\n"%intlength
    
    #hlxorig = [0, 0, 1.0]
    hlxorig = [0, 1, 0.0]
    hlxvec= normalize([dx,dy,dz])
    nvect = normalize(cross_product(hlxorig, hlxvec))
    theta2cos = dot_product(hlxorig, hlxvec)
    print theta2cos
    if theta2cos>1.0: theta2cos = 1.0
    elif theta2cos<-1.0: theta2cos = -1.0
    theta2 = math.acos(theta2cos)
    print theta2
    helixSpin = (nvect[0], nvect[1], nvect[2], theta2)
    
    xTrans=(x1f+x2f)/2
    yTrans=(y1f+y2f)/2
    zTrans=(z1f+z2f)/2
    
    rot=[phi,theta]
    mx=[0,0,0,0,0,0,0,0,0]
    mx[0]= math.cos(phi)
    mx[1]= math.sin(phi) 
    mx[2]= 0
    
    mx[3]= -math.cos(theta) * math.sin(phi)
    mx[4]= math.cos(theta) * math.cos(phi)
    mx[5]= math.sin(theta)	

    mx[6]= math.sin(theta) * math.sin(phi)
    mx[7]= -math.sin(theta) * math.cos(phi)
    mx[8]= math.cos(theta)
    
    coordList = []
    ######transforms and writes out each helix as a list of coordinates

    for n in range(intlength):
        CAxorigin=math.cos((35+100*n)*math.pi/180)*2.2629
        CAyorigin=math.sin((35+100*n)*math.pi/180)*2.2629
        CAzorigin=(n*14.0/9.0)+.87
        
        x5=(mx[0]*CAxorigin+mx[3]*CAyorigin+mx[6]*CAzorigin)
        y5=(mx[1]*CAxorigin+mx[4]*CAyorigin+mx[7]*CAzorigin)
        z5=(mx[2]*CAxorigin+mx[5]*CAyorigin+mx[8]*CAzorigin)
        CAxcoord=(float(x1f)+x5)/apix
        CAycoord=(float(y1f)+y5)/apix
        CAzcoord=(float(z1f)+z5)/apix
        
        coordList.append( (CAxcoord, CAycoord, CAzcoord) )
        
    return coordList
