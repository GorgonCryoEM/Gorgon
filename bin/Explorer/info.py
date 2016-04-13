from PyQt4.QtGui import *
from .widget3 import Widget3


class Info(QFrame):

    def __init__(self, parent=None):
        super(Info, self).__init__(parent)
        
        self.setFrameShape(QFrame.Box)
        
        self.loc = Widget3("Location")
        self.scale = Widget3("Scale")
        
        vbox = QVBoxLayout()
        vbox.addWidget(self.loc)
        vbox.addWidget(self.scale)
        
        self.setLayout(vbox)
        
        self.loc.valueChanged.connect(self.valueChanged)
        self.scale.valueChanged.connect(self.valueChanged)
        
    def valueChanged(self):
#         print val
        self.loc.locationChanged()
        self.scale.locationChanged()