
# Overridden
def getDrawColors(self):
    if (self.displayStyle == self.DisplayStyleBackbone):
        colors = [self.getAtomColor(), self.getBondColor(), self.getBondColor()]
    elif (self.displayStyle == self.DisplayStyleRibbon):
        colors = [self.getHelixColor(), self.getStrandColor(), self.getLoopColor()]
    elif (self.displayStyle == self.DisplayStyleSideChain):
        colors = [self.getAtomColor(), self.getBondColor(), self.getAtomColor()]
    else:
        colors = [None, None, None]
    return colors

# Overridden
def getDrawVisibility(self):
    if (self.displayStyle == self.DisplayStyleBackbone):
        visibility = [self.atomsVisible, self.bondsVisible, self.bondsVisible and (not self.atomsVisible)]
    elif (self.displayStyle == self.DisplayStyleRibbon):
        visibility = [self.helicesVisible, self.strandsVisible, self.loopsVisible]
    elif (self.displayStyle == self.DisplayStyleSideChain):
        visibility = [self.atomsVisible, self.bondsVisible, self.bondsVisible and (not self.atomsVisible)]
    else:
        visibility = [False, False, False]
    visibility = [True, True, True]
    return visibility
