# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A pushbutton which instead of displaying text, displays a color 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$

from PyQt4 import QtCore, QtGui
from color_picker_form import ColorPickerForm

class ColoredPushButton(QtGui.QPushButton):
    
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.brush = QtGui.QBrush()
        self.brush.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(128, 128, 128, 255)))
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
        self.brush.setColor(color)
        self.update()
    
    def color(self):
        return self.brush.color()
        
    def buttonPressed(self):
        self.colorPicker.setColor(self.brush.color())
        if(self.colorPicker.exec_() == QtGui.QDialog.Accepted) :
            self.setColor(self.colorPicker.getColor())
            self.emitColorChanged()
    
    def emitColorChanged(self):
        self.emit(QtCore.SIGNAL("colorChanged()"))

    