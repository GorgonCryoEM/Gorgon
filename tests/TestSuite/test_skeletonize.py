from .toolkit_tests import ToolkitTestCases


class TestSkeletonization(ToolkitTestCases.ToolkitTests):

	def __init__(self):
# 		self.inputs = [self.volume]
		
		ToolkitTestCases.ToolkitTests.__init__(self, 'mrc', prog_option='skeletonize')
		
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
