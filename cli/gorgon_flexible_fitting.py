#!/usr/bin/env python

import argparse
import logging
import pip
from gorgon.toolkit import FFEngine
from PyQt4 import QtGui, QtCore, QtOpenGL
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
from gorgon.gui.explorer.main_window_form import MainWindowForm
import argparse
import sys

def main():
    parser = argparse.ArgumentParser(description='Gorgon Flexible Fitting Engine')
    parser.add_argument('--skeleton', action="store")
    parser.add_argument('--correspondence', action="store")
    parser.add_argument('--helix', action="store")
    parser.add_argument('--template', action="store")
    parser.add_argument('--skeletonIterations', action="store")
    parser.add_argument('--helixDTerm', action="store")
    parser.add_argument('--helixFTerm', action="store")
    parser.add_argument('--skeletonDistance', action="store")
    parser.add_argument('--skeletonDTerm', action="store")
    parser.add_argument('--skeletonFTerm', action="store")
    parser.add_argument('--laplacianNeighborhoodSize', action="store")
    parser.add_argument('--transformationNeighborhoodSize', action="store")
    parser.add_argument('--log', action="store",
                    dest='loglevel',
                    choices=['info', 'debug'],
                    default='info',
                    help="log level"
                    )
    args = parser.parse_args()
    '''
    Logging setup
    '''
    loglevel = getattr(logging, args.loglevel.upper())
    
    FORMAT = "%(levelname)s:%(name)s: %(funcName)s():%(lineno)s: %(message)s"
    logging.basicConfig(level=loglevel,
                        format=FORMAT
                        )
    
    logger = logging.getLogger(__name__)



    flexible_fitter = FFEngine(args)
    #flexible_fitter.application = app
    flexible_fitter.accept()

if __name__ == "__main__":
    main()