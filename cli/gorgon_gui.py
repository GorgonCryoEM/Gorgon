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
    
    parser.add_argument('volume')
    parser.add_argument('skeleton')
    parser.add_argument('sequence')
    parser.add_argument('helix')
    parser.add_argument('calpha')
    parser.add_argument('output')
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    windows = []
    windows.append(BinarySkeletonization(args))
    windows.append(GrayscaleSkeletonization(args))
    windows.append(SSEIdentification(args))
    windows.append(SSECorrespondence(args))
    windows.append(CalphaAtomPlacement(args))
#     exit()
    
    for window in windows:
        window.resize(800, 600)
#         window.showMaximized()
    #     window.move(300,50)
        window.show()
        window.raise_()
        
        window.load()
   
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
