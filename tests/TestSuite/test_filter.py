from .toolkit_tests import ToolkitTestCases


class TestFilter(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		self.py = 'gorgon.py'
		self.prog = '--filter'
		self.fprefix = 'filter_'
		ToolkitTestCases.ToolkitTests.__init__(self)

	def test_normalize(self):
		self.run('normalize')
		
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
