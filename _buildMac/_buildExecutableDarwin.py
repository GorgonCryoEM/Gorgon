
from setuptools import setup
import py2app.recipes


setup(
    app=['gorgon.pyw'],
    data_files=[('', ['splash.png', 'gorgon.icns']), 
                ('../../../themes', ['../resources/Dark.thm', '../resources/Light.thm'] )],
    options={
             'py2app': {
                        'iconfile' : 'gorgon.icns',
                        'packages' : [],
                        "includes" : ['sip', 'PyQt4'],
                        "excludes" : [],                        
                        }
             }
)
