# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A dialog box which can be used to pick a color 

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/color_picker_form.py,v $
#   $Revision: 1.5 $
#   $Date: 2009/04/08 19:54:59 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: color_picker_form.py,v $
#   Revision 1.5  2009/04/08 19:54:59  ssa1
#   Adding in plugin functionality
#
#   Revision 1.4  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from ui_dialog_color_picker import Ui_DialogColorPicker

class ColorPickerForm(QtGui.QDialog):
    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.createUI()
        
    def createUI(self):
        self.ui = Ui_DialogColorPicker()
        self.ui.setupUi(self)       
        self.connect(self.ui.horizontalSliderRed, QtCore.SIGNAL("valueChanged(int)"), self.colorChanged)
        self.connect(self.ui.horizontalSliderGreen, QtCore.SIGNAL("valueChanged(int)"), self.colorChanged)
        self.connect(self.ui.horizontalSliderBlue, QtCore.SIGNAL("valueChanged(int)"), self.colorChanged) 
        self.connect(self.ui.horizontalSliderAlpha, QtCore.SIGNAL("valueChanged(int)"), self.colorChanged)    
    
    def getRed(self):
        return self.ui.horizontalSliderRed.value()
        
    def getBlue(self):
        return self.ui.horizontalSliderBlue.value()
        
    def getGreen(self):
        return self.ui.horizontalSliderGreen.value()
        
    def getAlpha(self):
        return self.ui.horizontalSliderAlpha.value()
    
    def setColor(self, color):
        self.ui.horizontalSliderRed.setValue(color.red())
        self.ui.horizontalSliderGreen.setValue(color.green())
        self.ui.horizontalSliderBlue.setValue(color.blue())
        self.ui.horizontalSliderAlpha.setValue(color.alpha())
        
        
    def getColor(self):
        return QtGui.QColor.fromRgba(QtGui.qRgba(self.getRed(), self.getGreen(), self.getBlue(), self.getAlpha()))
    
    def colorChanged(self, dummy):
        self.ui.colorPanel.setColor(self.getColor())

    
                                 