import sys, os

sys.path.append(os.path.abspath('.'))

import TestSuite
from  TestSuite import *

volume1.normalize()
volume1.crop()
volume1.downsample()
volume1.lapl()