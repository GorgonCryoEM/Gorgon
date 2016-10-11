class Match:
    # A forward match aligns the N-terminus with lesser vertex, and C-terminus with greater vertex
    #    lesser vertex is vertex closer to 0,0,0
    FORWARD = 0
    REVERSE = 1

    def __init__(self, observed, predicted, direction):
        self.observed = observed
        self.predicted = predicted
        self.direction = direction
        self.constrained = False
        self.directionConstrained = False

    def __repr__(self):
        if self.observed is None:
            return 0
        elif self.direction is self.FORWARD:
            return str(self.observed.label).lower()
        else:
            return str(self.observed.label).capitalize()
