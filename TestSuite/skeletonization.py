import TestSuite

pathname = TestSuite.__path__[0]+'/volume/skeletonization/'

volumeViewer = TestSuite.window.windowManager.volumeViewer

def binary():
    binarySkeletonizer    = volumeViewer.binarySkeletonizer
    
    volumeViewer.loadDataFromFile(pathname +'densityMap.mrc')
    
    param1 = binarySkeletonizer.getDensityThreshold()
    param2 = binarySkeletonizer.getMinCurveLength()
    param3 = binarySkeletonizer.getMinSurfaceSize()
    
    skeleton = binarySkeletonizer.viewer.renderer.performBinarySkeletonizationJu2007(param1, param2, param3)
    viewer   = binarySkeletonizer.app.viewers["skeleton"]
    viewer.loadVolume(skeleton)
    
    viewer.renderer.saveFile(pathname +'binary_skeletonized_test.mrc')

def grayscale():
    grayscaleSkeletonizer    = volumeViewer.grayscaleSkeletonizer
    action = grayscaleSkeletonizer.app.actions.getAction("perform_VolumeGrayscaleSkeletonization")
    
    volumeViewer.loadDataFromFile(pathname +'densityMap.mrc')
    
    action.trigger()
    grayscaleSkeletonizer.accept()
    
    viewer   = grayscaleSkeletonizer.app.viewers["skeleton"]
    viewer.renderer.saveFile(pathname +'grayscale_skeletonized_test.mrc')
