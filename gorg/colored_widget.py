from PyQt4 import QtCore, QtGui

class ColoredWidget(QtGui.QWidget):
    
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.brush = QtGui.QBrush()
        self.brush.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(128, 128, 128, 255)))
        self.brush.setStyle(QtCore.Qt.SolidPattern)
        
    def paintEvent(self, event):
        painter = QtGui.QPainter()
        painter.begin(self)
        painter.fillRect(0, 0, self.width()-1, self.height()-1, self.brush)
        painter.end()
    
    def setColor(self, color):
        self.brush.setColor(color)
        self.update()

    
