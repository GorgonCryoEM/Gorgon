import os

if os.getenv('CONDA_BUILD_STATE') != 'TEST':
    EXE_PATH = 'cli'
else:
    EXE_PATH = None

print "EXE_PATH: %s" % EXE_PATH
