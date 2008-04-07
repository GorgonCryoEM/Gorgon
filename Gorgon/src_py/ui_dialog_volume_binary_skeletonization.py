# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_binary_skeletonization.ui'
#
# Created: Sun Apr 06 18:24:49 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeBinarySkeletonization(object):
    def setupUi(self, DialogVolumeBinarySkeletonization):
        DialogVolumeBinarySkeletonization.setObjectName("DialogVolumeBinarySkeletonization")
        DialogVolumeBinarySkeletonization.setWindowModality(QtCore.Qt.NonModal)
        DialogVolumeBinarySkeletonization.resize(QtCore.QSize(QtCore.QRect(0,0,401,217).size()).expandedTo(DialogVolumeBinarySkeletonization.minimumSizeHint()))
        DialogVolumeBinarySkeletonization.setMinimumSize(QtCore.QSize(319,217))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeBinarySkeletonization)
        self.gridlayout.setObjectName("gridlayout")

        self.labelMethod = QtGui.QLabel(DialogVolumeBinarySkeletonization)
        self.labelMethod.setObjectName("labelMethod")
        self.gridlayout.addWidget(self.labelMethod,0,0,1,1)

        self.comboBoxMethod = QtGui.QComboBox(DialogVolumeBinarySkeletonization)
        self.comboBoxMethod.setObjectName("comboBoxMethod")
        self.gridlayout.addWidget(self.comboBoxMethod,0,1,1,2)

        self.textCitation = QtGui.QTextBrowser(DialogVolumeBinarySkeletonization)
        self.textCitation.setEnabled(True)
        self.textCitation.setObjectName("textCitation")
        self.gridlayout.addWidget(self.textCitation,1,1,1,2)

        self.labelIsoLevel = QtGui.QLabel(DialogVolumeBinarySkeletonization)
        self.labelIsoLevel.setObjectName("labelIsoLevel")
        self.gridlayout.addWidget(self.labelIsoLevel,2,0,1,1)

        self.horizontalSliderIsoLevel = QtGui.QSlider(DialogVolumeBinarySkeletonization)
        self.horizontalSliderIsoLevel.setMaximum(25500)
        self.horizontalSliderIsoLevel.setSingleStep(1)
        self.horizontalSliderIsoLevel.setPageStep(10)
        self.horizontalSliderIsoLevel.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderIsoLevel.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderIsoLevel.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderIsoLevel.setObjectName("horizontalSliderIsoLevel")
        self.gridlayout.addWidget(self.horizontalSliderIsoLevel,2,1,1,1)

        self.labelIsoValueDisplay = QtGui.QLabel(DialogVolumeBinarySkeletonization)
        self.labelIsoValueDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labelIsoValueDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelIsoValueDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelIsoValueDisplay.setObjectName("labelIsoValueDisplay")
        self.gridlayout.addWidget(self.labelIsoValueDisplay,2,2,1,1)

        self.labelMinimumCurve = QtGui.QLabel(DialogVolumeBinarySkeletonization)
        self.labelMinimumCurve.setObjectName("labelMinimumCurve")
        self.gridlayout.addWidget(self.labelMinimumCurve,3,0,1,1)

        self.spinBoxMinCurve = QtGui.QSpinBox(DialogVolumeBinarySkeletonization)
        self.spinBoxMinCurve.setMaximum(1000000)
        self.spinBoxMinCurve.setProperty("value",QtCore.QVariant(4))
        self.spinBoxMinCurve.setObjectName("spinBoxMinCurve")
        self.gridlayout.addWidget(self.spinBoxMinCurve,3,1,1,2)

        self.labelMaximumCurve = QtGui.QLabel(DialogVolumeBinarySkeletonization)
        self.labelMaximumCurve.setObjectName("labelMaximumCurve")
        self.gridlayout.addWidget(self.labelMaximumCurve,4,0,1,1)

        self.spinBoxMinSurface = QtGui.QSpinBox(DialogVolumeBinarySkeletonization)
        self.spinBoxMinSurface.setMaximum(1000000)
        self.spinBoxMinSurface.setProperty("value",QtCore.QVariant(4))
        self.spinBoxMinSurface.setObjectName("spinBoxMinSurface")
        self.gridlayout.addWidget(self.spinBoxMinSurface,4,1,1,2)

        self.buttonBox = QtGui.QDialogButtonBox(DialogVolumeBinarySkeletonization)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,5,0,1,3)

        self.retranslateUi(DialogVolumeBinarySkeletonization)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogVolumeBinarySkeletonization.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogVolumeBinarySkeletonization.reject)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeBinarySkeletonization)
        DialogVolumeBinarySkeletonization.setTabOrder(self.comboBoxMethod,self.textCitation)
        DialogVolumeBinarySkeletonization.setTabOrder(self.textCitation,self.horizontalSliderIsoLevel)
        DialogVolumeBinarySkeletonization.setTabOrder(self.horizontalSliderIsoLevel,self.spinBoxMinCurve)
        DialogVolumeBinarySkeletonization.setTabOrder(self.spinBoxMinCurve,self.spinBoxMinSurface)
        DialogVolumeBinarySkeletonization.setTabOrder(self.spinBoxMinSurface,self.buttonBox)

    def retranslateUi(self, DialogVolumeBinarySkeletonization):
        DialogVolumeBinarySkeletonization.setWindowTitle(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Volume - Binary Skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMethod.setText(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Method:", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxMethod.setWhatsThis(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "The skeletonization method to use", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxMethod.addItem(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Ju, Baker and Chiu [2007]", None, QtGui.QApplication.UnicodeUTF8))
        self.textCitation.setWhatsThis(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Paper discussing the skeletonization method", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setText(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Density Threshold:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderIsoLevel.setWhatsThis(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoValueDisplay.setText(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMinimumCurve.setText(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Minimum Curve Length:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMaximumCurve.setText(QtGui.QApplication.translate("DialogVolumeBinarySkeletonization", "Minimum Surface Size:", None, QtGui.QApplication.UnicodeUTF8))

