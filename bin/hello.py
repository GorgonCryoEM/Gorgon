#!/usr/bin/env python

import libpygorgon

def hello_world():
    """ Prints Hello World!
    
    >>> libpygorgon.hello()
    'Hello World!'
    """
    print libpygorgon.hello()

def hello():
    """ Returns Hello World!
    
    >>> libpygorgon.hello()
    'Hello World!'
    """
    return libpygorgon.hello()

if __name__ == '__main__':
    import doctest
    doctest.testmod()
