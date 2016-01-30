from os      import path, mkdir
from os.path import join

import unittest

from subprocess import check_call
from filecmp import cmp


class ToolkitTestCases(unittest.TestCase):

	class ToolkitTests:

		def __init__(self, out_extension, topdir=path.curdir, prog_name='gorgon.py', prog_option=None):
			self.prog_name     = prog_name
			self.prog_option   = prog_option
			self.out_extension = '.' + out_extension
			
			testsdir           = join(topdir, 'tests')

			self.bindir        = join(topdir,     'bin')
			self.indir         = join(testsdir,   'groel')
			self.outdir        = join(testsdir,   'outputs')
			self.refdir        = join(self.indir, 'refs')

			self.volume        = join(self.indir, 'densityMap.mrc')
			self.skeleton      = join(self.indir, 'densityMap-skeleton.mrc')
			self.segment       = join(self.indir, 'groel-segment.seq')
			self.helix_lengths = join(self.indir, 'helix-lengths.sse')
			self.helices       = join(self.indir, 'helices-densityMap.wrl')
			
			self.exe          = join(self.bindir, self.prog_name)
			
			if self.prog_option:
				self.outprefix    = self.prog_option + '_'
			else:
				self.outprefix    = path.splitext(self.prog_name)[0]
		
			if not path.exists(self.outdir):
				mkdir(self.outdir)
			
		def _cmd(self, option):
			filename = self.outprefix + option + self.out_extension
			
			self.output = join(self.outdir, filename)
			self.ref    = join(self.refdir, filename)
			
			inputs = ' '.join(self.get_inputs())
			if self.prog_option:
				cmd_option = '--%s %s' % (self.prog_option, option)
			else:
				cmd_option = ''
			
			cmd = '%s %s %s %s' % (self.exe, inputs, self.output, cmd_option)
			
			return cmd

		def run(self, option):
			check_call([cmd], shell=True)
			assert cmp(output, ref), "\nFiles differ:\n   1: %s\n   2: %s" % (output, ref)
