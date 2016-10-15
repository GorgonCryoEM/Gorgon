import os

if os.getenv('CONDA_BUILD_STATE') != 'TEST':
    EXE_PATH = 'cli'
else:
    EXE_PATH = None
