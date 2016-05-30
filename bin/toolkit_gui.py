#!/usr/bin/env python

from PyQt4 import QtGui, QtCore, QtOpenGL

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import sys
import argparse

from ToolkitGui import MainWindowForm
from ToolkitGui import MainWindowForm2
from ToolkitGui import MainWindowForm3
from ToolkitGui import MainWindowForm4


def main():
    parser = argparse.ArgumentParser(description='Gorgon Toolkit')
    
    parser.add_argument('volume', action="store")
    parser.add_argument('skeleton', action="store")
    parser.add_argument('sequence', action="store")
    parser.add_argument('helix', action="store")
    parser.add_argument('output', action="store")
    args = parser.parse_args()

    app = QtGui.QApplication(sys.argv)

    windows = []
#     windows.append(MainWindowForm('2.2.2', args))
#     windows.append(MainWindowForm2('2.2.3', args))
#     windows.append(MainWindowForm3('2.2.4', args))
    windows.append(MainWindowForm4('2.2.5', args))
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
