from PyQt4.QtGui import *
from PyQt4.QtCore import pyqtSignal


class Widget3(QFrame):
    
    valueChanged = pyqtSignal()

    def __init__(self, lbl, parent=None):
        super(Widget3, self).__init__(parent)
        
#         self.setFrameShape(QFrame.Box)
        
        self.lbl = QLabel(lbl)
        self.widgets = [QDoubleSpinBox() for i in range(3)]
        
        hbox = QHBoxLayout()
#         hbox.addStretch(1)
        hbox.addWidget(self.lbl)
        for w in self.widgets:
            w.setMaximum(1000)
            hbox.addWidget(w)

        self.setLayout(hbox)
        
        self.setupSignals()
        
    def setupSignals(self):
        for w in self.widgets:
            w.valueChanged.connect(self.valueChanged)
            
    def emitLoc(self):
        self.valueChanged.emit(self.getLocation())

    def getLocation(self):
        return [self.widgets[i].value() for i in range(len(self.widgets))]

    def locationChanged(self):
#         print "Location changed: ", self.sender()
        print "%s: %s" % (self.lbl.text(), self.getLocation())
#         for w in self.widgets:
#             w.

#     def show(self):
#         for w in self.widgets:
#             w.show()
