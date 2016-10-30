from PyQt4.QtGui import *
from .widget3 import Widget3
from .info_size import InfoSize


class Info(QFrame):

    def __init__(self, parent=None):
        super(Info, self).__init__(parent)
        
        self.setFrameShape(QFrame.Box)
        
        self.loc = Widget3("Location")
        self.scale = Widget3("Scale")
        
        vbox = QVBoxLayout()
        vbox.addWidget(InfoSize())
        vbox.addWidget(self.loc)
        vbox.addWidget(self.scale)
        
        vbox.setSpacing(0)
        print vbox.contentsMargins()
        vbox.setContentsMargins(0,0,0,0)
        
        self.setLayout(vbox)
        
        self.loc.valueChanged.connect(self.valueChanged)
        self.scale.valueChanged.connect(self.valueChanged)
        
    def valueChanged(self):
        self.loc.locationChanged()
        self.scale.locationChanged()
