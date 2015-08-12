import sys, os

sys.path.append('../../')

from gorgon import *

skeletonViewer = window.windowManager.skeletonViewer
sseViewer      = window.windowManager.sseViewer
calphaViewer   = window.windowManager.calphaViewer

helixCorrespondanceFinder = sseViewer.helixCorrespondanceFinder
ui                        = helixCorrespondanceFinder.ui

skeletonViewer.loadDataFromFile('densityMap-skeleton.mrc')
sseViewer.loadHelixDataFromFile('helices-densityMap.wrl')
calphaViewer.loadDataFromFile('groel-segment.seq')

ui.lineEditHelixLocationFile.setText('helices-densityMap.wrl')
ui.lineEditSkeletonFile.setText('densityMap-skeleton.mrc') 
ui.lineEditSequenceFile.setText('groel-segment.seq')

helixCorrespondanceFinder.checkOk()
helixCorrespondanceFinder.accept()
