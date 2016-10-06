#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from gorgon.ToolkitGui import BinarySkeletonization


def main():
    parser = argparse.ArgumentParser(description='Gorgon Binary Skeletonization Gui')
    
    parser.add_argument('volume')
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    window = BinarySkeletonization('2.3', args)
    
    window.resize(800, 600)
#     window.showMaximized()
    window.show()
    window.raise_()
    
    window.load()
   
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
