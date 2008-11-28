from distutils.core import setup
import py2exe

setup(
      name = "Gorgon",
      version = "1.0.0",
      description = "Gorgon: A toolkit for protein structure reconstruction",
      windows=['gorgon.pyw'],
      options={
               "py2exe":{
                         "includes" : ["sip", "PyQt4._qt", "ctypes", "distutils.util", "logging", "ctypes.util", "weakref"],
                         "excludes" : ["OpenGL"]
                         }
               }
      )


#  For PyOpengl 3.0.0b6
#"includes" : ["sip", "PyQt4._qt", "ctypes", "logging", "ctypes.util", "weakref"], 