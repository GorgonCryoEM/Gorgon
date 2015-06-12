# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_color_picker.ui'
#
# Created: Mon Apr 07 19:47:09 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogColorPicker(object):
    def setupUi(self, DialogColorPicker):
        DialogColorPicker.setObjectName("DialogColorPicker")
        DialogColorPicker.setWindowModality(QtCore.Qt.ApplicationModal)
        DialogColorPicker.resize(QtCore.QSize(QtCore.QRect(0,0,345,168).size()).expandedTo(DialogColorPicker.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogColorPicker)
        self.gridlayout.setObjectName("gridlayout")

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

        self.labelAlpha = QtGui.QLabel(DialogColorPicker)
        self.labelAlpha.setObjectName("labelAlpha")
        self.gridlayout.addWidget(self.labelAlpha,4,0,1,1)

        self.horizontalSliderAlpha = QtGui.QSlider(DialogColorPicker)
        self.horizontalSliderAlpha.setMaximum(255)
        self.horizontalSliderAlpha.setProperty("value",QtCore.QVariant(255))
        self.horizontalSliderAlpha.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderAlpha.setObjectName("horizontalSliderAlpha")
        self.gridlayout.addWidget(self.horizontalSliderAlpha,4,1,1,1)

        self.labelAlphaDisplay = QtGui.QLabel(DialogColorPicker)
        self.labelAlphaDisplay.setMinimumSize(QtCore.QSize(25,0))
        self.labelAlphaDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelAlphaDisplay.setObjectName("labelAlphaDisplay")
        self.gridlayout.addWidget(self.labelAlphaDisplay,4,2,1,1)

        self.buttonBox = QtGui.QDialogButtonBox(DialogColorPicker)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,5,0,1,3)

        self.colorPanel = ColoredWidget(DialogColorPicker)
        self.colorPanel.setMinimumSize(QtCore.QSize(0,30))
        self.colorPanel.setObjectName("colorPanel")
        self.gridlayout.addWidget(self.colorPanel,0,0,1,3)

        self.retranslateUi(DialogColorPicker)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogColorPicker.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogColorPicker.reject)
        QtCore.QObject.connect(self.horizontalSliderRed,QtCore.SIGNAL("valueChanged(int)"),self.labelRedDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderGreen,QtCore.SIGNAL("valueChanged(int)"),self.labelGreenDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderBlue,QtCore.SIGNAL("valueChanged(int)"),self.labelBlueDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderAlpha,QtCore.SIGNAL("valueChanged(int)"),self.labelAlphaDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogColorPicker)
        DialogColorPicker.setTabOrder(self.horizontalSliderRed,self.horizontalSliderGreen)
        DialogColorPicker.setTabOrder(self.horizontalSliderGreen,self.horizontalSliderBlue)
        DialogColorPicker.setTabOrder(self.horizontalSliderBlue,self.horizontalSliderAlpha)
        DialogColorPicker.setTabOrder(self.horizontalSliderAlpha,self.buttonBox)

    def retranslateUi(self, DialogColorPicker):
        DialogColorPicker.setWindowTitle(QtGui.QApplication.translate("DialogColorPicker", "Color Picker", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRed.setText(QtGui.QApplication.translate("DialogColorPicker", "Red:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRedDisplay.setText(QtGui.QApplication.translate("DialogColorPicker", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelGreen.setText(QtGui.QApplication.translate("DialogColorPicker", "Green:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelGreenDisplay.setText(QtGui.QApplication.translate("DialogColorPicker", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelBlue.setText(QtGui.QApplication.translate("DialogColorPicker", "Blue:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelBlueDisplay.setText(QtGui.QApplication.translate("DialogColorPicker", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelAlpha.setText(QtGui.QApplication.translate("DialogColorPicker", "Alpha:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelAlphaDisplay.setText(QtGui.QApplication.translate("DialogColorPicker", "255", None, QtGui.QApplication.UnicodeUTF8))

from colored_widget import ColoredWidget
