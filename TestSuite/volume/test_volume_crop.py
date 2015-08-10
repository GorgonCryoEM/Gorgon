import sys, os

sys.path.append('../../')

from gorgon import *

volumeViewer = window.windowManager.volumeViewer
cropper      = volumeViewer.cropper

volumeViewer.loadDataFromFile('densityMap.mrc')
cropper.viewer.renderer.cropVolume(20, 20, 20, 60, 60, 60)
volumeViewer.renderer.saveFile('cropped_vol_test.mrc')
