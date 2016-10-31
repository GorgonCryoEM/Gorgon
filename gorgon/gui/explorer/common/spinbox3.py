from PyQt4.QtGui import *
from PyQt4.QtCore import pyqtSignal


class SpinBox3(QFrame):
    
    valueChanged = pyqtSignal()

    def __init__(self, lbl, parent=None):
        super(SpinBox3, self).__init__(parent)
        
        self.lbl = QLabel(lbl)
        self.widgets = [QDoubleSpinBox() for i in range(3)]
        
        hbox = QHBoxLayout()
        hbox.addWidget(self.lbl)
        for w in self.widgets:
            w.setMaximum(1000)
            w.setMinimum(-1000)
            hbox.addWidget(w)

        hbox.setSpacing(0)
        hbox.setMargin(0)
        self.setLayout(hbox)
        
        self.setupSignals()
        
    def setupSignals(self):
        for w in self.widgets:
            w.valueChanged.connect(self.valueChanged)
