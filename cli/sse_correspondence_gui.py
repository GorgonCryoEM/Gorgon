#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from gorgon.gui import SSECorrespondence


def main():
    parser = argparse.ArgumentParser(description='Gorgon SSE Correspondence Gui')
    
    parser.add_argument('skeleton')
    parser.add_argument('sequence')
    parser.add_argument('helix')
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    window = SSECorrespondence('2.3', args)

    window.resize(800, 600)
    #     window.showMaximized()
    window.show()
    window.raise_()

    window.load()
   
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
