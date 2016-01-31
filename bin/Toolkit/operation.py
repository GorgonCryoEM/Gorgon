#!/usr/bin/env python

import logging
from libpycore import *


class Operation(object):

    def __init__(self, input, output):
        self.input = input
        self.output = output
        self.logger = logging.getLogger(__name__)
        
        self.renderer = Volume()
            
        self._loadVolume()
        self.run()

    def _loadVolume(self):
        self.logger.debug(__file__)
        self.logger.debug("Operation._loadVolume")
        self.logger.debug(self.renderer)
        self.renderer.loadFile(self.input)
        self.logger.debug(self.renderer)
        self.logger.debug(self.renderer.getSize())
        
    def _saveVolume(self):
        self.renderer.saveFile(self.output)
        
    def run(self):
        self.logger.debug("Operation.run")
        self.logger.debug(self.renderer)
        self.logger.debug(self.renderer.getSize())
        self._run()
        self.logger.debug("Operation.run")
        self.logger.debug(self.renderer.getSize())
        self._saveVolume()
