# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_crop.ui'
#
# Created: Tue Nov 18 14:15:34 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeCrop(object):
    def setupUi(self, DialogVolumeCrop):
        DialogVolumeCrop.setObjectName("DialogVolumeCrop")
        DialogVolumeCrop.resize(QtCore.QSize(QtCore.QRect(0,0,281,96).size()).expandedTo(DialogVolumeCrop.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeCrop)
        self.gridlayout.setObjectName("gridlayout")

        self.labelTopLeft = QtGui.QLabel(DialogVolumeCrop)
        self.labelTopLeft.setObjectName("labelTopLeft")
        self.gridlayout.addWidget(self.labelTopLeft,0,0,1,1)

        self.spinBoxTopLeftX = QtGui.QSpinBox(DialogVolumeCrop)
        self.spinBoxTopLeftX.setObjectName("spinBoxTopLeftX")
        self.gridlayout.addWidget(self.spinBoxTopLeftX,0,1,1,1)

        self.spinBoxTopLeftY = QtGui.QSpinBox(DialogVolumeCrop)
        self.spinBoxTopLeftY.setObjectName("spinBoxTopLeftY")
        self.gridlayout.addWidget(self.spinBoxTopLeftY,0,2,1,1)

        self.spinBoxTopLeftZ = QtGui.QSpinBox(DialogVolumeCrop)
        self.spinBoxTopLeftZ.setObjectName("spinBoxTopLeftZ")
        self.gridlayout.addWidget(self.spinBoxTopLeftZ,0,3,1,1)

        self.labelBottomRight = QtGui.QLabel(DialogVolumeCrop)
        self.labelBottomRight.setObjectName("labelBottomRight")
        self.gridlayout.addWidget(self.labelBottomRight,1,0,1,1)

        self.spinBoxBottomRightX = QtGui.QSpinBox(DialogVolumeCrop)
        self.spinBoxBottomRightX.setObjectName("spinBoxBottomRightX")
        self.gridlayout.addWidget(self.spinBoxBottomRightX,1,1,1,1)

        self.spinBoxBottomRightY = QtGui.QSpinBox(DialogVolumeCrop)
        self.spinBoxBottomRightY.setObjectName("spinBoxBottomRightY")
        self.gridlayout.addWidget(self.spinBoxBottomRightY,1,2,1,1)

        self.spinBoxBottomRightZ = QtGui.QSpinBox(DialogVolumeCrop)
        self.spinBoxBottomRightZ.setObjectName("spinBoxBottomRightZ")
        self.gridlayout.addWidget(self.spinBoxBottomRightZ,1,3,1,1)

        self.buttonBox = QtGui.QDialogButtonBox(DialogVolumeCrop)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,2,0,1,4)

        self.retranslateUi(DialogVolumeCrop)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogVolumeCrop.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogVolumeCrop.reject)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeCrop)
        DialogVolumeCrop.setTabOrder(self.spinBoxTopLeftX,self.spinBoxTopLeftY)
        DialogVolumeCrop.setTabOrder(self.spinBoxTopLeftY,self.spinBoxTopLeftZ)
        DialogVolumeCrop.setTabOrder(self.spinBoxTopLeftZ,self.spinBoxBottomRightX)
        DialogVolumeCrop.setTabOrder(self.spinBoxBottomRightX,self.spinBoxBottomRightY)
        DialogVolumeCrop.setTabOrder(self.spinBoxBottomRightY,self.spinBoxBottomRightZ)
        DialogVolumeCrop.setTabOrder(self.spinBoxBottomRightZ,self.buttonBox)

    def retranslateUi(self, DialogVolumeCrop):
        DialogVolumeCrop.setWindowTitle(QtGui.QApplication.translate("DialogVolumeCrop", "Volume Crop", None, QtGui.QApplication.UnicodeUTF8))
        self.labelTopLeft.setText(QtGui.QApplication.translate("DialogVolumeCrop", "Top Left:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelBottomRight.setText(QtGui.QApplication.translate("DialogVolumeCrop", "Bottom Right:", None, QtGui.QApplication.UnicodeUTF8))

