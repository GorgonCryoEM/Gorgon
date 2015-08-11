import TestSuite

pathname = TestSuite.__path__[0]+'/skeleton/'

skeletonViewer = TestSuite.window.windowManager.skeletonViewer

def laplacian_smooth():
    skeletonViewer.loadDataFromFile(pathname +'densityMap-skeleton.mrc')
    
    skeletonLaplacian = skeletonViewer.optionsWindow
    action = skeletonLaplacian.app.actions.getAction("perform_SkeletonLaplacianSmoothing")
    action.trigger()
    
    skeletonLaplacian.accept()
    
    skeletonViewer.renderer.saveFile(pathname +'laplacian_smoothed_skeleton_test.mrc')
