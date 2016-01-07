#!/usr/bin/env python

from libpycore import *


class Operation(object):

    def __init__(self, input, output):
        self.input = input
        self.output = output
        
        self.renderer = VolumeRenderer()
            
        self._loadVolume()
        self.run()

    def _loadVolume(self):
        self.renderer.loadFile(self.input)
        
    def _saveVolume(self):
        self.renderer.saveFile(self.output)
        
    def run(self):
        self._run()
        self._saveVolume()
