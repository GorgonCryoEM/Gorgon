from .toolkit_tests import ToolkitTestCases


class TestFilter(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		ToolkitTestCases.ToolkitTests.__init__(self, 'mrc', prog_option='filter')
		
	def get_inputs(self):
		return [self.volume]

	def test_normalize(self):
		self.run('normalize')
		
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
