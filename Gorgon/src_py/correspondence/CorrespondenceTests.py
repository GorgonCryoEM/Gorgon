import unittest

from StructureObservation import StructureObservation
from ObservedHelix import ObservedHelix
from ObservedSheet import ObservedSheet
from ObservedStrand import ObservedStrand

from StructurePrediction import StructurePrediction

from Match import Match
from Correspondence import Correspondence
from CorrespondenceLibrary import CorrespondenceLibrary

class CorrespondenceTests(unittest.TestCase):
  def setUp(self):
    pass

  def testCorrespondenceLibraryIO(self):
    self.assertEqual (1,0)

  def testB(self):
    self.assertEqual (1,1)

  def testC(self):
    self.assertEqual (1,1)

  def testD(self):
    self.assertEqual (1,1)

  def tearDown(self):
    pass
