import sys, os

sys.path.append('../../../')

from gorgon import *

volumeViewer             = window.windowManager.volumeViewer
grayscaleSkeletonizer    = volumeViewer.grayscaleSkeletonizer
action = grayscaleSkeletonizer.app.actions.getAction("perform_VolumeGrayscaleSkeletonization")

volumeViewer.loadDataFromFile('densityMap.mrc')

action.trigger()
grayscaleSkeletonizer.accept()

viewer   = grayscaleSkeletonizer.app.viewers["skeleton"]
viewer.renderer.saveFile('grayscale_skeletonized_test.mrc')
