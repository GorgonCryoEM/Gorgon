#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from gorgon.gui import BinarySkeletonization
from gorgon.gui import GrayscaleSkeletonization
from gorgon.gui import SSEIdentification
from gorgon.gui import SSECorrespondence
from gorgon.gui import CalphaAtomPlacement


def main():
    guis = { 'binary-skeletonization': BinarySkeletonization,
             'grayscale-skeletonization': GrayscaleSkeletonization,
             'sse-identification': SSEIdentification,
             'sse-correspondence': SSECorrespondence,
             'calpha-placement': CalphaAtomPlacement,
             }
    
    parser = argparse.ArgumentParser(description='Gorgon GUI')

    subparsers = parser.add_subparsers()
    
    for key,val in guis.iteritems():
        subparser = subparsers.add_parser(key)
        val.set_parser(subparser)
        subparser.set_defaults(func=val)
    
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    
    window = args.func(args)
    
    window.resize(800, 600)
    # window.showMaximized()
    # window.move(300,50)
    window.show()
    window.raise_()
    
    window.load()
   
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
