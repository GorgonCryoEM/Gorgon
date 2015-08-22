# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_raw_loader.ui'
#
# Created: Mon Dec 15 16:27:48 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeRawLoader(object):
    def setupUi(self, DialogVolumeRawLoader):
        DialogVolumeRawLoader.setObjectName("DialogVolumeRawLoader")
        DialogVolumeRawLoader.setWindowModality(QtCore.Qt.ApplicationModal)
        DialogVolumeRawLoader.resize(QtCore.QSize(QtCore.QRect(0,0,234,148).size()).expandedTo(DialogVolumeRawLoader.minimumSizeHint()))
        DialogVolumeRawLoader.setModal(True)

        self.gridlayout = QtGui.QGridLayout(DialogVolumeRawLoader)
        self.gridlayout.setObjectName("gridlayout")

        self.labelX = QtGui.QLabel(DialogVolumeRawLoader)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelX.sizePolicy().hasHeightForWidth())
        self.labelX.setSizePolicy(sizePolicy)
        self.labelX.setObjectName("labelX")
        self.gridlayout.addWidget(self.labelX,0,0,1,1)

        self.spinBoxSizeX = QtGui.QSpinBox(DialogVolumeRawLoader)
        self.spinBoxSizeX.setMaximum(99999)
        self.spinBoxSizeX.setObjectName("spinBoxSizeX")
        self.gridlayout.addWidget(self.spinBoxSizeX,0,1,1,1)

        self.labelY = QtGui.QLabel(DialogVolumeRawLoader)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelY.sizePolicy().hasHeightForWidth())
        self.labelY.setSizePolicy(sizePolicy)
        self.labelY.setObjectName("labelY")
        self.gridlayout.addWidget(self.labelY,1,0,1,1)

        self.spinBoxSizeY = QtGui.QSpinBox(DialogVolumeRawLoader)
        self.spinBoxSizeY.setMaximum(99999)
        self.spinBoxSizeY.setObjectName("spinBoxSizeY")
        self.gridlayout.addWidget(self.spinBoxSizeY,1,1,1,1)

        self.labelZ = QtGui.QLabel(DialogVolumeRawLoader)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelZ.sizePolicy().hasHeightForWidth())
        self.labelZ.setSizePolicy(sizePolicy)
        self.labelZ.setObjectName("labelZ")
        self.gridlayout.addWidget(self.labelZ,2,0,1,1)

        self.spinBoxSizeZ = QtGui.QSpinBox(DialogVolumeRawLoader)
        self.spinBoxSizeZ.setMaximum(99999)
        self.spinBoxSizeZ.setObjectName("spinBoxSizeZ")
        self.gridlayout.addWidget(self.spinBoxSizeZ,2,1,1,1)

        self.labelFormat = QtGui.QLabel(DialogVolumeRawLoader)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelFormat.sizePolicy().hasHeightForWidth())
        self.labelFormat.setSizePolicy(sizePolicy)
        self.labelFormat.setObjectName("labelFormat")
        self.gridlayout.addWidget(self.labelFormat,3,0,1,1)

        self.comboBoxFormat = QtGui.QComboBox(DialogVolumeRawLoader)
        self.comboBoxFormat.setObjectName("comboBoxFormat")
        self.gridlayout.addWidget(self.comboBoxFormat,3,1,1,1)

        self.buttonBox = QtGui.QDialogButtonBox(DialogVolumeRawLoader)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,4,0,1,2)

        self.retranslateUi(DialogVolumeRawLoader)
        self.comboBoxFormat.setCurrentIndex(0)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogVolumeRawLoader.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogVolumeRawLoader.reject)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeRawLoader)
        DialogVolumeRawLoader.setTabOrder(self.spinBoxSizeX,self.spinBoxSizeY)
        DialogVolumeRawLoader.setTabOrder(self.spinBoxSizeY,self.spinBoxSizeZ)
        DialogVolumeRawLoader.setTabOrder(self.spinBoxSizeZ,self.comboBoxFormat)
        DialogVolumeRawLoader.setTabOrder(self.comboBoxFormat,self.buttonBox)

    def retranslateUi(self, DialogVolumeRawLoader):
        DialogVolumeRawLoader.setWindowTitle(QtGui.QApplication.translate("DialogVolumeRawLoader", "Load RAW volume", None, QtGui.QApplication.UnicodeUTF8))
        self.labelX.setText(QtGui.QApplication.translate("DialogVolumeRawLoader", "Size (X):", None, QtGui.QApplication.UnicodeUTF8))
        self.labelY.setText(QtGui.QApplication.translate("DialogVolumeRawLoader", "Size (Y):", None, QtGui.QApplication.UnicodeUTF8))
        self.labelZ.setText(QtGui.QApplication.translate("DialogVolumeRawLoader", "Size (Z):", None, QtGui.QApplication.UnicodeUTF8))
        self.labelFormat.setText(QtGui.QApplication.translate("DialogVolumeRawLoader", "Format:", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxFormat.addItem(QtGui.QApplication.translate("DialogVolumeRawLoader", "8-bit", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxFormat.addItem(QtGui.QApplication.translate("DialogVolumeRawLoader", "16-bit", None, QtGui.QApplication.UnicodeUTF8))

