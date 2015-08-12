import sys, os

sys.path.append('../../')

from gorgon import *

# volumeViewer      = window.windowManager.volumeViewer
skeletonViewer = window.windowManager.skeletonViewer
# laplacianSmoother = volumeViewer.laplacianSmoother

skeletonViewer.loadDataFromFile('densityMap-skeleton.mrc')

skeletonLaplacian = skeletonViewer.optionsWindow
action = skeletonLaplacian.app.actions.getAction("perform_SkeletonLaplacianSmoothing")
action.trigger()

skeletonLaplacian.accept()

skeletonViewer.renderer.saveFile('laplacian_smoothed_skeleton_test.mrc')
