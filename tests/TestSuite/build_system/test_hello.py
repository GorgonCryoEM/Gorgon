import unittest

import libpyhello as gorgon


class Hello(unittest.TestCase):

	def test(self):
			assert gorgon.hello() == 'Hello World!'
	
	def hello_print(self):
	    """ Prints Hello World!
	
	    >>> gorgon.hello()
	    'Hello World!'
	    """
	    print gorgon.hello()
	
	def hello_return(self):
	    """ Returns Hello World!
	
	    >>> gorgon.hello()
	    'Hello World!'
	    """
	    return gorgon.hello()
