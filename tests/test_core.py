import libpycore as gorgon

def test():
		assert gorgon.hello() == 'Hello Core!'

def hello_world():
    """ Prints Hello Core!

    >>> gorgon.hello()
    'Hello Core!'
    """
    print gorgon.hello()

def hello():
    """ Returns Hello Core!

    >>> gorgon.hello()
    'Hello Core!'
    """
    return gorgon.hello()

if __name__ == '__main__':
    import doctest
    doctest.testmod()
