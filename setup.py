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
import py2app.recipes

mainscript = 'gorgon.pyw'

DATA_FILES = [('', ['../design/splash.png', '../design/gorgon.icns']),
	('../../../themes', ['../resources/Dark.thm', '../resources/Light.thm'] )]
OPTIONS = {'argv_emulation': True,
	   'iconfile' : '../design/gorgon.icns',
	   'packages' : [],
	   "includes" : ['sip', 'PyQt4'],
	   "excludes" : []
	   }

if sys.platform == 'darwin':
    extra_options = dict(
        setup_requires=['py2app'],
        app=[mainscript],
        # Cross-platform applications generally expect sys.argv to
        # be used for opening files.
        options=dict(py2app=OPTIONS),
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
    data_files=DATA_FILES,
    name="Gorgon",
    version="2.2.2",
    **extra_options
)
