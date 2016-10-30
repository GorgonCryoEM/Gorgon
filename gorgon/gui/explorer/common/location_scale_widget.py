from PyQt4.QtGui import *
from .spinbox3 import SpinBox3
from .labels_xyz import LabelsXYZ


class LocationScaleWidget(QFrame):

    def __init__(self, parent=None):
        super(LocationScaleWidget, self).__init__(parent)
        
        self.setFrameShape(QFrame.Box)
        
        self.loc = SpinBox3("Location")
        self.scale = SpinBox3("Scale")
        
        vbox = QVBoxLayout()
        vbox.addWidget(LabelsXYZ())
        vbox.addWidget(self.loc)
        vbox.addWidget(self.scale)
        
        vbox.setSpacing(0)
        vbox.setContentsMargins(0,0,0,0)
        
        self.setLayout(vbox)
        
        self.loc.valueChanged.connect(self.valueChanged)
        self.scale.valueChanged.connect(self.valueChanged)
        
    def valueChanged(self):
        self.loc.locationChanged()
        self.scale.locationChanged()
