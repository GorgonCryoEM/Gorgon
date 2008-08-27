#!/usr/bin/python

#gorgon_model.py
#Gorgon Models  python module
#
# extends seq_model.Atom to account for Gorgon Viewer interface 

from libpyGORGON import PDBAtom,Vector3DFloat
from seq_model.Chain import Chain
from seq_model.Atom import Atom

#--------------------------------------
class GAtom(Atom):
  def __init__(self, element, x,y,z, viewer, occupancy=1.0, temp_factor=1.0):
		Atom.__init__(self, element, x,y,z, occupancy=1.0, temp_factor=1.0)
		self.viewer=viewer

		#create C++ PDBAtom object and attributes
		self.raw_atom=PDBAtom()
		self.raw_atom.setElement(element)
		self.raw_atom.setPosition(Vector3DFloat(x,y,z))
		self.raw_atom.setOccupancy(occupancy)
		self.raw_atom.setTempFactor(temp_factor)
		self.raw_atom.setCharge('')

		#Add atom to viewer and update
		self.viewer.renderer.addAtom(self.raw_atom)
		if(not self.viewer.loaded):
			self.viewer.loaded = True
			self.viewer.emitModelLoaded()
		self.viewer.dirty = True
		self.viewer.emitModelChanged()


#--------------------------------------
def main(argv):

  from seq_model import *
  # Instantiate my_chain
  print '\n\n\n********************** executing main() ********************\n'
  my_chain = Chain("ACEFGHIKLACEFGHIKLMNPYVWQPMIKESMATTHEW")
  print 'my_chain=%s' %my_chain

  my_chain.secel_list.append (Coil (my_chain,'l0',1,9))
  my_chain.secel_list.append (Helix(my_chain,'h1',10,20))
  my_chain.secel_list.append (Coil (my_chain,'l2',21,25))
  my_chain.secel_list.append (Helix(my_chain,'h3',26,30))
  my_chain.secel_list.append (Coil (my_chain,'l4',31,38))


  # Instantiate sub_chain
  sub_chain=my_chain[14:20]
  print 'sub_chain=%s' %sub_chain


  # Add coordinate records to my_chain
  for residue in my_chain[3:-5]:
  #or residue in my_chain:
      x=random.gauss(20.0,5.0)
      y=random.gauss(20.0,5.0)
      z=random.gauss(20.0,5.0)
      residue.atoms['CA']=Atom('C',x,y,z)


  # Test to_pdb on both chains
  print '\nmy_chain.to_pdb\n' + my_chain.to_pdb(backbone_only=True)

  print '\nsub_chain.to_pdb\n' + sub_chain.to_pdb(backbone_only=True)


 
  '''
  # Test chain-slicing
  #ew_chain = Chain.read('inputfile.txt')
  print "my_chain = %s" %my_chain

  print "\nmy_chain[:10]"
  print my_chain[:10]

  print "\nmy_chain[:-3]"
  print my_chain[:-3]

  print "\nmy_chain[-1:]"
  print my_chain[-1:]

  print "\nmy_chain[-3:]"
  print my_chain[-3:]

  print "\nmy_chain[3:7]"
  print my_chain[3:7]
  '''



if __name__=="__main__":
  from sys import argv
  main(argv)

