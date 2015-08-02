from libpyGORGON import *
from sse_hunter_engine import SSEHunterEngine

#The idea of this program is to compare results from the EMAN1 program ssehunter3.py with those from Gorgon's SSEHunter
#The reasons why Gorgon scores differ from those in EMAN1 is partly because of improvements Sasakthi made to the scoring process
# but there very well could also be bugs in Gorgon's code or even in EMAN1's code (but EMAN1 is no longer under active development).

#This script is not general purpose (yet) because I'm just using it for debugging

eman1_csv_filename = "/Users/ross/Work/score-AVGmonomer96.csv"
volume_filepath = "/Users/ross/Work/AVGmonomer96.mrc"
skeleton_filepath = "/Users/ross/Work/AVGmonomer_gorgon_binary_skeleton.off"
resolution = 7.0
threshold = 13.0

#In EMAN1 ssehunter3.py, helixsize==3 and sheetsize==4
min_curve = 3 #helixsize?
min_surface = 4 #sheetsize?


def getScoredAtoms(engine, correlationWeight, skeletonWeight, geometryWeight):
	"""
	This is basically copied from sse_hunter_engine.py, but has some modifications to make it work here. 
	The idea is to modify things here rather than in sse_hunter_engine unless bug fixes are found... then incorporate them in sse_hunter_engine
	"""
	
	engine.createPseudoAtoms()
	engine.setSkeletonScores()
	engine.setCorrelationScores()
	numPAtoms = engine.getNumberOfPseudoAtoms()
	pseudoatoms = [engine.getPseudoAtom(i) for i in range(numPAtoms)]
	corrScores = [p.getCorrelationScore() for p in pseudoatoms]
	engine.setGeometryScores(corrScores)
	
	origin = (engine.volume.getOriginX(), engine.volume.getOriginY(), engine.volume.getOriginZ())
	apix = (engine.volume.getSpacingX(), engine.volume.getSpacingY(), engine.volume.getSpacingZ())
	pseudoatom_dicts = []
	for i in range(len(pseudoatoms)):
		pAtom = pseudoatoms[i]
		loc = pAtom.getPosition()
		loc = [loc.x(), loc.y(), loc.z()]
		loc[0] -= origin[0] #getting them like in the ssehunter3.py printout of EMAN1
		loc[1] -= origin[1]
		loc[2] -= origin[2]
		score = correlationWeight*pAtom.getCorrelationScore()
		score += skeletonWeight*pAtom.getSkeletonScore()
		score += geometryWeight*pAtom.getGeometryScore()
		pAtom.setTempFactor(score)
		pAtomDict = {"serial": pAtom.getSerial(), "x":loc[0], "y":loc[1], "z":loc[2], "correlation_score": pAtom.getCorrelationScore(), "skeleton_score":pAtom.getSkeletonScore(), "geometry_score":pAtom.getGeometryScore()}
		pseudoatom_dicts.append(pAtomDict)
		print "%3i: (%6.2f, %6.2f, %6.2f)\tCorr: %6.3f\tSkel: %6.3f\tGeom: %6.3f\tOverall: %6.3f" % (i+1, loc[0], loc[1], loc[2], pAtom.getCorrelationScore(), pAtom.getSkeletonScore(), pAtom.getGeometryScore(), score)
	return pseudoatom_dicts #pseudoatoms



# This reads in EMAN1 ssehunter3.py scores
eman_cvs_file = open(eman1_csv_filename, "r")
import csv
csv_reader = csv.reader(eman_cvs_file, quoting=csv.QUOTE_NONE)
header = csv_reader.next()
print header
print "\n\n"

eman1_scores = []
for row in csv_reader:
	atom_dict = {"serial": int(row[0]), "x":float(row[1]), "y":float(row[2]), "z":float(row[3]), "correlation_score": float(row[4]), "skeleton_score": float(row[5]), "geometry_score": float(row[6])}
	eman1_scores.append(atom_dict)


#Running Gorgon's SSEHunter from the comand line
vol_renderer = VolumeRenderer()
mesh_renderer = MeshRenderer()

vol_renderer.loadFile(volume_filepath)
volume = vol_renderer.getVolume()

if True:
	skeleton = vol_renderer.performBinarySkeletonizationJu2007(threshold, min_curve, min_surface)
	mesh_renderer.loadVolume(skeleton)
	skeleton = mesh_renderer.getMesh()
	mesh_renderer.saveFile(skeleton_filepath)
	mesh_renderer.saveFile(skeleton_filepath + ".mrc")
else:
	mesh_renderer.loadFile(skeleton_filepath)
	skeleton = mesh_renderer.getMesh()

engine = SSEHunterEngine(volume, skeleton, resolution, threshold)
#scored_atoms = engine.getScoredAtoms(1.0, 1.0, 1.0)
scored_atoms = getScoredAtoms(engine, 1.0, 1.0, 1.0)


#Comparing scores
for i in range(len(scored_atoms)):
	print "\n%20s\t%s\t%s" % (" ", "EMAN1", "Gorgon")
	gorgon_score_dict = scored_atoms[i]
	eman1_score_dict = eman1_scores[i]
	for key in ["serial", "x", "y", "z", "correlation_score", "skeleton_score", "geometry_score"]:
		print "%20s\t%.2f\t%.2f" % (key, eman1_score_dict[key], gorgon_score_dict[key])


#json module is in Python 2.6 or later
import json
outfile = open("ssehunter_scores.json", 'w')
json.dump(scored_atoms, outfile, sort_keys = True, indent=4)
outfile.close()



