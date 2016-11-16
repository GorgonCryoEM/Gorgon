import argparse
from .operation import Operation


class Skeletonizer(Operation):

    def __init__(self, parser):
        super(Skeletonizer, self).__init__(parser)

    def _add_parser_arguments(self):
        self.parser.add_argument("--thresh", type=float)
        self.parser.add_argument("--min_curve_length", type=int, default=4)
        self.parser.add_argument("--min_surface_size", type=int, default=4)

    def _run(self, args):
        self.thresh = float(args.thresh) if args.thresh else self.defaultDensity()
        print "THRESHOLD: %f" % self.thresh
        self._run_derived(args)

    def _saveVolume(self, output):
        self.renderer.setVolume(self.skeleton)
        self.renderer.saveFile(output)
        
    def defaultDensity(self):
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()
        return (int(minDensity) + int(maxDensity)) / 2

        
class Binary(Skeletonizer):

    def __init__(self, parser):
        super(Binary, self).__init__(parser)

    def _run_derived(self, args):
        self.logger.debug(__file__)
        self.logger.debug("Binary._run")
        self.logger.debug(self.renderer)
        self.logger.debug("renderer.getSize(): %d" % self.renderer.getSize())
        self.skeleton = self.renderer.performBinarySkeletonizationJu2007(self.thresh, args.min_curve_length, args.min_surface_size)
        self.logger.debug(self.renderer)
        self.logger.debug(self.skeleton)
        self.logger.debug("skeleton.getSize(): %d" % self.skeleton.getSize())
        

class GrayScale(Skeletonizer):

    def __init__(self, parser):
        super(GrayScale, self).__init__(parser)

    def _add_parser_arguments(self):
        self.parser.add_argument("--step_count", type=int, default=250)
        self.parser.add_argument("--curve_radius", type=int, default=2)
        self.parser.add_argument("--surface_radius", type=int, default=2)
        self.parser.add_argument("--skeleton_radius", type=int, default=4)

    def _run_derived(self, args):
        self.skeleton = self.renderer.performGrayscaleSkeletonizationAbeysinghe2008(self.thresh, args.step_count, args.min_curve_length, args.min_surface_size, args.curve_radius, args.surface_radius, args.skeleton_radius)
