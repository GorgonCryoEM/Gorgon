from .operation import Operation


class Skeletonizer(Operation):

    def __init__(self, input, output):
        super(Skeletonizer, self).__init__(input, output)
    
    def _saveVolume(self):
        self.renderer.setVolume(self.skeleton)
        self.renderer.saveFile(self.output)
        
    def defaultDensity(self):
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()
        return (int(minDensity) + int(maxDensity)) / 2

        
class Binary(Skeletonizer):

    def __init__(self, input, output, args):
        super(Binary, self).__init__(input, output)
        
    def _run(self):
        self.logger.debug(__file__)
        self.logger.debug("Binary._run")
        self.logger.debug(self.renderer)
        self.logger.debug("renderer.getSize(): %d" % self.renderer.getSize())
        self.skeleton = self.renderer.performBinarySkeletonizationJu2007(self.defaultDensity(), 4, 4)
        self.logger.debug(self.renderer)
        self.logger.debug(self.skeleton)
        self.logger.debug("skeleton.getSize(): %d" % self.skeleton.getSize())
        

class GrayScale(Skeletonizer):

    def __init__(self, input, output, args):
        super(GrayScale, self).__init__(input, output)

    def _run(self):
        self.skeleton = self.renderer.performGrayscaleSkeletonizationAbeysinghe2008(self.defaultDensity(), 250, 4, 4, 2, 2, 4)
        

