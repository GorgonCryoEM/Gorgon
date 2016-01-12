from ..toolkit_tests import ToolkitTestCases


class Skeletonization(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		self.py = 'skeletonization.py'
		self.fprefix = 'skeleton_'
		ToolkitTestCases.ToolkitTests.__init__(self)

	def test_binary(self):
		self.run('binary')
	
	def test_grayscale(self):
		self.run('grayscale')
		
	test_grayscale.long = True
	
	
if __name__ == '__main__':
    import doctest
    doctest.testmod()
