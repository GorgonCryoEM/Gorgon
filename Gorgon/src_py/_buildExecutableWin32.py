from distutils.core import setup
import py2exe

setup(
      name = "Gorgon",
      version = "1.0.0",
      description = "Gorgon: A toolkit for protein structure reconstruction",
      windows=[{
                'script': 'gorgon.pyw',
                'icon_resources': [(0, 'gorgon.ico')]
                }
               ],
      data_files=[('' ,[
                        'splash.png', 
                        'gorgon.ico',
                        'msvcr80.dll',
                        'msvcp80.dll']), 
                  ('themes', ['..\\resources\\Dark.thm', '..\\resources\\Light.thm'] ),
                  ('plugins', ['.\\plugins\__init__.py'])                  
                  ],
      options={
               "py2exe":{
                         "includes" : ["sip", "PyQt4._qt", "ctypes", "distutils.util", "logging", "ctypes.util", "weakref"],
                         "excludes" : ["OpenGL"]
                         }
               }
      )
