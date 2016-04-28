#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from libpytoolkit import *
from Explorer import MainWindowForm
from Explorer.widget3 import Widget3


def main():
    parser = argparse.ArgumentParser(description='Gorgon Toolkit')
    
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    window = MainWindowForm('2.2.2')
    
    window.resize(800, 600)
#     window.move(300,50)
    window.show()
    window.raise_()
    window.load()
   
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
