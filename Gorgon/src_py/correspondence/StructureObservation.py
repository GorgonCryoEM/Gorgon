#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class:  StructureObservation
# Class Description: Class that represents all of the secels found in an EM map.
#                    More info in: correspondence-doc.txt
#

from ObservedHelix import ObservedHelix

class StructureObservation:  #SSEBuilderResults
  def __init__(self, params=None, files=None, comments=None, helixDict=None, sheetDict=None, strandDict=None, filename=None):
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
         sheet-vrml           (.wrl)
         coil-vrml            (.wrl)
         helices              (.pdb)
     *** dejavu               (.sse)  #this file is redundant with previous 2
     *** labeled psuedo-atoms (.pdb)
    '''
    if (filename):
      self.load(filename)
    else:
      self.params = params
      self.files = files
      self.otherAttribs = {} 
      self.otherAttribs['comments']=comments
      self.helixDict = helixDict
      self.sheetDict = sheetDict
      #self.strandDict = strandDict
      #self.__loadHelices()
      #self.__loadSheets()
      #self.__loadStrands()


  def __loadHelices(self):
    infile=open(self.files['sseDejavu'])
    lines=infile.readlines()
    infile.close()

    for line in lines:
      split=line.split()
      if split[0]=="ALPHA":
        label=split[1].split("'")[1]
        x0=float(split[5])
        y0=float(split[6])
        z0=float(split[7])
        x1=float(split[8])
        y1=float(split[9])
        z1=float(split[10])
        self.helixDict[label]=ObservedHelix(label, (x0, y0, z0), (x1, y1, z1))

  def __loadSheets(self):
    pass

  def __loadStrands(self):
    pass

  def __addAttrib(self, dictName, key, value):
    if dictName in ('params','files'):
      try:
        getattr(self, dictName)[key]=value
      except AttributeError:
        print "%s is not a valid dictionary for StructureObseration objects" %dictName

    #elif dictName == ('helixDict','strandDict','sheetDict'):
      #getattr(self, dictName).__init__(key,value)

  def load(self, filename):
    self.params={}
    self.files={}
    self.sheetDict={}
    self.strandDict={}
    self.helixDict={}
    self.otherAttribs={}

    infile=open(filename)
    lines=infile.readlines()

    for line in lines:

      # Attributes with arbitrary key values.  Includes 'comments'
      if line.rfind('\t') < 1 and line.find('=') > 0 :
        key=line.strip().split('=')[0]
        value=line.strip().split('=')[1]
        self.otherAttribs[key]=value

      # Nested Dictionary Items those found in dictionaries 'files' and 'params'
      else:
        # last character in line is ':'
        if line.find(':') == len(line.strip()) :
          dictName=line.strip().split(':')[0]

        else:
          # line holds key=value pair
          if line.find('=') > 0:
            split=line.strip().split('=')
            key=split[0]
            value=split[1]
            self.__addAttrib (dictName, key, value)


  def save(self, filename):
    outfile=open(filename,'w')
    outfile.write("\nStructureObservation\n")

    outfile.write("\tparams:\n")
    for key in self.params:
      value=self.params[key]
      outfile.write("\t\t%s=%s\n" %(key,value))
     
    outfile.write("\tfiles:\n")
    for key in self.files:
      value=self.files[key]
      outfile.write("\t\t%s=%s\n" %(key,value))
     
    for key in self.otherAttribs:
      value=self.otherAttribs[key]
      outfile.write("\t%s=%s\n" %(key,value))
     
    outfile.write("\thelixDict:\n")
    for label in self.helixDict:
      x0=self.helixDict[label].beginningCoord[0]
      y0=self.helixDict[label].beginningCoord[1]
      z0=self.helixDict[label].beginningCoord[2]
      x1=self.helixDict[label].endCoord[0]
      y1=self.helixDict[label].endCoord[1]
      z1=self.helixDict[label].endCoord[2]
      print "\t\t%s=%.3f, %.3f, %.3f, %.3f, %.3f, %.3f" %(label,x0,y0,z0,x1,y1,z1)
      outfile.write("\t\t%s=%.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n" %(label,x0,y0,z0,x1,y1,z1))
      #outfile.write("\t\t%s=%s\n" %(label,'??'))
     
    outfile.write("\tsheetDict:\n")
    for key in self.sheetDict:
      value=self.sheetDict[key]
      outfile.write("\t\t%s=%s\n" %(key,value))

    outfile.write("\tstrandDict:\n")
    for key in self.sheetDict:
      value=self.sheetDict[key]
      outfile.write("\t\t%s=%s\n" %(key,value))

    outfile.close()
