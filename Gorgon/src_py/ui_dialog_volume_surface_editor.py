# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_surface_editor.ui'
#
# Created: Sun Apr 06 03:35:18 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeSurfaceEditor(object):
    def setupUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setObjectName("DialogVolumeSurfaceEditor")
        DialogVolumeSurfaceEditor.resize(QtCore.QSize(QtCore.QRect(0,0,558,95).size()).expandedTo(DialogVolumeSurfaceEditor.minimumSizeHint()))
        DialogVolumeSurfaceEditor.setMinimumSize(QtCore.QSize(230,95))
        DialogVolumeSurfaceEditor.setMaximumSize(QtCore.QSize(16777215,95))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeSurfaceEditor)
        self.gridlayout.setObjectName("gridlayout")

        self.groupBoxSurfaceOptions = QtGui.QGroupBox(DialogVolumeSurfaceEditor)
        self.groupBoxSurfaceOptions.setObjectName("groupBoxSurfaceOptions")

        self.gridlayout1 = QtGui.QGridLayout(self.groupBoxSurfaceOptions)
        self.gridlayout1.setObjectName("gridlayout1")

        self.labelIsoLevel = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelIsoLevel.setObjectName("labelIsoLevel")
        self.gridlayout1.addWidget(self.labelIsoLevel,0,0,1,1)

        self.horizontalSliderIsoLevel = QtGui.QSlider(self.groupBoxSurfaceOptions)
        self.horizontalSliderIsoLevel.setMaximum(25500)
        self.horizontalSliderIsoLevel.setSingleStep(1)
        self.horizontalSliderIsoLevel.setPageStep(10)
        self.horizontalSliderIsoLevel.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderIsoLevel.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderIsoLevel.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderIsoLevel.setObjectName("horizontalSliderIsoLevel")
        self.gridlayout1.addWidget(self.horizontalSliderIsoLevel,0,1,1,1)

        self.labelIsoValueDisplay = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelIsoValueDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labelIsoValueDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelIsoValueDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelIsoValueDisplay.setObjectName("labelIsoValueDisplay")
        self.gridlayout1.addWidget(self.labelIsoValueDisplay,0,2,1,1)

        self.labelSamplingInterval = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelSamplingInterval.setObjectName("labelSamplingInterval")
        self.gridlayout1.addWidget(self.labelSamplingInterval,1,0,1,1)

        self.horizontalSliderSampling = QtGui.QSlider(self.groupBoxSurfaceOptions)
        self.horizontalSliderSampling.setMinimum(1)
        self.horizontalSliderSampling.setMaximum(8)
        self.horizontalSliderSampling.setPageStep(1)
        self.horizontalSliderSampling.setProperty("value",QtCore.QVariant(1))
        self.horizontalSliderSampling.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderSampling.setTickPosition(QtGui.QSlider.TicksBelow)
        self.horizontalSliderSampling.setTickInterval(0)
        self.horizontalSliderSampling.setObjectName("horizontalSliderSampling")
        self.gridlayout1.addWidget(self.horizontalSliderSampling,1,1,1,1)

        self.labelSamplingDisplay = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelSamplingDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelSamplingDisplay.setObjectName("labelSamplingDisplay")
        self.gridlayout1.addWidget(self.labelSamplingDisplay,1,2,1,1)
        self.gridlayout.addWidget(self.groupBoxSurfaceOptions,0,0,1,1)

        self.groupBoxVisualization = QtGui.QGroupBox(DialogVolumeSurfaceEditor)
        self.groupBoxVisualization.setObjectName("groupBoxVisualization")

        self.gridlayout2 = QtGui.QGridLayout(self.groupBoxVisualization)
        self.gridlayout2.setObjectName("gridlayout2")

        self.labelTransparency = QtGui.QLabel(self.groupBoxVisualization)
        self.labelTransparency.setObjectName("labelTransparency")
        self.gridlayout2.addWidget(self.labelTransparency,0,0,1,1)

        self.horizontalSliderTransparency = QtGui.QSlider(self.groupBoxVisualization)
        self.horizontalSliderTransparency.setMaximum(100)
        self.horizontalSliderTransparency.setSingleStep(1)
        self.horizontalSliderTransparency.setPageStep(10)
        self.horizontalSliderTransparency.setProperty("value",QtCore.QVariant(0))
        self.horizontalSliderTransparency.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderTransparency.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderTransparency.setObjectName("horizontalSliderTransparency")
        self.gridlayout2.addWidget(self.horizontalSliderTransparency,0,1,1,1)

        self.labelTransparencyDisplay = QtGui.QLabel(self.groupBoxVisualization)
        self.labelTransparencyDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labelTransparencyDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelTransparencyDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelTransparencyDisplay.setObjectName("labelTransparencyDisplay")
        self.gridlayout2.addWidget(self.labelTransparencyDisplay,0,2,1,1)
        self.gridlayout.addWidget(self.groupBoxVisualization,0,1,1,1)

        self.retranslateUi(DialogVolumeSurfaceEditor)
        QtCore.QObject.connect(self.horizontalSliderSampling,QtCore.SIGNAL("valueChanged(int)"),self.labelSamplingDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeSurfaceEditor)

    def retranslateUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setWindowTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Volume - Surface Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBoxSurfaceOptions.setTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Surface Parameters:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Density Threshold:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoValueDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingInterval.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Sampling Interval:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "1", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBoxVisualization.setTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Visualization:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelTransparency.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Transparency:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelTransparencyDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "0", None, QtGui.QApplication.UnicodeUTF8))

