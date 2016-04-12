from PyQt4.QtGui import *
from PyQt4.QtCore import pyqtSignal


class Widget3(QFrame):

    def __init__(self, parent=None):
        super(Widget3, self).__init__(parent)
        
        self.widgets = [QSpinBox(), QSpinBox(), QSpinBox()]
        
        hbox = QHBoxLayout()
        hbox.addStretch(1)
        for w in self.widgets:
            hbox.addWidget(w)

        self.setLayout(hbox)
        
#         for w in self.widgets:
#             w.

    def show(self):
        for w in self.widgets:
            w.show()
