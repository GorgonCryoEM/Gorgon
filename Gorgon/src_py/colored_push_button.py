# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: A pushbutton which instead of displaying text, displays a color

from PyQt4 import QtCore, QtGui

class ColoredPushButton(QtGui.QPushButton):
    
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.brush = QtGui.QBrush()
        self.brush.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(128, 128, 128, 255)))
        self.brush.setStyle(QtCore.Qt.SolidPattern)
        
    def paintEvent(self, event):
        QtGui.QPushButton.paintEvent(self, event)
        painter = QtGui.QPainter()
        painter.begin(self)
        painter.fillRect(5, 4, self.width()-10, self.height()-8, self.brush)
        painter.end()
    
    def setColor(self, color):
        self.brush.setColor(color)
        self.update()

    