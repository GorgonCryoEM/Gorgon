"""
py2app/py2exe build script for gorgon.pyw.

Will automatically ensure that all build prerequisites are available
via ez_setup

Usage (Mac OS X):
    python setup.py py2app

Usage (Windows):
    python setup.py py2exe
"""
import ez_setup
ez_setup.use_setuptools()

import sys
from setuptools import setup

# --------
APP = ['gorgon.pyw']
DATA_FILES = [('', ['splash.png', 'gorgon.icns']),
	('../../../themes', ['../resources/Dark.thm', '../resources/Light.thm'] )]
OPTIONS = {'argv_emulation': True,
	   'iconfile' : 'gorgon.icns',
	   'packages' : [],
	   "includes" : ['sip', 'PyQt4'],
	   "excludes" : []
	   }
mainscript = 'gorgon.pyw'
# --------

if sys.platform == 'darwin':
    extra_options = dict(
        setup_requires=['py2app'],
        app=[mainscript],
        # Cross-platform applications generally expect sys.argv to
        # be used for opening files.
        options=dict(py2app=dict(argv_emulation=True)),
    )
elif sys.platform == 'win32':
    extra_options = dict(
        setup_requires=['py2exe'],
        app=[mainscript],
    )
else:
     extra_options = dict(
         # Normally unix-like platforms will use "setup.py install"
         # and install the main script as such
         scripts=[mainscript],
     )

setup(
# --------
    app=APP,
    data_files=DATA_FILES,
    options={'py2app': OPTIONS},
    setup_requires=['py2app'],
# --------
    name="Gorgon",
    **extra_options
)