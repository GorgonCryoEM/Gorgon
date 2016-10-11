import unittest

import libpyhello as libpy


class Hello(unittest.TestCase):

	def test(self):
			assert libpy.hello() == 'Hello World!'
	
	def hello_print(self):
	    """ Prints Hello World!
	
	    >>> libpy.hello()
	    'Hello World!'
	    """
	    print libpy.hello()
	
	def hello_return(self):
	    """ Returns Hello World!
	
	    >>> libpy.hello()
	    'Hello World!'
	    """
	    return libpy.hello()
