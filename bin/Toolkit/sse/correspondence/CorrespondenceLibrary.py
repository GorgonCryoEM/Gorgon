#!/usr/bin/python
# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:  Mike Marsh (michael.marsh@bcm.edu)
# Class: CorrespondenceLibrary
# Class Description: Class that stores a set of correspondences and the parameters used to compute them.
#                    More info in: correspondence-doc.txt
#

import cPickle as pickle

class CorrespondenceLibrary:
  def __init__(self, sp=None, so=None, algo=None, params=None, correspondenceList=None, comments=None, filename=None):
    if (filename):
      self.load(filename)
    else:
      self.structurePrediction=sp
      self.structureObservation=so
      self.algorithm=algo
      self.params=params
      self.correspondenceList=correspondenceList
      self.currentCorrespondenceIndex = None
      self.otherAttribs={}
      self.otherAttribs['comments']=comments
      # sort correspondences by score
  def getCurrentCorrespondenceIndex(self):
      return self.currentCorrespondenceIndex
  def setCurrentCorrespondenceIndex(self, corrIndex):
      self.currentCorrespondenceIndex = corrIndex

  def save(self, filename):
    # write observed & SSE parameters
    # write StructurePrediction & params
    # write algorithm, params, comments
    # write correspondence list by calling Correspondence.__repr__
    outfile=open(filename,'w')
    pickle.dump(self, outfile)
    outfile.close()
  
  @classmethod
  def load(cls, filename):
    infile=open(filename,'r')
    result = pickle.load(infile)
    infile.close()
    return result

#    self.structurePrediction=None
#    self.structureObservation=None
#    self.algorithm=None
#    self.params=None
#    self.correspondenceList=None
#    self.params={}
#    self.files={}
#    self.otherAttribs={}
#
#    infile=open(filename)
#    lines=infile.readlines()
#
#    for line in lines:
#
#      # Attributes with arbitrary key values.  Includes 'comments'
#      if line.rfind('\t') < 1 and line.find('=') > 0 :
#        key=line.strip().split('=')[0]
#        value=line.strip().split('=')[1]
#        self.otherAttribs[key]=value
#
#      # Nested Dictionary Items those found in dictionaries 'files' and 'params'
#      else:
#        # last character in line is ':'
#        if line.find(':') == len(line.strip()) :
#          dictName=line.strip().split(':')[0]
#
#        else:
#          # line holds key=value pair
#          if line.find('=') > 0:
#            split=line.strip().split('=')
#            key=split[0]
#            value=split[1]
#            self.__addAttrib (dictName, key, value)
