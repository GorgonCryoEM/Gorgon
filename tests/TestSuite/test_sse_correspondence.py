from .toolkit_tests import ToolkitTestCases


class TestSSECorrespondence(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		ToolkitTestCases.ToolkitTests.__init__(self, 'corr', prog_name='sse_correspondence.py')

	def test_sse_correspondence(self):
		input1 = join(self.indir, 'densityMap-skeleton.mrc')
		input2 = join(self.indir, 'groel-segment.seq')
		input3 = join(self.indir, 'helices-densityMap.wrl')
		output = join(self.outdir, 'sse_correspondences.txt')
		ref    = join(self.refdir, 'sse_correspondences.txt')

		cmd = '%s %s %s %s %s' % (self.exe, input1, input2, input3, output)
		
		check_call([cmd], shell=True)
		assert cmp(output, ref), "\nFiles differ:\n   1: %s\n   2: %s" % (output, ref)
		

if __name__ == '__main__':
    import doctest
    doctest.testmod()
