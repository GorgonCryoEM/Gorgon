
from setuptools import setup
import py2app.recipes


setup(
    app=['gorgon.pyw'],
    data_files=['splash.png', 'gorgon.ico', 'gorgon.icns'],
    options={
             'py2app': {
                        'iconfile' : 'gorgon.icns',
                        'packages' : [],
                        "includes" : ['sip', 'PyQt4'],
                        "excludes" : [],                        
                        }
             }
)
