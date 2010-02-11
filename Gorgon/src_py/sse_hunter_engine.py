# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross Coleman
# Description:   An engine which performs SSE Hunter (Scoring of PDB atoms based on how likely it is that they form Secondary Structure Elements)

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$


from libpyGORGON import SSEHunter, RadialProfileType
from math import *

class SSEHunterEngine:
	def __init__(self, volume, skeleton, resolution, threshold):
		self.volume = volume
		self.skeleton = skeleton
		self.resolution = resolution
		self.threshold = threshold
		self.sseh = SSEHunter()
		
		
	def getScoredAtoms(self, correlationWeight, skeletonWeight, geometryWeight):
		self.createPseudoAtoms()
		self.setGeometryScores()
		self.setSkeletonScores()
		self.setCorrelationScores()
		pseudoatoms = []
		for i in range(self.getNumberOfPseudoAtoms()):
			pseudoatom = self.getPseudoAtom(i)
			score = correlationWeight*pseudoatom.getCorrelationScore()
			score += skeletonWeight*pseudoatom.getSkeletonScore()
			score += geometryWeight*pseudoatom.getGeometryScore()
			pseudoatom.setTempFactor(score)
			pseudoatoms.append(pseudoatom)
		return pseudoatoms
		
		
	def createPseudoAtoms(self):
		self.sseh.createPseudoAtoms(self.volume, self.resolution, self.threshold)
		
	def getNumberOfPseudoAtoms(self):
		return self.sseh.getNumberOfPseudoAtoms()
		
	def getPseudoAtom(self, index):
		return self.sseh.getPseudoAtom(index)
		
	def setCorrelationScores(self, radialProfileType = None, deltaAngleRadians= None):
		if not radialProfileType:
			radialProfileType = RadialProfileType.gaussianDip
		if not deltaAngleRadians:
			deltaAngleRadians = 5*pi/180
		self.sseh.setCorrelationScores(self.volume, radialProfileType, self.resolution, deltaAngleRadians)
		
	def setSkeletonScores(self):
		self.sseh.setSkeletonScores(self.volume, self.skeleton, self.resolution)
		
	def setGeometryScores(self):
		#self.sseh.setGeometryScores(self.volume, self.resolution, self.threshold)
		localDirectionalityScores = self.getLocalDirectionalityScores()
		numPAtoms = self.getNumberOfPseudoAtoms()
		assert len(localDirectionalityScores) == numPAtoms
		for i in range(numPAtoms):
			pseudoatom = self.getPseudoAtom(i)
			score = localDirectionalityScores[i]
			#score += ???
			#socre += ???
			pseudoatom.setGeometryScore(score)
		
		
	def getAtomDistances(self):
		return self.sseh.getAtomDistances()
		
	def getNeighborhoodVoxels(self):
		return self.sseh.getNeighborhoodVoxels(self.volume, threshold)
		
	def getLocalDirectionalityScores(self):
		return self.sseh.getLocalDirectionalityScores(self.volume)