# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_manual_skeletonization.ui'
#
# Created: Wed Oct 15 13:01:20 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeManualSkeletonization(object):
    def setupUi(self, DialogVolumeManualSkeletonization):
        DialogVolumeManualSkeletonization.setObjectName("DialogVolumeManualSkeletonization")
        DialogVolumeManualSkeletonization.resize(QtCore.QSize(QtCore.QRect(0,0,416,185).size()).expandedTo(DialogVolumeManualSkeletonization.minimumSizeHint()))
        DialogVolumeManualSkeletonization.setMinimumSize(QtCore.QSize(416,144))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeManualSkeletonization)
        self.gridlayout.setObjectName("gridlayout")

        self.labelMedialness = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelMedialness.setObjectName("labelMedialness")
        self.gridlayout.addWidget(self.labelMedialness,0,0,1,1)

        self.horizontalSliderMedialness = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderMedialness.setMinimum(0)
        self.horizontalSliderMedialness.setMaximum(100)
        self.horizontalSliderMedialness.setSingleStep(1)
        self.horizontalSliderMedialness.setPageStep(10)
        self.horizontalSliderMedialness.setProperty("value",QtCore.QVariant(25))
        self.horizontalSliderMedialness.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderMedialness.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderMedialness.setObjectName("horizontalSliderMedialness")
        self.gridlayout.addWidget(self.horizontalSliderMedialness,0,1,1,1)

        self.labelMedialnessDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelMedialnessDisplay.setMinimumSize(QtCore.QSize(20,0))
        self.labelMedialnessDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelMedialnessDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelMedialnessDisplay.setObjectName("labelMedialnessDisplay")
        self.gridlayout.addWidget(self.labelMedialnessDisplay,0,2,1,1)

        self.groupBoxMedialnessScore = QtGui.QGroupBox(DialogVolumeManualSkeletonization)
        self.groupBoxMedialnessScore.setObjectName("groupBoxMedialnessScore")

        self.gridlayout1 = QtGui.QGridLayout(self.groupBoxMedialnessScore)
        self.gridlayout1.setObjectName("gridlayout1")

        self.radioButtonBinary = QtGui.QRadioButton(self.groupBoxMedialnessScore)
        self.radioButtonBinary.setObjectName("radioButtonBinary")
        self.gridlayout1.addWidget(self.radioButtonBinary,0,0,1,1)

        self.radioButtonGlobalRank = QtGui.QRadioButton(self.groupBoxMedialnessScore)
        self.radioButtonGlobalRank.setChecked(True)
        self.radioButtonGlobalRank.setObjectName("radioButtonGlobalRank")
        self.gridlayout1.addWidget(self.radioButtonGlobalRank,1,0,1,1)

        self.radioButtonLocalRank = QtGui.QRadioButton(self.groupBoxMedialnessScore)
        self.radioButtonLocalRank.setChecked(False)
        self.radioButtonLocalRank.setObjectName("radioButtonLocalRank")
        self.gridlayout1.addWidget(self.radioButtonLocalRank,2,0,1,1)
        self.gridlayout.addWidget(self.groupBoxMedialnessScore,0,3,5,1)

        self.labelSmoothness = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelSmoothness.setObjectName("labelSmoothness")
        self.gridlayout.addWidget(self.labelSmoothness,1,0,1,1)

        self.horizontalSliderSmoothness = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderSmoothness.setMinimum(0)
        self.horizontalSliderSmoothness.setMaximum(100)
        self.horizontalSliderSmoothness.setSingleStep(1)
        self.horizontalSliderSmoothness.setPageStep(10)
        self.horizontalSliderSmoothness.setProperty("value",QtCore.QVariant(25))
        self.horizontalSliderSmoothness.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderSmoothness.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderSmoothness.setObjectName("horizontalSliderSmoothness")
        self.gridlayout.addWidget(self.horizontalSliderSmoothness,1,1,1,1)

        self.labeSmoothnessDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labeSmoothnessDisplay.setMinimumSize(QtCore.QSize(20,0))
        self.labeSmoothnessDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labeSmoothnessDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labeSmoothnessDisplay.setObjectName("labeSmoothnessDisplay")
        self.gridlayout.addWidget(self.labeSmoothnessDisplay,1,2,1,1)

        self.labelSketchImportance = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelSketchImportance.setObjectName("labelSketchImportance")
        self.gridlayout.addWidget(self.labelSketchImportance,2,0,1,1)

        self.horizontalSliderSketchPriority = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderSketchPriority.setMinimum(1)
        self.horizontalSliderSketchPriority.setMaximum(10000)
        self.horizontalSliderSketchPriority.setSingleStep(1)
        self.horizontalSliderSketchPriority.setPageStep(10)
        self.horizontalSliderSketchPriority.setProperty("value",QtCore.QVariant(1000))
        self.horizontalSliderSketchPriority.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderSketchPriority.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderSketchPriority.setObjectName("horizontalSliderSketchPriority")
        self.gridlayout.addWidget(self.horizontalSliderSketchPriority,2,1,1,1)

        self.labeSketchPriorityDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labeSketchPriorityDisplay.setMinimumSize(QtCore.QSize(20,0))
        self.labeSketchPriorityDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labeSketchPriorityDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labeSketchPriorityDisplay.setObjectName("labeSketchPriorityDisplay")
        self.gridlayout.addWidget(self.labeSketchPriorityDisplay,2,2,1,1)

        self.labelStartDensity = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStartDensity.setObjectName("labelStartDensity")
        self.gridlayout.addWidget(self.labelStartDensity,3,0,1,1)

        self.horizontalSliderStartingDensity = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderStartingDensity.setMaximum(25500)
        self.horizontalSliderStartingDensity.setSingleStep(1)
        self.horizontalSliderStartingDensity.setPageStep(10)
        self.horizontalSliderStartingDensity.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderStartingDensity.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderStartingDensity.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderStartingDensity.setObjectName("horizontalSliderStartingDensity")
        self.gridlayout.addWidget(self.horizontalSliderStartingDensity,3,1,1,1)

        self.labelStartingDensityDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStartingDensityDisplay.setMinimumSize(QtCore.QSize(20,0))
        self.labelStartingDensityDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelStartingDensityDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelStartingDensityDisplay.setObjectName("labelStartingDensityDisplay")
        self.gridlayout.addWidget(self.labelStartingDensityDisplay,3,2,1,1)

        self.labelStepSize = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStepSize.setObjectName("labelStepSize")
        self.gridlayout.addWidget(self.labelStepSize,4,0,1,1)

        self.horizontalSliderStepCount = QtGui.QSlider(DialogVolumeManualSkeletonization)
        self.horizontalSliderStepCount.setMaximum(1000)
        self.horizontalSliderStepCount.setSingleStep(1)
        self.horizontalSliderStepCount.setPageStep(10)
        self.horizontalSliderStepCount.setProperty("value",QtCore.QVariant(100))
        self.horizontalSliderStepCount.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderStepCount.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderStepCount.setObjectName("horizontalSliderStepCount")
        self.gridlayout.addWidget(self.horizontalSliderStepCount,4,1,1,1)

        self.labelStepSizeDisplay = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelStepSizeDisplay.setMinimumSize(QtCore.QSize(20,0))
        self.labelStepSizeDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelStepSizeDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelStepSizeDisplay.setObjectName("labelStepSizeDisplay")
        self.gridlayout.addWidget(self.labelStepSizeDisplay,4,2,1,1)

        self.gridlayout2 = QtGui.QGridLayout()
        self.gridlayout2.setObjectName("gridlayout2")

        self.labelMinimumCurve = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelMinimumCurve.setObjectName("labelMinimumCurve")
        self.gridlayout2.addWidget(self.labelMinimumCurve,0,0,1,1)

        self.spinBoxMinCurve = QtGui.QSpinBox(DialogVolumeManualSkeletonization)
        self.spinBoxMinCurve.setMaximum(1000000)
        self.spinBoxMinCurve.setProperty("value",QtCore.QVariant(4))
        self.spinBoxMinCurve.setObjectName("spinBoxMinCurve")
        self.gridlayout2.addWidget(self.spinBoxMinCurve,0,1,1,1)
        self.gridlayout.addLayout(self.gridlayout2,5,0,1,3)

        self.gridlayout3 = QtGui.QGridLayout()
        self.gridlayout3.setObjectName("gridlayout3")

        self.labelCurveRadius = QtGui.QLabel(DialogVolumeManualSkeletonization)
        self.labelCurveRadius.setObjectName("labelCurveRadius")
        self.gridlayout3.addWidget(self.labelCurveRadius,0,0,1,1)

        self.spinBoxCurveRadius = QtGui.QSpinBox(DialogVolumeManualSkeletonization)
        self.spinBoxCurveRadius.setMaximum(10)
        self.spinBoxCurveRadius.setProperty("value",QtCore.QVariant(2))
        self.spinBoxCurveRadius.setObjectName("spinBoxCurveRadius")
        self.gridlayout3.addWidget(self.spinBoxCurveRadius,0,1,1,1)
        self.gridlayout.addLayout(self.gridlayout3,5,3,1,1)

        self.gridlayout4 = QtGui.QGridLayout()
        self.gridlayout4.setObjectName("gridlayout4")

        self.pushButtonClear = QtGui.QPushButton(DialogVolumeManualSkeletonization)
        self.pushButtonClear.setObjectName("pushButtonClear")
        self.gridlayout4.addWidget(self.pushButtonClear,0,0,1,1)

        spacerItem = QtGui.QSpacerItem(141,26,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout4.addItem(spacerItem,0,1,1,1)

        self.pushButtonStart = QtGui.QPushButton(DialogVolumeManualSkeletonization)
        self.pushButtonStart.setObjectName("pushButtonStart")
        self.gridlayout4.addWidget(self.pushButtonStart,0,2,1,1)

        self.pushButtonClose = QtGui.QPushButton(DialogVolumeManualSkeletonization)
        self.pushButtonClose.setEnabled(False)
        self.pushButtonClose.setObjectName("pushButtonClose")
        self.gridlayout4.addWidget(self.pushButtonClose,0,3,1,1)
        self.gridlayout.addLayout(self.gridlayout4,6,0,1,4)

        self.retranslateUi(DialogVolumeManualSkeletonization)
        QtCore.QObject.connect(self.horizontalSliderMedialness,QtCore.SIGNAL("valueChanged(int)"),self.labelMedialnessDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderSmoothness,QtCore.SIGNAL("valueChanged(int)"),self.labeSmoothnessDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderStepCount,QtCore.SIGNAL("valueChanged(int)"),self.labelStepSizeDisplay.setNum)
        QtCore.QObject.connect(self.horizontalSliderSketchPriority,QtCore.SIGNAL("valueChanged(int)"),self.labeSketchPriorityDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeManualSkeletonization)
        DialogVolumeManualSkeletonization.setTabOrder(self.horizontalSliderMedialness,self.horizontalSliderSmoothness)
        DialogVolumeManualSkeletonization.setTabOrder(self.horizontalSliderSmoothness,self.horizontalSliderSketchPriority)
        DialogVolumeManualSkeletonization.setTabOrder(self.horizontalSliderSketchPriority,self.horizontalSliderStartingDensity)
        DialogVolumeManualSkeletonization.setTabOrder(self.horizontalSliderStartingDensity,self.horizontalSliderStepCount)
        DialogVolumeManualSkeletonization.setTabOrder(self.horizontalSliderStepCount,self.spinBoxMinCurve)
        DialogVolumeManualSkeletonization.setTabOrder(self.spinBoxMinCurve,self.spinBoxCurveRadius)
        DialogVolumeManualSkeletonization.setTabOrder(self.spinBoxCurveRadius,self.radioButtonBinary)
        DialogVolumeManualSkeletonization.setTabOrder(self.radioButtonBinary,self.radioButtonGlobalRank)
        DialogVolumeManualSkeletonization.setTabOrder(self.radioButtonGlobalRank,self.radioButtonLocalRank)
        DialogVolumeManualSkeletonization.setTabOrder(self.radioButtonLocalRank,self.pushButtonStart)
        DialogVolumeManualSkeletonization.setTabOrder(self.pushButtonStart,self.pushButtonClose)

    def retranslateUi(self, DialogVolumeManualSkeletonization):
        DialogVolumeManualSkeletonization.setWindowTitle(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMedialness.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Medialness:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderMedialness.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMedialnessDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "25", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBoxMedialnessScore.setTitle(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Medialness Function:", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonBinary.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Binary", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonGlobalRank.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Global Rank", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonLocalRank.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Local Rank", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSmoothness.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Smoothness:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderSmoothness.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labeSmoothnessDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "25", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSketchImportance.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Sketch Priority:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderSketchPriority.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labeSketchPriorityDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "1000", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStartDensity.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Starting Density:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderStartingDensity.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStartingDensityDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStepSize.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Step Count:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderStepCount.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStepSizeDisplay.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "100", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMinimumCurve.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Minimum Curve Length:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius.setWhatsThis(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "The Radius to use when obtaining the structure tensor of a curve... The larger the radius, the more global information is used", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Curve Radius:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonClear.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Clear", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonStart.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Start", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonClose.setText(QtGui.QApplication.translate("DialogVolumeManualSkeletonization", "Finish", None, QtGui.QApplication.UnicodeUTF8))

