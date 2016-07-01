# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_grayscale_skeletonization.ui'
#
# Created: Thu Sep 10 13:21:30 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeGrayscaleSkeletonization(object):
    def setupUi(self, DialogVolumeGrayscaleSkeletonization):
        DialogVolumeGrayscaleSkeletonization.setObjectName("DialogVolumeGrayscaleSkeletonization")
        DialogVolumeGrayscaleSkeletonization.resize(QtCore.QSize(QtCore.QRect(0,0,334,337).size()).expandedTo(DialogVolumeGrayscaleSkeletonization.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeGrayscaleSkeletonization)
        self.gridlayout.setObjectName("gridlayout")

        self.labelMethod = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelMethod.setObjectName("labelMethod")
        self.gridlayout.addWidget(self.labelMethod,0,0,1,1)

        self.comboBoxMethod = QtGui.QComboBox(DialogVolumeGrayscaleSkeletonization)
        self.comboBoxMethod.setObjectName("comboBoxMethod")
        self.gridlayout.addWidget(self.comboBoxMethod,0,1,1,2)

        self.textCitation = QtGui.QTextBrowser(DialogVolumeGrayscaleSkeletonization)
        self.textCitation.setEnabled(True)
        self.textCitation.setObjectName("textCitation")
        self.gridlayout.addWidget(self.textCitation,1,1,1,2)

        self.labelStartDensity = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelStartDensity.setObjectName("labelStartDensity")
        self.gridlayout.addWidget(self.labelStartDensity,2,0,1,1)

        self.horizontalSliderStartingDensity = QtGui.QSlider(DialogVolumeGrayscaleSkeletonization)
        self.horizontalSliderStartingDensity.setMaximum(25500)
        self.horizontalSliderStartingDensity.setSingleStep(1)
        self.horizontalSliderStartingDensity.setPageStep(10)
        self.horizontalSliderStartingDensity.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderStartingDensity.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderStartingDensity.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderStartingDensity.setObjectName("horizontalSliderStartingDensity")
        self.gridlayout.addWidget(self.horizontalSliderStartingDensity,2,1,1,1)

        self.labelStartingDensityDisplay = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelStartingDensityDisplay.sizePolicy().hasHeightForWidth())
        self.labelStartingDensityDisplay.setSizePolicy(sizePolicy)
        self.labelStartingDensityDisplay.setMinimumSize(QtCore.QSize(10,0))
        self.labelStartingDensityDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelStartingDensityDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelStartingDensityDisplay.setObjectName("labelStartingDensityDisplay")
        self.gridlayout.addWidget(self.labelStartingDensityDisplay,2,2,1,1)

        self.labelStepSize = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelStepSize.setObjectName("labelStepSize")
        self.gridlayout.addWidget(self.labelStepSize,3,0,1,1)

        self.horizontalSliderStepCount = QtGui.QSlider(DialogVolumeGrayscaleSkeletonization)
        self.horizontalSliderStepCount.setMaximum(1000)
        self.horizontalSliderStepCount.setSingleStep(1)
        self.horizontalSliderStepCount.setPageStep(10)
        self.horizontalSliderStepCount.setProperty("value",QtCore.QVariant(250))
        self.horizontalSliderStepCount.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderStepCount.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderStepCount.setObjectName("horizontalSliderStepCount")
        self.gridlayout.addWidget(self.horizontalSliderStepCount,3,1,1,1)

        self.labelStepSizeDisplay = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelStepSizeDisplay.sizePolicy().hasHeightForWidth())
        self.labelStepSizeDisplay.setSizePolicy(sizePolicy)
        self.labelStepSizeDisplay.setMinimumSize(QtCore.QSize(10,0))
        self.labelStepSizeDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelStepSizeDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelStepSizeDisplay.setObjectName("labelStepSizeDisplay")
        self.gridlayout.addWidget(self.labelStepSizeDisplay,3,2,1,1)

        self.labelMinimumCurve = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelMinimumCurve.setObjectName("labelMinimumCurve")
        self.gridlayout.addWidget(self.labelMinimumCurve,4,0,1,1)

        self.spinBoxMinCurve = QtGui.QSpinBox(DialogVolumeGrayscaleSkeletonization)
        self.spinBoxMinCurve.setMaximum(1000000)
        self.spinBoxMinCurve.setProperty("value",QtCore.QVariant(4))
        self.spinBoxMinCurve.setObjectName("spinBoxMinCurve")
        self.gridlayout.addWidget(self.spinBoxMinCurve,4,1,1,2)

        self.labelMaximumCurve = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelMaximumCurve.setObjectName("labelMaximumCurve")
        self.gridlayout.addWidget(self.labelMaximumCurve,5,0,1,1)

        self.spinBoxMinSurface = QtGui.QSpinBox(DialogVolumeGrayscaleSkeletonization)
        self.spinBoxMinSurface.setMaximum(1000000)
        self.spinBoxMinSurface.setProperty("value",QtCore.QVariant(4))
        self.spinBoxMinSurface.setObjectName("spinBoxMinSurface")
        self.gridlayout.addWidget(self.spinBoxMinSurface,5,1,1,2)

        self.labelCurveRadius = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelCurveRadius.setObjectName("labelCurveRadius")
        self.gridlayout.addWidget(self.labelCurveRadius,6,0,1,1)

        self.spinBoxCurveRadius = QtGui.QSpinBox(DialogVolumeGrayscaleSkeletonization)
        self.spinBoxCurveRadius.setMaximum(10)
        self.spinBoxCurveRadius.setProperty("value",QtCore.QVariant(2))
        self.spinBoxCurveRadius.setObjectName("spinBoxCurveRadius")
        self.gridlayout.addWidget(self.spinBoxCurveRadius,6,1,1,2)

        self.labelCurveRadius_2 = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelCurveRadius_2.setObjectName("labelCurveRadius_2")
        self.gridlayout.addWidget(self.labelCurveRadius_2,7,0,1,1)

        self.spinBoxSurfaceRadius = QtGui.QSpinBox(DialogVolumeGrayscaleSkeletonization)
        self.spinBoxSurfaceRadius.setMaximum(10)
        self.spinBoxSurfaceRadius.setProperty("value",QtCore.QVariant(2))
        self.spinBoxSurfaceRadius.setObjectName("spinBoxSurfaceRadius")
        self.gridlayout.addWidget(self.spinBoxSurfaceRadius,7,1,1,2)

        self.labelCurveRadius_3 = QtGui.QLabel(DialogVolumeGrayscaleSkeletonization)
        self.labelCurveRadius_3.setObjectName("labelCurveRadius_3")
        self.gridlayout.addWidget(self.labelCurveRadius_3,8,0,1,1)

        self.spinBoxSkeletonRadius = QtGui.QSpinBox(DialogVolumeGrayscaleSkeletonization)
        self.spinBoxSkeletonRadius.setMaximum(10)
        self.spinBoxSkeletonRadius.setProperty("value",QtCore.QVariant(4))
        self.spinBoxSkeletonRadius.setObjectName("spinBoxSkeletonRadius")
        self.gridlayout.addWidget(self.spinBoxSkeletonRadius,8,1,1,2)

        self.checkBoxPreserveSkeleton = QtGui.QCheckBox(DialogVolumeGrayscaleSkeletonization)
        self.checkBoxPreserveSkeleton.setEnabled(False)
        self.checkBoxPreserveSkeleton.setObjectName("checkBoxPreserveSkeleton")
        self.gridlayout.addWidget(self.checkBoxPreserveSkeleton,9,0,1,2)

        self.buttonBox = QtGui.QDialogButtonBox(DialogVolumeGrayscaleSkeletonization)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,10,1,1,2)

        self.retranslateUi(DialogVolumeGrayscaleSkeletonization)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogVolumeGrayscaleSkeletonization.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogVolumeGrayscaleSkeletonization.reject)
        QtCore.QObject.connect(self.horizontalSliderStepCount,QtCore.SIGNAL("valueChanged(int)"),self.labelStepSizeDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeGrayscaleSkeletonization)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.comboBoxMethod,self.textCitation)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.textCitation,self.horizontalSliderStartingDensity)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.horizontalSliderStartingDensity,self.horizontalSliderStepCount)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.horizontalSliderStepCount,self.spinBoxMinCurve)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.spinBoxMinCurve,self.spinBoxMinSurface)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.spinBoxMinSurface,self.spinBoxCurveRadius)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.spinBoxCurveRadius,self.spinBoxSurfaceRadius)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.spinBoxSurfaceRadius,self.spinBoxSkeletonRadius)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.spinBoxSkeletonRadius,self.checkBoxPreserveSkeleton)
        DialogVolumeGrayscaleSkeletonization.setTabOrder(self.checkBoxPreserveSkeleton,self.buttonBox)

    def retranslateUi(self, DialogVolumeGrayscaleSkeletonization):
        DialogVolumeGrayscaleSkeletonization.setWindowTitle(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Volume - Grayscale Skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMethod.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Method:", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxMethod.setWhatsThis(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "The skeletonization method to use", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxMethod.addItem(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Abeysinghe, et al [2008]", None, QtGui.QApplication.UnicodeUTF8))
        self.textCitation.setWhatsThis(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Paper discussing the skeletonization method", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStartDensity.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Starting Density:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderStartingDensity.setWhatsThis(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStartingDensityDisplay.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStepSize.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Step Count:", None, QtGui.QApplication.UnicodeUTF8))
        self.horizontalSliderStepCount.setWhatsThis(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Picks the density to use when performing skeletonization", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStepSizeDisplay.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "250", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMinimumCurve.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Minimum Curve Length:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelMaximumCurve.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Minimum Surface Size:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius.setWhatsThis(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "The Radius to use when obtaining the structure tensor of a curve... The larger the radius, the more global information is used", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Curve Radius:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius_2.setWhatsThis(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "The Radius to use when obtaining the structure tensor of a surface... The larger the radius, the more global information is used", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius_2.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Surface Radius:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius_3.setWhatsThis(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "The Radius to use when obtaining the direction of a skeletal element... The larger the radius, the more global information is used", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCurveRadius_3.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Skeleton Radius:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxPreserveSkeleton.setText(QtGui.QApplication.translate("DialogVolumeGrayscaleSkeletonization", "Use current skeleton as base", None, QtGui.QApplication.UnicodeUTF8))

