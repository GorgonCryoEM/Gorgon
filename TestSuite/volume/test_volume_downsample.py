import sys, os

sys.path.append('../../')

from gorgon import *

volumeViewer = window.windowManager.volumeViewer

volumeViewer.loadDataFromFile('densityMap.mrc')
volumeViewer.renderer.downsampleVolume()
volumeViewer.renderer.saveFile('downsampled_vol_test.mrc')
