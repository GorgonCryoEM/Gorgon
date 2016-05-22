from PyQt4 import QtCore, QtGui


class ColoredPushButton(QtGui.QPushButton):
    
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.actualColor = QtGui.QColor.fromRgba(QtGui.qRgba(128, 128, 128, 255))
        self.brush = QtGui.QBrush()
        self.brush.setColor(self.actualColor)
        self.brush.setStyle(QtCore.Qt.SolidPattern)
        
        self.connect(self, QtCore.SIGNAL("pressed ()"), self.buttonPressed)
        self.colorPicker = ColorPickerForm()
        
    def paintEvent(self, event):
        QtGui.QPushButton.paintEvent(self, event)
        painter = QtGui.QPainter()
        painter.begin(self)
        painter.fillRect(5, 4, self.width()-10, self.height()-8, self.brush)
        painter.end()
    
    def setColor(self, color):
        if (self.actualColor != color):
            self.actualColor = color;
            self.brush.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(color.red(), color.green(), color.blue(), 255)))
            self.update()
    
    def color(self):
        return self.actualColor
        
    def buttonPressed(self):
        self.colorPicker.setColor(self.actualColor)
        if(self.colorPicker.exec_() == QtGui.QDialog.Accepted):
            self.setColor(self.colorPicker.getColor())
            self.emitColorChanged()
    
    def emitColorChanged(self):
        self.emit(QtCore.SIGNAL("colorChanged()"))
