# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget which paints itself based on the specified color 

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/colored_widget.py,v $
#   $Revision: 1.3 $
#   $Date: 2008/06/18 18:15:41 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: colored_widget.py,v $
#   Revision 1.3  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

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

    