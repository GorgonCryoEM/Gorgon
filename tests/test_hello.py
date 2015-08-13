import libpygorgon as gorgon

def test():
		assert gorgon.hello() == 'Hello World!'

def hello_world():
    """ Prints Hello World!

    >>> gorgon.hello()
    'Hello World!'
    """
    print gorgon.hello()

def hello():
    """ Returns Hello World!

    >>> gorgon.hello()
    'Hello World!'
    """
    return gorgon.hello()

if __name__ == '__main__':
    import doctest
    doctest.testmod()
