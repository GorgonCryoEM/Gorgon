import os
from os import path
from subprocess import check_call
from filecmp import cmp
import termcolor
from . import EXE_PATH


class TestSSECorrespondence(ToolkitTestCases.ToolkitTests):

	def __init__(self):
		if os.getenv('CONDA_BUILD_STATE') == 'TEST':
			prog_name = 'gorgon-sse-correspondence'
		else:
			prog_name = 'gorgon_sse_correspondence.py'
		
		ToolkitTestCases.ToolkitTests.__init__(self, 'corr', prog_name=prog_name)
		
	def get_inputs(self):
		return [self.skeleton, self.segment, self.helices]

	def test_sse_correspondence(self):
		self.run()
		

if __name__ == '__main__':
    import doctest
    doctest.testmod()
