#!/usr/bin/env python

# import libpygorgon
import gorg

import os, sys

pathname = os.path.abspath(gorg.__path__[0]+'/../../demo/groel')
print pathname

w = gorg.window
w.show()
w.raise_()

volumeViewer = gorg.window.volumeViewer
volumeViewer.loadDataFromFile(pathname +'/densityMap.mrc')

gorg.app.exec_()
