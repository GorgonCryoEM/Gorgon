# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_color_picker.ui'
#
# Created: Mon Apr 07 13:18:17 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui
from render_area import RenderArea

class Ui_DialogColorPicker(object):
    def setupUi(self, DialogColorPicker):
        DialogColorPicker.setObjectName("DialogColorPicker")
        DialogColorPicker.resize(QtCore.QSize(QtCore.QRect(0,0,345,146).size()).expandedTo(DialogColorPicker.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogColorPicker)
        self.gridlayout.setObjectName("gridlayout")

        self.frameColorDisplay = RenderArea(DialogColorPicker)
        self.frameColorDisplay.setMinimumSize(QtCore.QSize(16,30))
        self.frameColorDisplay.setObjectName("frameColorDisplay")        
        self.gridlayout.addWidget(self.frameColorDisplay,0,0,1,3)    

        self.labelRed = QtGui.QLabel(DialogColorPicker)
        self.labelRed.setObjectName("labelRed")
        self.gridlayout.addWidget(self.labelRed,1,0,1,1)

        self.horizontalSliderRed = QtGui.QSlider(DialogColorPicker)
        self.horizontalSliderRed.setMaximum(255)
        self.horizontalSliderRed.setProperty("value",QtCore.QVariant(128))
        self.horizontalSliderRed.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderRed.setObjectName("horizontalSliderRed")
        self.gridlayout.addWidget(self.horizontalSliderRed,1,1,1,1)

        self.labelRedDisplay = QtGui.QLabel(DialogColorPicker)
        self.labelRedDisplay.setMinimumSize(QtCore.QSize(25,0))
        self.labelRedDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelRedDisplay.setObjectName("labelRedDisplay")
        self.gridlayout.addWidget(self.labelRedDisplay,1,2,1,1)

        self.labelGreen = QtGui.QLabel(DialogColorPicker)
        self.labelGreen.setObjectName("labelGreen")
        self.gridlayout.addWidget(self.labelGreen,2,0,1,1)

        self.horizontalSliderGreen = QtGui.QSlider(DialogColorPicker)
        self.horizontalSliderGreen.setMaximum(255)
        self.horizontalSliderGreen.setProperty("value",QtCore.QVariant(128))
        self.horizontalSliderGreen.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderGreen.setObjectName("horizontalSliderGreen")
        self.gridlayout.addWidget(self.horizontalSliderGreen,2,1,1,1)

        self.labelGreenDisplay = QtGui.QLabel(DialogColorPicker)
        self.labelGreenDisplay.setMinimumSize(QtCore.QSize(25,0))
        self.labelGreenDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelGreenDisplay.setObjectName("labelGreenDisplay")
        self.gridlayout.addWidget(self.labelGreenDisplay,2,2,1,1)

        self.labelBlue = QtGui.QLabel(DialogColorPicker)
        self.labelBlue.setObjectName("labelBlue")
        self.gridlayout.addWidget(self.labelBlue,3,0,1,1)

        self.horizontalSliderBlue = QtGui.QSlider(DialogColorPicker)
        self.horizontalSliderBlue.setMaximum(255)
        self.horizontalSliderBlue.setProperty("value",QtCore.QVariant(128))
        self.horizontalSliderBlue.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderBlue.setObjectName("horizontalSliderBlue")
        self.gridlayout.addWidget(self.horizontalSliderBlue,3,1,1,1)

        self.labelBlueDisplay = QtGui.QLabel(DialogColorPicker)
        self.labelBlueDisplay.setMinimumSize(QtCore.QSize(25,0))
        self.labelBlueDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelBlueDisplay.setObjectName("labelBlueDisplay")
        self.gridlayout.addWidget(self.labelBlueDisplay,3,2,1,1)

        self.buttonBox = QtGui.QDialogButtonBox(DialogColorPicker)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,4,0,1,3)

        self.retranslateUi(DialogColorPicker)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogColorPicker.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogColorPicker.reject)
        QtCore.QObject.connect(self.horizontalSliderRed,QtCore.SIGNAL("valueChanged(int)"),self.labelRedDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderGreen,QtCore.SIGNAL("valueChanged(int)"),self.labelGreenDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderBlue,QtCore.SIGNAL("valueChanged(int)"),self.labelBlueDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogColorPicker)

    def retranslateUi(self, DialogColorPicker):
        DialogColorPicker.setWindowTitle(QtGui.QApplication.translate("DialogColorPicker", "Color Picker", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRed.setText(QtGui.QApplication.translate("DialogColorPicker", "Red:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRedDisplay.setText(QtGui.QApplication.translate("DialogColorPicker", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelGreen.setText(QtGui.QApplication.translate("DialogColorPicker", "Green:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelGreenDisplay.setText(QtGui.QApplication.translate("DialogColorPicker", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelBlue.setText(QtGui.QApplication.translate("DialogColorPicker", "Blue:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelBlueDisplay.setText(QtGui.QApplication.translate("DialogColorPicker", "128", None, QtGui.QApplication.UnicodeUTF8))

