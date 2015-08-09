import sys, os

sys.path.append('../../')

from gorgon import *

volumeViewer      = window.windowManager.volumeViewer
laplacianSmoother = volumeViewer.laplacianSmoother

volumeViewer.loadDataFromFile('densityMap.mrc')

v1 = laplacianSmoother.ui.doubleSpinBoxConvergenceRate.value()
v2 = laplacianSmoother.ui.spinBoxNoOfIterations.value()
laplacianSmoother.viewer.renderer.performSmoothLaplacian(v1, v2)

volumeViewer.renderer.saveFile('laplacian_smoothed_vol_test.mrc')
