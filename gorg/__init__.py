# __all__=[]
__version__ = '2.2.3'

import sys, os, inspect

from PyQt4 import QtGui, QtCore
from main_window_form import MainWindowForm

# import time

# if __name__ == '__main__':
app = QtGui.QApplication(sys.argv)    
app.processEvents()

window = MainWindowForm(__version__)
# window.isNightlyBuild = True
# window.showMaximized()
# window.loadPlugins()
#     app.exec_()
