# __all__=[]

import sys, os, inspect

# topdir = os.path.abspath(inspect.getfile(inspect.currentframe()))
# topdir = os.path.join(os.path.dirname(topdir),os.pardir)
# topdir = os.path.abspath(topdir)
# 
# gorgon_path = sys.path.append(os.path.join(topdir,'Gorgon','src_py'))
# sys.path.append(gorgon_path)

from PyQt4 import QtGui, QtCore
from main_window_form import MainWindowForm

# import time

# if __name__ == '__main__':
gorgonVersion = '2.2.2'
app = QtGui.QApplication(sys.argv)    
#setting different font sizes for different OSs
# if(sys.platform == 'win32'):
#     pass
# elif(sys.platform == 'darwin'):
#     app.setStyleSheet("* { font-size: 10px }")
# else :
#     app.setStyleSheet("* { font-size: 10px }")
    
# pixmap = QtGui.QPixmap(os.path.join(topdir, "Gorgon", "design","splash.png"))
# splash = QtGui.QSplashScreen(pixmap, QtCore.Qt.WindowStaysOnTopHint)
# splash.setMask(pixmap.mask())
# splash.show()
app.processEvents()

window = MainWindowForm(gorgonVersion)
# window.isNightlyBuild = True
# window.showMaximized()
# splash.finish(window)
# window.loadPlugins()
#     app.exec_()
