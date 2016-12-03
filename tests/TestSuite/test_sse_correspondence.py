import os
from os import path
from subprocess import check_call
from filecmp import cmp
import termcolor
from . import EXE_PATH


class TestSSECorrespondence:

	def __init__(self):
		if os.getenv('CONDA_BUILD_STATE') == 'TEST':
			prog_name = 'gorgon-sse-correspondence'
		else:
			prog_name = 'gorgon_sse_correspondence.py'
		
		
		
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
