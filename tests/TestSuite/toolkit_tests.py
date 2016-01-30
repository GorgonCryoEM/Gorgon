from os      import path, mkdir
from os.path import join

import unittest

from subprocess import check_call
from filecmp import cmp


class ToolkitTestCases(unittest.TestCase):

	class ToolkitTests:

		def __init__(self):
			topdir   = '.'
			testsdir = 'tests'

			self.bindir        = join(topdir,     'bin')
			self.indir         = join(testsdir,   'groel')
			self.outdir        = join(testsdir,   'outputs')
			self.refdir        = join(self.indir, 'refs')

			self.input = join(self.indir, 'densityMap.mrc')
			self.exe   = join(self.bindir, self.py)

			if not path.exists(self.outdir):
				mkdir(self.outdir)

		def run(self, option):
			output = join(self.outdir, self.fprefix + option + '.mrc')
			ref    = join(self.refdir, self.fprefix + option + '.mrc')

			cmd = '%s %s %s %s %s' % (self.exe, self.input, output, self.prog, option)

			check_call([cmd], shell=True)
			assert cmp(output, ref), "\nFiles differ:\n   1: %s\n   2: %s" % (output, ref)
