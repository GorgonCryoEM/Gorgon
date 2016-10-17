#!/usr/bin/env python

import logging
from libpytoolkit import *


class Operation(object):

    def __init__(self, parser):
        
        self.parser = parser
        self._add_parser_arguments()

        self.logger = logging.getLogger(__name__)

        self.renderer = Volume()

    def _loadVolume(self, input):
        self.logger.debug(__file__)
        self.logger.debug("Operation._loadVolume")
        self.logger.debug(self.renderer)
        self.renderer.loadFile(input)
        self.logger.debug(self.renderer)
        self.logger.debug(self.renderer.getSize())
        
    def _saveVolume(self, output):
        self.renderer.saveFile(output)
        
    def run(self, args):
        self._loadVolume(args.input)
        self.logger.debug("Operation.run")
        self.logger.debug(self.renderer)
        self.logger.debug(self.renderer.getSize())
        self._run(args)
        self.logger.debug("Operation.run")
        self.logger.debug(self.renderer.getSize())
        self._saveVolume(args.output)
