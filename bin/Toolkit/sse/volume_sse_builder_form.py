import logging
from libpycore import *
from sse_hunter_engine import SSEHunterEngine
from termcolor import colored


class VolumeSSEBuilderForm(object):

    def __init__(self, volume, skeleton, output):
        self.logger = logging.getLogger(__name__)
        
        self.volume = Volume()
        self.volume.loadFile(volume)
        self.logger.debug("%s before getVolume" % self.volume)
        self.logger.debug("self.volume.getSize(): %d" % self.volume.getSize())
#         self.volume = self.volume.getVolume()
        self.logger.debug("%s after getVolume" % self.volume)
        self.logger.debug("self.volume.getSize(): %d" % self.volume.getSize())
#         exit()
        
        self.skeleton = MeshRenderer()
        self.logger.debug(self.volume)
        self.logger.debug("self.volume.getSize(): %d" % self.volume.getSize())
        self.logger.debug(self.skeleton)
        self.logger.debug("after MeshRenderer(): self.skeleton.getSize(): %d" % self.skeleton.getSize())
        
        self.skeleton.loadFile(skeleton)
        self.logger.debug(self.skeleton)
        self.logger.debug(colored("after loadFile(skeleton): self.skeleton.getSize(): %d" % self.skeleton.getSize(), "yellow"))
        self.skeleton = self.skeleton.getMesh()
        self.logger.debug(self.skeleton)
        self.logger.debug(colored("after skeleton.getMesh(): self.skeleton.getSize(): %d" % self.skeleton.getSize(), "cyan"))

        self.output = output
        self.calphaRenderer = CAlphaRenderer()
        self.runSSEHunter()
        self.savePseudoatoms()

    def savePseudoatoms(self):
        self.calphaRenderer.saveSSEHunterFile(self.output)
    
    def runSSEHunter(self):
        self.logger.debug(self.volume)
        self.logger.debug("self.volume.getSize(): %d" % self.volume.getSize())
        self.logger.debug(self.skeleton)
        self.logger.debug("self.skeleton.getSize(): %d" % self.skeleton.getSize())

        threshold = 0.38
        resolution = 8.0
        correlationWeight = 1.0
        skeletonWeight = 1.0
        geometryWeight = 1.0

        #self.calphaViewer.runSSEHunter( threshold, resolution, correlationWeight, skeletonWeight, geometryWeight )
        self.logger.debug(self.volume)
        self.logger.debug("self.volume.getSize(): %d" % self.volume.getSize())
        sseh = SSEHunterEngine(self.volume, self.skeleton, resolution, threshold)
        patoms = sseh.getScoredAtoms(correlationWeight, skeletonWeight, geometryWeight)
        
        for pseudoatom in patoms:
            self.calphaRenderer.addAtom(pseudoatom)
