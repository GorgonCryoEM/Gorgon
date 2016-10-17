import argparse
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

    def __init__(self, input, output, args=None):
        parser = argparse.ArgumentParser()
        parser.add_argument("--thresh")
        parser.add_argument("--min_curve_length", default=4)
        parser.add_argument("--min_surface_size", default=4)

        self.args = parser.parse_args(args)
        
        super(Binary, self).__init__(input, output)

    def _run(self):
        self.logger.debug(__file__)
        self.logger.debug("Binary._run")
        self.logger.debug(self.renderer)
        self.logger.debug("renderer.getSize(): %d" % self.renderer.getSize())
        thresh = float(self.args.thresh) if self.args.thresh else self.defaultDensity()
        self.skeleton = self.renderer.performBinarySkeletonizationJu2007(thresh, self.args.min_curve_length, self.args.min_surface_size)
        self.logger.debug(self.renderer)
        self.logger.debug(self.skeleton)
        self.logger.debug("skeleton.getSize(): %d" % self.skeleton.getSize())
        

class GrayScale(Skeletonizer):

    def __init__(self, input, output, args=None):
        super(GrayScale, self).__init__(input, output)
        self.parser.add_argument("--thresh")
        self.parser.add_argument("--min_curve_length", default=4)
        self.parser.add_argument("--min_surface_size", default=4)
        self.parser.add_argument("--step_count", default=250)
        self.parser.add_argument("--curve_radius", default=2)
        self.parser.add_argument("--surface_radius", default=2)
        self.parser.add_argument("--skeleton_radius", default=4)

    def _run(self):
        self.skeleton = self.renderer.performGrayscaleSkeletonizationAbeysinghe2008(self.defaultDensity(), 250, 4, 4, 2, 2, 4)
        

