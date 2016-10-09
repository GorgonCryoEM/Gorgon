
def setAtomColorsAndVisibility(self, displayStyle):
    if displayStyle == self.DisplayStyleBackbone:
        self.setAllAtomColor(self.getAtomColor())
    elif displayStyle == self.DisplayStyleRibbon:
        self.renderer.cleanSecondaryStructures()
        self.ribbonMouseMapping = {}
        self.ribbonMouseMapping[0] = {}
        self.ribbonMouseMapping[1] = {}
        self.ribbonMouseMapping[2] = {}
        
        for chain in self.loadedChains:
            for helixIx, helix in chain.helices.items():
                ix = self.renderer.startHelix()
                self.ribbonMouseMapping[0][ix] = helixIx
                for i in range(helix.startIndex, helix.stopIndex + 1):
                    if (i in chain.residueList) and ("CA" in chain[i].getAtomNames()):
                        CAatom = chain[i].getAtom("CA")
                        self.renderer.addHelixElement(ix, chain[i].getAtom("CA").getHashKey())
                        resNumTemp = i - 1
                        if resNumTemp > 0 and resNumTemp in chain.residueList:
                            prevCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                            while not prevCAAtom and resNumTemp >= 0:
                                prevCAAtom = helix.chain[resNumTemp].getAtom('CA')
                                resNumTemp = resNumTemp - 1
                            if prevCAAtom:
                                CAatom.setPrevCAHash(prevCAAtom.getHashKey())
                                if resNumTemp < helix.startIndex:  # and [k for k,v in chain.secelList.items() if v == helix][0] ==
                                    for sheet in chain.sheets.values():
                                        for strand in sheet.strandList.values():
                                            if strand.startIndex <= resNumTemp <= strand.stopIndex:
                                                jx = self.renderer.startLoop()
                                                self.renderer.addLoopElement(jx, prevCAAtom.getHashKey())
                                                self.renderer.addLoopElement(jx, CAatom.getHashKey())
                                    for temphelix in chain.helices.values():
                                        if temphelix != helix and temphelix.startIndex <= resNumTemp <= temphelix.stopIndex:
                                            jx = self.renderer.startLoop()
                                            self.renderer.addLoopElement(jx, prevCAAtom.getHashKey())
                                            self.renderer.addLoopElement(jx, CAatom.getHashKey())
                            else:
                                CAatom.setPrevCAHash(CAatom.getHashKey())
                            resNumTemp = i + 1
                            if resNumTemp <= chain.getLastResidueIndex() and resNumTemp in chain.residueList:
                                nextCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                while not nextCAAtom and resNumTemp <= chain.getLastResidueIndex():
                                    nextCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                    resNumTemp = resNumTemp + 1
                                if nextCAAtom:
                                    CAatom.setNextCAHash(nextCAAtom.getHashKey())
                                else:
                                    CAatom.setNextCAHash(CAatom.getHashKey())
                    else:
                        if (i in chain.residueList) and not ("CA" in chain[i].getAtomNames()):
                            print "chain[i] did not contain a CA atom"
                            print "i:", i
                            print "chain[i]:", chain[i]
            
            for sheetIx, sheet in chain.sheets.items():
                for strandIx, strand in sheet.strandList.items():
                    ix = self.renderer.startStrand()
                    self.ribbonMouseMapping[1][ix] = [sheetIx, strandIx]
                    for i in range(strand.startIndex, strand.stopIndex + 1):
                        if (i in chain.residueList) and ("CA" in chain[i].getAtomNames()):
                            CAatom = chain[i].getAtom("CA")
                            self.renderer.addStrandElement(ix, chain[i].getAtom("CA").getHashKey())
                            resNumTemp = i - 1
                            if resNumTemp > 0:
                                prevCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                while not prevCAAtom and resNumTemp >= 0:
                                    prevCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                    resNumTemp = resNumTemp - 1
                                if prevCAAtom:
                                    CAatom.setPrevCAHash(prevCAAtom.getHashKey())
                                    if resNumTemp < strand.startIndex:
                                        for helix in chain.helices.values():
                                            if helix.startIndex <= resNumTemp <= helix.stopIndex:
                                                jx = self.renderer.startLoop()
                                                self.renderer.addLoopElement(jx, prevCAAtom.getHashKey())
                                                self.renderer.addLoopElement(jx, CAatom.getHashKey())
                                        for tempsheet in chain.sheets.values():
                                            for tempstrand in tempsheet.strandList.values():
                                                if tempstrand != strand and tempstrand.startIndex <= resNumTemp <= tempstrand.stopIndex:
                                                    jx = self.renderer.startLoop()
                                                    self.renderer.addLoopElement(jx, prevCAAtom.getHashKey())
                                                    self.renderer.addLoopElement(jx, CAatom.getHashKey())
                            else: # if CAatom is in the first residue, set it as its own previous (hacky)
                                CAatom.setPrevCAHash(CAatom.getHashKey())
                            # set the atom's "next" CA atom
                            resNumTemp = i + 1
                            if resNumTemp <= chain.getLastResidueIndex():
                                nextCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                while not nextCAAtom and resNumTemp <= chain.getLastResidueIndex():
                                    nextCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                    resNumTemp = resNumTemp + 1
                                if nextCAAtom:
                                    CAatom.setNextCAHash(nextCAAtom.getHashKey())
                            else: # if CAatom is in the last residue, set it as its own next (hacky)
                                CAatom.setNextCAHash(CAatom.getHashKey())
                        else:
                            if (i in chain.residueList) and not ("CA" in chain[i].getAtomNames()):
                                print "chain[i] did not contain a CA atom"
                                print "i:", i
                                print "chain[i]:", chain[i]
            for loopIx, loop in chain.coils.items():
                ix = self.renderer.startLoop()
                atomcounter = 0
                self.ribbonMouseMapping[2][ix] = loopIx
                for i in range(loop.startIndex - 1, loop.stopIndex + 2):
                    if (i in chain.residueList) and ("CA" in chain[i].getAtomNames()):
                        CAatom = chain[i].getAtom("CA")
                        atomcounter += 1
                        self.renderer.addLoopElement(ix, chain[i].getAtom("CA").getHashKey())
                        # set the atom's "previous" CA atom
                        resNumTemp = i - 1
                        if resNumTemp > 0 and resNumTemp in chain.residueList:
                            prevCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                            while not prevCAAtom and resNumTemp >= 0:
                                prevCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                resNumTemp = resNumTemp - 1
                            if prevCAAtom:
                                CAatom.setPrevCAHash(prevCAAtom.getHashKey())
                        else: # if CAatom is in the first residue, set it as its own previous (hacky)
                            CAatom.setPrevCAHash(CAatom.getHashKey())
                        # set the atom's "next" CA atom
                        resNumTemp = i + 1
                        if resNumTemp <= chain.getLastResidueIndex() and resNumTemp in chain.residueList:
                            nextCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                            while not nextCAAtom and resNumTemp <= chain.getLastResidueIndex():
                                nextCAAtom = chain.residueList[resNumTemp].getAtom('CA')
                                resNumTemp = resNumTemp + 1
                            if nextCAAtom:
                                CAatom.setNextCAHash(nextCAAtom.getHashKey())
                        else: # if CAatom is in the last residue, set it as its own next (hacky)
                            CAatom.setNextCAHash(CAatom.getHashKey())
                    else:
                        if (i in chain.residueList) and not ("CA" in chain[i].getAtomNames()):
                            print "in loops"
                            print "chain[i] did not contain a CA atom"
                            print "i:", i
                            print "chain[i]:", chain[i]
                            #print "in loop", ix, ", added", atomcounter, "atoms"
    
    elif displayStyle == self.DisplayStyleSideChain:
        self.setSpecificAtomColor('C', self.getCarbonColor())
        self.setSpecificAtomColor('N', self.getNitrogenColor())
        self.setSpecificAtomColor('O', self.getOxygenColor())
        self.setSpecificAtomColor('S', self.getSulphurColor())
    else:
        pass
    
    for chain in self.loadedChains:
        # Setting visibility of SSE atoms
        for i, secel in chain.secelList.items():
            if i in chain.residueList:
                for atomName in chain[i].getAtomNames():
                    atom = chain[i].getAtom(atomName)
                    if atom:
                        atom.setVisible((secel.type == "helix" and self.helicesVisible) or (
                            secel.type == "strand" and self.strandsVisible)
                                        or (secel.type == "loop" and self.loopsVisible))

def setAtomVisibility(self, visible):
    self.atomsVisible = visible
    self.repaintCamera()

def setBondVisibility(self, visible):
    self.bondsVisible = visible
    self.repaintCamera()

def setHelixVisibility(self, visible):
    self.helicesVisible = visible
    self.setAtomColorsAndVisibility(self.displayStyle)
    self.modelChanged()

def setLoopVisibility(self, visible):
    self.loopsVisible = visible
    self.setAtomColorsAndVisibility(self.displayStyle)
    self.modelChanged()

def setStrandVisibility(self, visible):
    self.strandsVisible = visible
    self.setAtomColorsAndVisibility(self.displayStyle)
    self.modelChanged()

def setAllAtomColor(self, color):
    for chain in self.loadedChains:
        for i in chain.residueRange():
            if i in chain.residueList:
                for atomName in chain[i].getAtomNames():
                    atom = chain[i].getAtom(atomName)
                    if atom:
                        atom.setColor(color.redF(), color.greenF(), color.blueF(), color.alphaF())

def setSpecificAtomColor(self, molecule, color):
    for chain in self.loadedChains:
        for i in chain.residueRange():
            if i in chain.residueList:
                for atomName in chain[i].getAtomNames():
                    atom = chain[i].getAtom(atomName)
                    if atomName[0] == molecule[0]:
                        atom.setColor(color.redF(), color.greenF(), color.blueF(), color.alphaF())

def setAtomColor(self, color):
    self.app.themes.addColor(self.title + ":" + "Atom", color)
    self.setAllAtomColor(color)
    self.modelChanged()

def setBondColor(self, color):
    oldColor = self.getBondColor()
    self.app.themes.addColor(self.title + ":" + "Bond", color)
    self.repaintCamera2(oldColor, color)

def setHelixColor(self, color):
    oldColor = self.getHelixColor()
    self.app.themes.addColor(self.title + ":" + "Helix", color)
    self.repaintCamera2(oldColor, color)

def setStrandColor(self, color):
    oldColor = self.getStrandColor()
    self.app.themes.addColor(self.title + ":" + "Strand", color)
    self.repaintCamera2(oldColor, color)

def setLoopColor(self, color):
    oldColor = self.getLoopColor()
    self.app.themes.addColor(self.title + ":" + "Loop", color)
    self.repaintCamera2(oldColor, color)

def setCarbonColor(self, color):
    self.app.themes.addColor(self.title + ":" + "Carbon", color)
    self.setSpecificAtomColor('C', color)
    self.modelChanged()

def setNitrogenColor(self, color):
    self.app.themes.addColor(self.title + ":" + "Nitrogen", color)
    self.setSpecificAtomColor('N', color)
    self.modelChanged()

def setOxygenColor(self, color):
    self.app.themes.addColor(self.title + ":" + "Oxygen", color)
    self.setSpecificAtomColor('O', color)
    self.modelChanged()

def setSulphurColor(self, color):
    self.app.themes.addColor(self.title + ":" + "Sulphur", color)
    self.setSpecificAtomColor('S', color)
    self.modelChanged()

def getAtomColor(self):
    return self.colors[self.title + ":" + "Atom"]

def getBondColor(self):
    return self.colors[self.title + ":" + "Bond"]

def getHelixColor(self):
    return self.colors[self.title + ":" + "Helix"]

def getStrandColor(self):
    return self.colors[self.title + ":" + "Strand"]

def getLoopColor(self):
    return self.colors[self.title + ":" + "Loop"]

def getCarbonColor(self):
    return self.colors[self.title + ":" + "Carbon"]

def getNitrogenColor(self):
    return self.colors[self.title + ":" + "Nitrogen"]

def getOxygenColor(self):
    return self.colors[self.title + ":" + "Oxygen"]

def getSulphurColor(self):
    return self.colors[self.title + ":" + "Sulphur"]
