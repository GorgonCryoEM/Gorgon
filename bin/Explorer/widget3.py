from PyQt4.QtGui import QWidget, QPushButton, QPainter, QColorDialog, QColor, QSpinBox, QLayout
from PyQt4.QtCore import pyqtSignal


class Widget3(QSpinBox):

    def __init__(self, widget, parent=None):
        super(Widget3, self).__init__(parent)
        
        self.widgets = [QSpinBox(), QSpinBox(), QSpinBox()]
        
        hbox = QtGui.QHBoxLayout()
#         hbox.addStretch(1)
        for w in self.widgets:
            hbox.addWidget(w)
        
        self.setLayout(hbox)
