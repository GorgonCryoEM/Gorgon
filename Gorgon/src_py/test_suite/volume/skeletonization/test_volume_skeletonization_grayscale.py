import sys, os

sys.path.append('../../../')

from gorgon import *

volumeViewer             = window.windowManager.volumeViewer
grayscaleSkeletonizer    = volumeViewer.grayscaleSkeletonizer

volumeViewer.loadDataFromFile('densityMap.mrc')

skeletonRenderer = grayscaleSkeletonizer.app.viewers["skeleton"].renderer
skeleton = grayscaleSkeletonizer.viewer.renderer.performPreservingGrayscaleSkeletonizationAbeysinghe2008(
                    skeletonRenderer.getMesh(),
                    grayscaleSkeletonizer.getStartingDensity(),
                    grayscaleSkeletonizer.getStepCount(),
                    grayscaleSkeletonizer.getMinCurveLength(),
                    grayscaleSkeletonizer.getMinSurfaceSize(),
                    grayscaleSkeletonizer.getCurveRadius(),
                    grayscaleSkeletonizer.getSurfaceRadius(),
                    grayscaleSkeletonizer.getSkeletonRadius()
                )

viewer   = grayscaleSkeletonizer.app.viewers["skeleton"]
viewer.loadVolume(skeleton)

viewer.renderer.saveFile('grayscale_skeletonized_test.mrc')
