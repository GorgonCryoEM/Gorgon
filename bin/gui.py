#!/usr/bin/env python

# import libpygorgon
import gorg

import os, sys

pathname = os.path.abspath(gorg.__path__[0]+'/../../../../groel-demo')
print pathname

gorg.window.show()

volumeViewer = gorg.window.windowManager.volumeViewer
volumeViewer.loadDataFromFile(pathname +'/densityMap.mrc')
