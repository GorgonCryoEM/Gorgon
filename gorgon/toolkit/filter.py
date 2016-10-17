from .operation import Operation


class Filter(Operation):

    def __init__(self, parser):
        super(Filter, self).__init__(parser)


class Normalize(Filter):

    def __init__(self, parser):
        super(Normalize, self).__init__(parser)
    
    def _add_parser_arguments(self):
        pass

    def _run(self, args):
        self.renderer.normalizeVolume()


class LowPass(Filter):

    def __init__(self, parser):
        super(LowPass, self).__init__(parser)

    def _add_parser_arguments(self):
        pass
        
    def _run(self, args):
        print "\033[31m""Sorry, NOT implemented!""\033[0m"
        

class Gaussian(Filter):

    def __init__(self, parser):
        super(Gaussian, self).__init__(parser)

    def _add_parser_arguments(self):
        pass
        
    def _run(self, args):
        print "\033[31m""Sorry, NOT implemented!""\033[0m"
