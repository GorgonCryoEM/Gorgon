# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_surface_editor.ui'
#
# Created: Sat Apr 05 23:53:31 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeSurfaceEditor(object):
    def setupUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setObjectName("DialogVolumeSurfaceEditor")
        DialogVolumeSurfaceEditor.resize(QtCore.QSize(QtCore.QRect(0,0,230,67).size()).expandedTo(DialogVolumeSurfaceEditor.minimumSizeHint()))
        DialogVolumeSurfaceEditor.setMinimumSize(QtCore.QSize(230,67))
        DialogVolumeSurfaceEditor.setMaximumSize(QtCore.QSize(16777215,67))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeSurfaceEditor)
        self.gridlayout.setObjectName("gridlayout")

        self.labelIsoLevel = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelIsoLevel.setObjectName("labelIsoLevel")
        self.gridlayout.addWidget(self.labelIsoLevel,0,0,1,1)

        self.horizontalSliderIsoLevel = QtGui.QSlider(DialogVolumeSurfaceEditor)
        self.horizontalSliderIsoLevel.setMaximum(25500)
        self.horizontalSliderIsoLevel.setSingleStep(1)
        self.horizontalSliderIsoLevel.setPageStep(10)
        self.horizontalSliderIsoLevel.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderIsoLevel.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderIsoLevel.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderIsoLevel.setObjectName("horizontalSliderIsoLevel")
        self.gridlayout.addWidget(self.horizontalSliderIsoLevel,0,1,1,1)

        self.labelIsoValueDisplay = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelIsoValueDisplay.setMinimumSize(QtCore.QSize(40,0))
        self.labelIsoValueDisplay.setMaximumSize(QtCore.QSize(40,16777215))
        self.labelIsoValueDisplay.setObjectName("labelIsoValueDisplay")
        self.gridlayout.addWidget(self.labelIsoValueDisplay,0,2,1,1)

        self.labelSamplingInterval = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelSamplingInterval.setObjectName("labelSamplingInterval")
        self.gridlayout.addWidget(self.labelSamplingInterval,1,0,1,1)

        self.horizontalSliderSampling = QtGui.QSlider(DialogVolumeSurfaceEditor)
        self.horizontalSliderSampling.setMinimum(1)
        self.horizontalSliderSampling.setMaximum(8)
        self.horizontalSliderSampling.setPageStep(1)
        self.horizontalSliderSampling.setProperty("value",QtCore.QVariant(1))
        self.horizontalSliderSampling.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderSampling.setTickPosition(QtGui.QSlider.TicksBelow)
        self.horizontalSliderSampling.setTickInterval(0)
        self.horizontalSliderSampling.setObjectName("horizontalSliderSampling")
        self.gridlayout.addWidget(self.horizontalSliderSampling,1,1,1,1)

        self.labelSamplingDisplay = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelSamplingDisplay.setObjectName("labelSamplingDisplay")
        self.gridlayout.addWidget(self.labelSamplingDisplay,1,2,1,1)

        self.retranslateUi(DialogVolumeSurfaceEditor)
        QtCore.QObject.connect(self.horizontalSliderSampling,QtCore.SIGNAL("valueChanged(int)"),self.labelSamplingDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeSurfaceEditor)

    def retranslateUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setWindowTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Volume - Surface Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Density Threshold:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoValueDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingInterval.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Sampling Interval:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "1", None, QtGui.QApplication.UnicodeUTF8))

