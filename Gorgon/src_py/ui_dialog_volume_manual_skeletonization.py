# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_manual_skeletonization.ui'
#
# Created: Fri Apr 11 22:06:28 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeManualSkeletonization(object):
    def setupUi(self, DialogVolumeManualSkeletonization):
        DialogVolumeManualSkeletonization.setObjectName("DialogVolumeManualSkeletonization")
        DialogVolumeManualSkeletonization.resize(QtCore.QSize(QtCore.QRect(0,0,416,144).size()).expandedTo(DialogVolumeManualSkeletonization.minimumSizeHint()))
        DialogVolumeManualSkeletonization.setMinimumSize(QtCore.QSize(416,144))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeManualSkeletonization)
        self.gridlayout.setObjectName("gridlayout")

        self.gridlayout1 = QtGui.QGridLayout()
        self.gridlayout1.setObjectName("gridlayout1")

        self.labelMedialness = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelMedialness.setObjectName("labelMedialness")
        self.gridlayout1.addWidget(self.labelMedialness,0,0,1,1)

        self.horizontalSliderMedialness = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderMedialness.setMinimum(1)
        self.horizontalSliderMedialness.setMaximum(100)
        self.horizontalSliderMedialness.setSingleStep(1)
        self.horizontalSliderMedialness.setPageStep(10)
        self.horizontalSliderMedialness.setProperty("value",QtCore.QVariant(50))
        self.horizontalSliderMedialness.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderMedialness.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderMedialness.setObjectName("horizontalSliderMedialness")
        self.gridlayout1.addWidget(self.horizontalSliderMedialness,0,1,1,1)

        self.labelMedialnessDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelMedialnessDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labelMedialnessDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelMedialnessDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelMedialnessDisplay.setObjectName("labelMedialnessDisplay")
        self.gridlayout1.addWidget(self.labelMedialnessDisplay,0,2,1,1)

        self.line = QtGui.QFrame(DialogVolumeManualSkeletonization)
        self.line.setFrameShape(QtGui.QFrame.VLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.gridlayout1.addWidget(self.line,0,3,4,1)

        self.labelMinimumCurve = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelMinimumCurve.setObjectName("labelMinimumCurve")
        self.gridlayout1.addWidget(self.labelMinimumCurve,0,4,1,1)

        self.spinBoxMinCurve = QtGui.QSpinBox(DialogVolumeManualSkeletonization)
        self.spinBoxMinCurve.setMaximum(1000000)
        self.spinBoxMinCurve.setProperty("value",QtCore.QVariant(4))
        self.spinBoxMinCurve.setObjectName("spinBoxMinCurve")
        self.gridlayout1.addWidget(self.spinBoxMinCurve,0,5,1,1)

        self.labelSmoothness = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelSmoothness.setObjectName("labelSmoothness")
        self.gridlayout1.addWidget(self.labelSmoothness,1,0,1,1)

        self.horizontalSliderSmoothness = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderSmoothness.setMinimum(1)
        self.horizontalSliderSmoothness.setMaximum(100)
        self.horizontalSliderSmoothness.setSingleStep(1)
        self.horizontalSliderSmoothness.setPageStep(10)
        self.horizontalSliderSmoothness.setProperty("value",QtCore.QVariant(50))
        self.horizontalSliderSmoothness.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderSmoothness.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderSmoothness.setObjectName("horizontalSliderSmoothness")
        self.gridlayout1.addWidget(self.horizontalSliderSmoothness,1,1,1,1)

        self.labeSmoothnessDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labeSmoothnessDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labeSmoothnessDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labeSmoothnessDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labeSmoothnessDisplay.setObjectName("labeSmoothnessDisplay")
        self.gridlayout1.addWidget(self.labeSmoothnessDisplay,1,2,1,1)

        self.labelCurveRadius = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelCurveRadius.setObjectName("labelCurveRadius")
        self.gridlayout1.addWidget(self.labelCurveRadius,1,4,2,1)

        self.spinBoxCurveRadius = QtGui.QSpinBox(DialogVolumeManualSkeletonization)
        self.spinBoxCurveRadius.setMaximum(10)
        self.spinBoxCurveRadius.setProperty("value",QtCore.QVariant(2))
        self.spinBoxCurveRadius.setObjectName("spinBoxCurveRadius")
        self.gridlayout1.addWidget(self.spinBoxCurveRadius,1,5,2,1)

        self.labelStartDensity = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStartDensity.setObjectName("labelStartDensity")
        self.gridlayout1.addWidget(self.labelStartDensity,2,0,1,1)

        self.horizontalSliderStartingDensity = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderStartingDensity.setMaximum(25500)
        self.horizontalSliderStartingDensity.setSingleStep(1)
        self.horizontalSliderStartingDensity.setPageStep(10)
        self.horizontalSliderStartingDensity.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderStartingDensity.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderStartingDensity.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderStartingDensity.setObjectName("horizontalSliderStartingDensity")
        self.gridlayout1.addWidget(self.horizontalSliderStartingDensity,2,1,1,1)

        self.labelStartingDensityDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStartingDensityDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labelStartingDensityDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelStartingDensityDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelStartingDensityDisplay.setObjectName("labelStartingDensityDisplay")
        self.gridlayout1.addWidget(self.labelStartingDensityDisplay,2,2,1,1)

        self.labelStepSize = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStepSize.setObjectName("labelStepSize")
        self.gridlayout1.addWidget(self.labelStepSize,3,0,1,1)

        self.horizontalSliderStepCount = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderStepCount.setMaximum(1000)
        self.horizontalSliderStepCount.setSingleStep(1)
        self.horizontalSliderStepCount.setPageStep(10)
        self.horizontalSliderStepCount.setProperty("value",QtCore.QVariant(250))
        self.horizontalSliderStepCount.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderStepCount.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderStepCount.setObjectName("horizontalSliderStepCount")
        self.gridlayout1.addWidget(self.horizontalSliderStepCount,3,1,1,1)

        self.labelStepSizeDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStepSizeDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labelStepSizeDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelStepSizeDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelStepSizeDisplay.setObjectName("labelStepSizeDisplay")
        self.gridlayout1.addWidget(self.labelStepSizeDisplay,3,2,1,1)
        self.gridlayout.addLayout(self.gridlayout1,0,0,1,1)

        self.gridlayout2 = QtGui.QGridLayout()
        self.gridlayout2.setObjectName("gridlayout2")

        spacerItem = QtGui.QSpacerItem(40,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout2.addItem(spacerItem,0,0,1,1)

        self.pushButtonStart = QtGui.QPushButton(DialogVolumeManualSkeletonization)
        self.pushButtonStart.setObjectName("pushButtonStart")
        self.gridlayout2.addWidget(self.pushButtonStart,0,1,1,1)

        self.pushButtonClose = QtGui.QPushButton(DialogVolumeManualSkeletonization)
        self.pushButtonClose.setEnabled(False)
        self.pushButtonClose.setObjectName("pushButtonClose")
        self.gridlayout2.addWidget(self.pushButtonClose,0,2,1,1)
        self.gridlayout.addLayout(self.gridlayout2,1,0,1,1)

        self.retranslateUi(DialogVolumeManualSkeletonization)
        QtCore.QObject.connect(self.horizontalSliderMedialness,QtCore.SIGNAL("valueChanged(int)"),self.labelMedialnessDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderSmoothness,QtCore.SIGNAL("valueChanged(int)"),self.labeSmoothnessDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeManualSkeletonization)

    def retranslateUi(self, DialogVolumeManualSkeletonization):
        DialogVolumeManualSkeletonization.setWindowTitle(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMedialness.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Medialness:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderMedialness.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMedialnessDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "50", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMinimumCurve.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Minimum Curve Length:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSmoothness.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Smoothness:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderSmoothness.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labeSmoothnessDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "50", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "The Radius to use when obtaining the structure tensor of a curve... The larger the radius, the more global information is used", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Curve Radius:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStartDensity.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Starting Density:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderStartingDensity.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStartingDensityDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStepSize.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Step Count:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderStepCount.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStepSizeDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "250", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonStart.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Start", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonClose.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Finish", None, QtGui.QApplication.UnicodeUTF8))

