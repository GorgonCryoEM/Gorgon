import sys, os

sys.path.append('../../../')

from gorgon import *

volumeViewer          = window.windowManager.volumeViewer
binarySkeletonizer    = volumeViewer.binarySkeletonizer

volumeViewer.loadDataFromFile('densityMap.mrc')

param1 = binarySkeletonizer.getDensityThreshold()
param2 = binarySkeletonizer.getMinCurveLength()
param3 = binarySkeletonizer.getMinSurfaceSize()

skeleton = binarySkeletonizer.viewer.renderer.performBinarySkeletonizationJu2007(param1, param2, param3)
viewer   = binarySkeletonizer.app.viewers["skeleton"]
viewer.loadVolume(skeleton)

viewer.renderer.saveFile('binary_skeletonized_test.mrc')
