
from setuptools import setup
import py2app.recipes


setup(
    app=['gorgon.pyw'],
    version="2.2.0",
    data_files=[('', ['../design/splash.png', '../design/gorgon.icns']), 
                ('../../../themes', ['../resources/Dark.thm', '../resources/Light.thm'] )],
    options={
             'py2app': {
                        'iconfile' : '../design/gorgon.icns',
                        'packages' : [],
                        "includes" : ['sip', 'PyQt4'],
                        "excludes" : [],                        
                        }
             }
)
