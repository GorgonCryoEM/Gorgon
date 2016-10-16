from .toolkit_tests import ToolkitTestCases


class TestSSECorrespondence(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		ToolkitTestCases.ToolkitTests.__init__(self, 'corr', prog_name='gorgon_sse_correspondence.py')
		
	def get_inputs(self):
		return [self.skeleton, self.segment, self.helices]

	def test_sse_correspondence(self):
		self.run()
		

if __name__ == '__main__':
    import doctest
    doctest.testmod()
