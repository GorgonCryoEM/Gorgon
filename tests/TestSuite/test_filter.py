import os
from .toolkit_tests import ToolkitTestCases


class TestFilter(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		if os.getenv('CONDA_BUILD_STATE') == 'TEST':
			prog_name = 'gorgon-filter'
		else:
			prog_name = 'gorgon_filter.py'
		
		ToolkitTestCases.ToolkitTests.__init__(self, 'mrc', prog_name=prog_name)
		
	def get_inputs(self):
		return [self.volume]

	def test_normalize(self):
		self.run('normalize')
		
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
