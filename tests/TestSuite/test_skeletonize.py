import os
from .toolkit_tests import ToolkitTestCases


class TestSkeletonization(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		if os.getenv('CONDA_BUILD_STATE') == 'TEST':
			prog_name = 'gorgon-skeletonize'
		else:
			prog_name = 'gorgon_skeletonize.py'
		
		ToolkitTestCases.ToolkitTests.__init__(self, 'mrc', prog_name=prog_name)
		
	def get_inputs(self):
		return [self.volume]

	def test_binary(self):
		self.run('binary')
	
	def test_grayscale(self):
		self.run('grayscale')
		
	test_grayscale.long = True
	
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
