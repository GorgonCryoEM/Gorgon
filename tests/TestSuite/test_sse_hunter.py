from os.path import join

from subprocess import check_call
from filecmp import cmp

from .toolkit_tests import ToolkitTestCases


class TestFilter(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		self.py = 'sse_hunter.py'
		ToolkitTestCases.ToolkitTests.__init__(self)

	def test_sse_hunter(self):
		input2 = join(self.indir, 'densityMap-skeleton.mrc')
		output = join(self.outdir, 'pseudoatoms.pdb')
		ref    = join(self.refdir, 'scored_pseudoatoms.pdb')

		cmd = '%s %s %s %s' % (self.exe, self.input, input2, output)
		
		check_call([cmd], shell=True)
		assert cmp(output, ref), "\nFiles differ:\n   1: %s\n   2: %s" % (output, ref)
		
	test_sse_hunter.long = True
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
