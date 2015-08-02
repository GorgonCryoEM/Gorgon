from distutils.core import setup
import py2exe

setup(
      name = "Gorgon",
      version = "2.0.0",
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
                        'msvcp80.dll',
                        'libfftw3-3.dll',
                        'libfftw3f-3.dll',
                        'libfftw3l-3.dll' ]), 
                  ('themes', ['..\\resources\\Dark.thm', '..\\resources\\Light.thm'] ),
                  ('plugins', ['.\\plugins\\__init__.py', '.\\plugins\\update_notifier_plugin.py'])                                 
                  ],
      options={
               "py2exe":{
                         "includes" : ["sip", "PyQt4.Qt", "ctypes", "distutils.util", "logging", "ctypes.util", "weakref"],
                         "excludes" : ["OpenGL"]
                         }
               }
      )
