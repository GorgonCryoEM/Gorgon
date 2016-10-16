from .toolkit_tests import ToolkitTestCases


class TestSSEHunter(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		ToolkitTestCases.ToolkitTests.__init__(self, 'pdb', prog_name='gorgon_sse_hunter.py')

	def get_inputs(self):
		return [self.volume, self.skeleton]
	
	def test_sse_hunter(self):
		self.run()

	test_sse_hunter.long = True
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
