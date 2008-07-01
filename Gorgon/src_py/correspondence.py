#!/usr/bin/python

#structs.py
#Gorgon Python Data Structures for Correspondence, CorrespondenceLibrary, SSEBuilderResults
#
#
#
# The cryoEM map is analyzed for helices and sheets/strands.  Those found are stored as a
#   StructureObservation
#
# The sequence is analyzed for helices and sheets.  Those predicted are stored as a
#   StructurePrediction
#
# A Match is a 1:1 pairing of a predicted helix/sheet with an observed helix/sheet.
#
# A Correspondence is a complete solution for matching up predicted helices/sheets
#   with observed helices/sheets.
#
# A CorrespondenceLibrary is a set of complete solutions and the parameters used to 
#   compute them.
#

from seq_model import Chain,Helix,Strand,Coil

# None --> 'a'
# 'a' --> 'b'
# 'b' --> 'c'
# 'c' --> 'd'
# ...
# 'z' --> 'aa'
# 'aa' --> 'bb'
# 'bb' --> 'cc'
# ...
# 'zz' --> 'aa'
def nextLabel(prevLabel=None):
    if prevLabel is None:
      return 'a'
    else:
      nChars=len(prevLabel)
      if prevLabel[0]=='z':
	nChars=nChars+1
	prevLabel=chr(ord(prevLabel[0])-26)
      char=chr(ord(prevLabel[0])+1)
      return nChars*char

#--------------------------------------
class StructureObservation:  #SSEBuilderResults
  def __init__(self, filename):
    '''
    Parameters with *** attribute need to be recorded with the CorrespondenceLibrary
      1. SSEHunter
         Input parameters:
     ***   apix
     ***   resolution of map
     ***   isosurface threshold
         Input files:
           cryo-em map
      
         a. Cross-correlation computed     (3 output files)
              coefficients (.mrc)
              alt          (.mrc)
              az           (.mrc)
      
         b. Skeleton computed              (4 output files)
              alpha-only skeleton (.mrc)
              beta-only skeleton  (.mrc)
     ***      composite skeleton  (.mrc)
              skeleton score      (.pdb)
      
         c. Psuedo-atoms computed          (2 output files)
              unscored psuedo-atoms (.pdb)
              scored psuedo-atoms   (.pdb)
      
      2. SSEBuilder or Autocluster         (output files)
     *** helix-vrml           (.wrl)
     *** sheet-vrml           (.wrl)
     *** coil-vrml            (.wrl)
         helices              (.pdb)
         sse                  (.dejavu)  #this file is redundant with previous 2
     *** labeled psuedo-atoms (.pdb)
    '''
      
    self.params = {}
    self.params['aPix']=0.0
    self.params['mapRes']=0.0
    self.params['isosurfaceThreshold']=0.0
    self.params['emanVersion']=0.0

    self.files={}
    self.files['input_map']=None
    self.files['skeletonComposite']=None
    self.files['sseVRML']=None
    self.files['sseDejavu']=None
    self.files['psuedoAtomsScored']=None

    self.comments = None

    self.helixDict = {}  #keys are nextLabel(prevLabel)
    self.sheetDict = []


     
#--------------------------------------
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
    
    self.startX=lesser[0]
    self.startY=lesser[1]
    self.startZ=lesser[2]

    self.stopX=greater[0]
    self.stopY=greater[1]
    self.stopZ=greater[2]


#--------------------------------------
class ObservedSheet:
  pass


#--------------------------------------
class ObservedStrand:
  pass


#--------------------------------------
class StructurePrediction:  #results of secondary-structure prediction
  def __init__(self, filename):
    secelIndex=0
    self.secelDict={}
    self.params=None
    self.comments=None

    lines=open(filename).readlines()
    chain=Chain(str.strip(lines[0]))

    # loop pre-conditions
    # i is the next char in the predictionStr
    predictionsStr=lines[1]
    current='-'
    start=1
    helixCount=0
    strandCount=0
    coilCount=0
    for i,index in zip(predictionsStr,range(1,len(predictionsStr)+1)):

      prev=current
      current=i

      if prev != current:
	stop=index-1
	print '%s: %s-%s' %(prev,start,stop)

	if prev=='H':
	  helixCount=helixCount+1
	  label='H' + str(helixCount)
	  secel = Helix(chain,label,start,stop)
	  #chain.secel_list.append(secel)

	elif prev=='E':
	  strandCount=strandCount+1
	  label='E' + str(strandCount)
	  secel = Strand(chain,label,start,stop)
	  #chain.secel_list.append(secel)

	elif prev=='-':
	  coilCount=coilCount+1
	  label='C' + str(coilCount)
	  secel = Coil(chain,label,start,stop)
	  #chain.secel_list.append(secel)

	else:
	  errString="got %s when rexpecting 'H','E', or '-'" %prev
	  raise ValueError(errString)
	start = index
	self.secelDict[secelIndex]=secel
	secelIndex = secelIndex+1
	
    print chain.to_pdb()


#--------------------------------------
FORWARD=0
REVERSE=1
class Match:
  def __init__(self, observed, predicted, direction):
    # A forward match aligns the N-terminus with lesser vertex, and C-terminus with greater vertex
    #    lesser vertex is vertex closer to 0,0,0
    self.observed=observed
    self.predictred=predicted
    self.direction=direction


#--------------------------------------
class Correspondence:
  def __init__(self, library, matchList, score, comments=None, chain=None):
    self.matchList=[]
    self.score=0.0
    self.libary=library
    self.comments=None
    self.chain=None

  def __repr__(self):
    s=''
    # print index, score, matchList
    #    matchList is ordered by StructurePrediction
    #    b,A,cCC
    #    how to tell difference between cCCD and cC,C,D cCC,D
    if chain:
      s=s+chain
    return s


#--------------------------------------
class CorrespondenceLibrary:
  def __init__(self, sp=None, so=None, algo=None, params=None, correspondenceList=None, comments=None, filename=None):
    if (filename):
      load(filename)
    else:
      self.structurePrediction=sp
      self.structureObservation=sp
      self.algorithm=algo
      self.params=params
      self.correspondenceList=corresondenceList
      self.comments=comments
      # sort correspondences by score

  def save(filename):
    # write observed & SSE parameters
    # write StructurePrediction & params
    # write algorithm, params, comments
    # write correspondence list by calling Correspondence.__repr__
    pass
  
  def load(filename):
    pass
  


#--------------------------------------
def main(argv):
  filename='/Users/mikemarsh/for-baker/2008-03-24/1qgt.seq'
  structurePrediction=StructurePrediction(filename)

 
  so=StructureObservation('filename')
  prevLabel=None
  for i in range(53):
    nextLabel_= nextLabel(prevLabel)
    prevLabel=nextLabel_
    print i,nextLabel_

  '''
  corr1=Correspondence()
  corr2=Correspondence()
  corr3=Correspondence()

  corrLibrary=CorrespondenceLibrary()
  corrLibrary.save()
  corrLibrary.load()
  '''


if __name__=="__main__":
  from sys import argv
  main(argv)


