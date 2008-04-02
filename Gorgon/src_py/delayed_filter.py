from PyQt4 import QtGui, QtCore

class DelayedFilter(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.delay = 200
        self.waiting = False
        self.value = 0
        self.timer = QtCore.QTimer(self)
        self.connect(self.timer, QtCore.SIGNAL("timeout()"), self.emitValue)
    
    def setValue(self, value):
        self.value = value
        if not self.waiting:
            self.timer.start(self.delay)
            self.waiting = True

    def emitValue(self):
        self.timer.stop()
        self.emit(QtCore.SIGNAL("valueChanged(int)"), self.value)
        self.waiting = False
