# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_manual_skeletonization.ui'
#
# Created: Fri Apr 11 12:22:44 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeManualSkeletonization(object):
    def setupUi(self, DialogVolumeManualSkeletonization):
        DialogVolumeManualSkeletonization.setObjectName("DialogVolumeManualSkeletonization")
        DialogVolumeManualSkeletonization.resize(QtCore.QSize(QtCore.QRect(0,0,189,44).size()).expandedTo(DialogVolumeManualSkeletonization.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeManualSkeletonization)
        self.gridlayout.setObjectName("gridlayout")

        self.pushButtonClose = QtGui.QPushButton(DialogVolumeManualSkeletonization)
        self.pushButtonClose.setObjectName("pushButtonClose")
        self.gridlayout.addWidget(self.pushButtonClose,0,0,1,1)

        self.retranslateUi(DialogVolumeManualSkeletonization)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeManualSkeletonization)

    def retranslateUi(self, DialogVolumeManualSkeletonization):
        DialogVolumeManualSkeletonization.setWindowTitle(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonClose.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Exit Manual Skeletonization Mode", None, QtGui.QApplication.UnicodeUTF8))

