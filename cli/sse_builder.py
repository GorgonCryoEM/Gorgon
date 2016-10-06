#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from gorgon.ToolkitGui import SSEIdentification


def main():
    parser = argparse.ArgumentParser(description='Gorgon Toolkit - SSE Builder')
    
    parser.add_argument('volume')
    parser.add_argument('skeleton')
    parser.add_argument('output')
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    window = SSEIdentification('2.2.4', args)
    
    window.resize(800, 600)
    window.show()
    window.raise_()
    
    window.load()
   
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
