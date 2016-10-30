from PyQt4.QtGui import QWidget, QPushButton, QPainter, QColorDialog, QColor
from PyQt4.QtCore import pyqtSignal


class ColoredPushButton(QPushButton):
    
    valueChanged = pyqtSignal(QColor)

    def __init__(self, parent = None):
        QWidget.__init__(self, parent)
        self.color = QColor(128, 128, 128, 255)
        
        self.clicked.connect(self.buttonPressed)
        
    def paintEvent(self, event):
        painter = QPainter()
        painter.begin(self)
        painter.fillRect(5, 4, self.width()-10, self.height()-8, self.color)
        painter.end()
    
    def setColor(self, color):
        if(self.color != color):
            self.color = color
            self.update()
            self.valueChanged.emit(self.color)
    
    def color(self):
        return self.color
        
    def buttonPressed(self):
        color = QColorDialog().getColor(self.color, self, '', QColorDialog.ShowAlphaChannel)
        
        if color.isValid():
            self.setColor(color)
