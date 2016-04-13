from PyQt4.QtGui import *


class InfoSize(QFrame):

    def __init__(self, parent=None):
        super(InfoSize, self).__init__(parent)
        
        self.setFrameShape(QFrame.Box)
        
        self.labels = [QLabel(" "), QLabel("X"), QLabel("Y"), QLabel("Z")]
        hbox = QHBoxLayout()
        
        for l in self.labels:
            hbox.addWidget(l)
        
        self.setLayout(hbox)
