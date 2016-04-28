#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from ToolkitGui import MainWindowForm


def main():
    parser = argparse.ArgumentParser(description='Gorgon Toolkit')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    window = MainWindowForm('2.2.2', args)
    
    window.resize(800, 600)
#     window.move(300,50)
    window.show()
    window.raise_()
   
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
