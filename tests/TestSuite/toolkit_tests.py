import os
from os      import path, mkdir
from os.path import join

import unittest
import abc

from subprocess import check_call
from filecmp import cmp
import termcolor

from . import EXE_PATH


class ToolkitTestCases(unittest.TestCase):

	class ToolkitTests:
		__metaclass__ = abc.ABCMeta
		
		@abc.abstractmethod
		def get_inputs(self):
			'''Return list of inputs for the test'''
			return

		def __init__(self, out_extension, topdir=path.curdir, prog_name='gorgon_cli.py', prog_option=None):
			if prog_name == 'gorgon_cli.py' and os.getenv('CONDA_BUILD_STATE') == 'TEST':
				prog_name = 'gorgon'
			self.prog_name     = prog_name
			self.prog_option   = prog_option
			self.out_extension = '.' + out_extension
			
			testsdir           = join(topdir, 'tests')

			self.indir         = join(topdir,     'demo', 'groel')
			self.outdir        = join(testsdir,   'outputs')
			self.refdir        = join(self.indir, 'refs')

			self.volume        = join(self.indir, 'densityMap.mrc')
			self.skeleton      = join(self.indir, 'densityMap-skeleton.mrc')
			self.segment       = join(self.indir, 'groel-segment.seq')
			self.helix_lengths = join(self.indir, 'helix-lengths.sse')
			self.helices       = join(self.indir, 'helices-densityMap.wrl')
			
			if EXE_PATH:
				self.exe          = join(EXE_PATH, self.prog_name)
			else:
				self.exe          = self.prog_name
			
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
				cmd_option = '%s %s' % (self.prog_option, option)
			else:
				cmd_option = '%s' % (option)
			
			cmd = '%s %s %s %s' % (self.exe, inputs, self.output, cmd_option)
			
			for f in [self.ref, inputs]:
				if path.isfile(f):
					print "%s does NOT exist" % f
				else:
					print "%s does exist" % f
				
				check_call([self.exe, "-h"], shell=True)
			return cmd

		def run(self, option=''):
			print termcolor.colored("\n+ %s" % self._cmd(option), "green")
			
			# remove output file in case left from previous test runs
			if path.isfile(self.output):
				os.remove(self.output)
			check_call([self._cmd(option)], shell=True)
			assert cmp(self.output, self.ref), "\nFiles differ:\n   1: %s\n   2: %s" % (self.output, self.ref)
