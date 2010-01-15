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
#   Revision 1.5  2009/12/24 01:38:53  ssa1
#   Fixing bug in macos where color dialogs automatically change when camera changes.  Bug ID 4
#
#   Revision 1.4  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from color_picker_form import ColorPickerForm

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
        if(self.colorPicker.exec_() == QtGui.QDialog.Accepted) :
            self.setColor(self.colorPicker.getColor())
            self.emitColorChanged()
    
    def emitColorChanged(self):
        self.emit(QtCore.SIGNAL("colorChanged()"))

    