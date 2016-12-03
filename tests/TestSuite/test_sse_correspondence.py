import os
from os import path
from subprocess import check_call
from filecmp import cmp
import termcolor
from . import EXE_PATH


class TestSSECorrespondence:

	def __init__(self):
		if os.getenv('CONDA_BUILD_STATE') == 'TEST':
			self.prog_name = 'gorgon-sse-correspondence'
		else:
			self.prog_name = 'gorgon_sse_correspondence.py'
		
		self.output = path.join('tests', 'outputs', 'ppp.txt')
		
		if EXE_PATH:
			self.exe          = path.join(EXE_PATH, self.prog_name)
		else:
			self.exe          = self.prog_name
				
	def test_groel(self):
		self.dir = 'groel'
		self.ref    = path.join('demo', 'groel', 'refs', 'gorgon_sse_correspondence.corr')
		self.inputs = ['densityMap-skeleton.mrc', 'groel-segment.seq', 'helices-densityMap.wrl']
		self._run()
	
	def test_rotavirus(self):
		self.dir = 'rotavirus'
		self.ref    = path.join('demo', 'rotavirus', 'refs', 'gorgon_sse_correspondence.corr')
		self.inputs = ['skeleton-vp6-b0.4.mrc', 'vp6.seq', 'vp6-helices.wrl']
		self._run()
		
	def _run(self):
		inputs = [path.join(path.curdir, 'demo', self.dir, f) for f in self.inputs]
		
		# remove output file in case left from previous test runs
		if path.isfile(self.output):
			os.remove(self.output)

		cmd = [self.exe] + inputs + ['--output', self.output]
		print termcolor.colored("\n+ %s" % cmd, "green")
		check_call(cmd)

		assert cmp(self.output, self.ref), "\nFiles differ:\n   1: %s\n   2: %s" % (self.output, self.ref)
		

if __name__ == '__main__':
    import doctest
    doctest.testmod()
