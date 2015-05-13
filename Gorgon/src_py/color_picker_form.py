# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A dialog box which can be used to pick a color 


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

    
                                 
