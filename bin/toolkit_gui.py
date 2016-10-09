#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from gorgon.ToolkitGui import BinarySkeletonization
from gorgon.ToolkitGui import GrayscaleSkeletonization
from gorgon.ToolkitGui import SSEIdentification
from gorgon.ToolkitGui import SSECorrespondence
from gorgon.ToolkitGui import CalphaAtomPlacement


def main():
    parser = argparse.ArgumentParser(description='Gorgon Toolkit Gui')
    
    parser.add_argument('volume')
    parser.add_argument('skeleton')
    parser.add_argument('sequence')
    parser.add_argument('helix')
    parser.add_argument('calpha')
    parser.add_argument('output')
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    windows = []
    windows.append(BinarySkeletonization('2.2.2', args))
    windows.append(GrayscaleSkeletonization('2.2.3', args))
    windows.append(SSEIdentification('2.2.4', args))
    windows.append(SSECorrespondence('2.2.5', args))
    windows.append(CalphaAtomPlacement('2.2.6', args))
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
