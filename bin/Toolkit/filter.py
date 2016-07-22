from .operation import Operation


class Filter(Operation):

    def __init__(self, input, output):
        super(Filter, self).__init__(input, output)


class Normalize(Filter):

    def __init__(self, input, output, args):
        super(Normalize, self).__init__(input, output)

    def _run(self):
        self.renderer.normalizeVolume()


class LowPass(Filter):

    def __init__(self, input, output, args):
        super(LowPass, self).__init__(input, output)
        
    def _run(self):
        print "\033[31m""Sorry, NOT implemented!""\033[0m"
        

class Gaussian(Filter):

    def __init__(self, input, output, args):
        super(Gaussian, self).__init__(input, output)
        
    def _run(self):
        print "\033[31m""Sorry, NOT implemented!""\033[0m"
