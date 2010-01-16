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
		self.setSkeletonScores()
		self.setCorrelationScores(None, 30*pi/180)
		self.setGeometryScores()
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
		self.sseh.setGeometryScores(self.volume, self.resolution, self.threshold)
		
	def getAtomDistances(self):
		return self.sseh.getAtomDistances()
		
	def getNeighborhoodVoxels(self):
		return self.sseh.getNeighborhoodVoxels(self.volume, threshold)
		
	def getLocalDirectionalityScores(self):
		return self.sseh.getLocalDirectionalityScores(self.volume)