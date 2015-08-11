import TestSuite

pathname = TestSuite.__path__[0]+'/sse/'

skeletonViewer = TestSuite.window.windowManager.skeletonViewer
sseViewer      = TestSuite.window.windowManager.sseViewer
calphaViewer   = TestSuite.window.windowManager.calphaViewer

def helixCorrespondance():
    helixCorrespondanceFinder = sseViewer.helixCorrespondanceFinder
    ui                        = helixCorrespondanceFinder.ui
    
    skeletonViewer.loadDataFromFile(pathname +'densityMap-skeleton.mrc')
    sseViewer.loadHelixDataFromFile(pathname +'helices-densityMap.wrl')
    calphaViewer.loadDataFromFile(pathname +'groel-segment.seq')
    
    ui.lineEditHelixLocationFile.setText(pathname +'helices-densityMap.wrl')
    ui.lineEditSkeletonFile.setText(pathname +'densityMap-skeleton.mrc') 
    ui.lineEditSequenceFile.setText(pathname +'groel-segment.seq')
    
    helixCorrespondanceFinder.checkOk()
    helixCorrespondanceFinder.accept()
