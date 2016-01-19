import pkg_resources
from pkg_resources import DistributionNotFound, VersionConflict

import os
filepath = os.path.join(os.path.dirname(__file__), 'requirements.txt')

with open(filepath) as f:
    dependencies = f.readlines()

def test():
    pkg_resources.require(dependencies)
