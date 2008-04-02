from PyQt4 import QtGui

class SkeletonRenderer(QtGui.QWidget):
    """Class for holding Skeleton rendering code"""

    def __init__(self, main, back_end, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.back_end = back_end

    def draw(self):
        self.back_end.DrawSkeleton()
