from .toolkit_tests import ToolkitTestCases


class TestSSECorrespondence(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		ToolkitTestCases.ToolkitTests.__init__(self, 'corr', prog_name='sse_correspondence.py')

	def test_sse_correspondence(self):
		self.run()
		

if __name__ == '__main__':
    import doctest
    doctest.testmod()
