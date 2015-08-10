import sys, os

sys.path.append('../../')

from gorgon import *

volumeViewer = window.windowManager.volumeViewer

volumeViewer.loadDataFromFile('densityMap.mrc')
volumeViewer.renderer.normalizeVolume()
volumeViewer.renderer.saveFile('normalized_vol_test.mrc')
