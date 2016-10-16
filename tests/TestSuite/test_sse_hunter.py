import os
from .toolkit_tests import ToolkitTestCases


class TestSSEHunter(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		if os.getenv('CONDA_BUILD_STATE') == 'TEST':
			prog_name = 'gorgon-sse-hunter'
		else:
			prog_name = 'gorgon_sse_hunter.py'
		
		ToolkitTestCases.ToolkitTests.__init__(self, 'pdb', prog_name=prog_name)

	def get_inputs(self):
		return [self.volume, self.skeleton]
	
	def test_sse_hunter(self):
		self.run()

	test_sse_hunter.long = True
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
