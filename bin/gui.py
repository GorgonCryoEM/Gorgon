#!/usr/bin/env python

# import libpygorgon
import gorg

import os, sys

pathname = os.path.abspath(gorg.__path__[0]+'../../../../groel-demo')
print pathname

w = gorg.window
w.show()
w.raise_()

volumeViewer = gorg.window.windowManager.volumeViewer
volumeViewer.loadDataFromFile(pathname +'/densityMap.mrc')

gorg.app.exec_()
