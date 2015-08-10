from TestSuite import *
import TestSuite

pathname = TestSuite.__path__[0]+'/volume/'

def normalize():
    volumeViewer = TestSuite.window.windowManager.volumeViewer
     
    volumeViewer.loadDataFromFile(pathname +'densityMap.mrc')
    volumeViewer.renderer.normalizeVolume()
    volumeViewer.renderer.saveFile(pathname +'normalized_vol_test.mrc')

def crop():
    volumeViewer = TestSuite.window.windowManager.volumeViewer
    cropper      = volumeViewer.cropper
    
    volumeViewer.loadDataFromFile(pathname +'densityMap.mrc')
    cropper.viewer.renderer.cropVolume(20, 20, 20, 60, 60, 60)
    volumeViewer.renderer.saveFile(pathname +'cropped_vol_test.mrc')

def downsample():
    volumeViewer = TestSuite.window.windowManager.volumeViewer
    
    volumeViewer.loadDataFromFile(pathname +'densityMap.mrc')
    volumeViewer.renderer.downsampleVolume()
    volumeViewer.renderer.saveFile(pathname +'downsampled_vol_test.mrc')

def lapl():
    volumeViewer      = TestSuite.window.windowManager.volumeViewer
    laplacianSmoother = volumeViewer.laplacianSmoother
    
    volumeViewer.loadDataFromFile(pathname +'densityMap.mrc')
    
    v1 = laplacianSmoother.ui.doubleSpinBoxConvergenceRate.value()
    v2 = laplacianSmoother.ui.spinBoxNoOfIterations.value()
    laplacianSmoother.viewer.renderer.performSmoothLaplacian(v1, v2)
    
    volumeViewer.renderer.saveFile(pathname +'laplacian_smoothed_vol_test.mrc')
