# __all__=[]

import sys, os, inspect

from PyQt4 import QtGui, QtCore
from main_window_form import MainWindowForm

# import time

# if __name__ == '__main__':
gorgonVersion = '2.2.2'
app = QtGui.QApplication(sys.argv)    
app.processEvents()

window = MainWindowForm(gorgonVersion)
# window.isNightlyBuild = True
# window.showMaximized()
# window.loadPlugins()
#     app.exec_()
