# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_surface_editor.ui'
#
# Created: Wed Sep 03 13:31:59 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeSurfaceEditor(object):
    def setupUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setObjectName("DialogVolumeSurfaceEditor")
        DialogVolumeSurfaceEditor.resize(QtCore.QSize(QtCore.QRect(0,0,338,146).size()).expandedTo(DialogVolumeSurfaceEditor.minimumSizeHint()))
        DialogVolumeSurfaceEditor.setMinimumSize(QtCore.QSize(230,124))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeSurfaceEditor)
        self.gridlayout.setObjectName("gridlayout")

        self.groupBoxSurfaceOptions = QtGui.QGroupBox(DialogVolumeSurfaceEditor)
        self.groupBoxSurfaceOptions.setObjectName("groupBoxSurfaceOptions")

        self.gridlayout1 = QtGui.QGridLayout(self.groupBoxSurfaceOptions)
        self.gridlayout1.setObjectName("gridlayout1")

        self.gridlayout2 = QtGui.QGridLayout()
        self.gridlayout2.setObjectName("gridlayout2")

        self.radioButtonIsoSurface = QtGui.QRadioButton(self.groupBoxSurfaceOptions)
        self.radioButtonIsoSurface.setChecked(True)
        self.radioButtonIsoSurface.setObjectName("radioButtonIsoSurface")
        self.gridlayout2.addWidget(self.radioButtonIsoSurface,0,0,1,1)

        self.radioButtonCrossSection = QtGui.QRadioButton(self.groupBoxSurfaceOptions)
        self.radioButtonCrossSection.setObjectName("radioButtonCrossSection")
        self.gridlayout2.addWidget(self.radioButtonCrossSection,0,1,1,1)

        self.radioButtonSolid = QtGui.QRadioButton(self.groupBoxSurfaceOptions)
        self.radioButtonSolid.setObjectName("radioButtonSolid")
        self.gridlayout2.addWidget(self.radioButtonSolid,0,2,1,1)
        self.gridlayout1.addLayout(self.gridlayout2,0,0,1,3)

        self.labelIsoLevel = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelIsoLevel.setObjectName("labelIsoLevel")
        self.gridlayout1.addWidget(self.labelIsoLevel,1,0,1,1)

        self.horizontalSliderIsoLevel = QtGui.QSlider(self.groupBoxSurfaceOptions)
        self.horizontalSliderIsoLevel.setMaximum(25500)
        self.horizontalSliderIsoLevel.setSingleStep(1)
        self.horizontalSliderIsoLevel.setPageStep(10)
        self.horizontalSliderIsoLevel.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderIsoLevel.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderIsoLevel.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderIsoLevel.setObjectName("horizontalSliderIsoLevel")
        self.gridlayout1.addWidget(self.horizontalSliderIsoLevel,1,1,1,1)

        self.labelIsoValueDisplay = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelIsoValueDisplay.setMinimumSize(QtCore.QSize(60,0))
        self.labelIsoValueDisplay.setMaximumSize(QtCore.QSize(60,16777215))
        self.labelIsoValueDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelIsoValueDisplay.setObjectName("labelIsoValueDisplay")
        self.gridlayout1.addWidget(self.labelIsoValueDisplay,1,2,1,1)

        self.labelSamplingInterval = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelSamplingInterval.setObjectName("labelSamplingInterval")
        self.gridlayout1.addWidget(self.labelSamplingInterval,2,0,1,1)

        self.horizontalSliderSampling = QtGui.QSlider(self.groupBoxSurfaceOptions)
        self.horizontalSliderSampling.setMinimum(1)
        self.horizontalSliderSampling.setMaximum(8)
        self.horizontalSliderSampling.setPageStep(1)
        self.horizontalSliderSampling.setProperty("value",QtCore.QVariant(1))
        self.horizontalSliderSampling.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderSampling.setTickPosition(QtGui.QSlider.TicksBelow)
        self.horizontalSliderSampling.setTickInterval(0)
        self.horizontalSliderSampling.setObjectName("horizontalSliderSampling")
        self.gridlayout1.addWidget(self.horizontalSliderSampling,2,1,1,1)

        self.labelSamplingDisplay = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelSamplingDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelSamplingDisplay.setObjectName("labelSamplingDisplay")
        self.gridlayout1.addWidget(self.labelSamplingDisplay,2,2,1,1)

        self.labelDisplayRadius = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelDisplayRadius.setObjectName("labelDisplayRadius")
        self.gridlayout1.addWidget(self.labelDisplayRadius,3,0,1,1)

        self.horizontalSliderDisplayRadius = QtGui.QSlider(self.groupBoxSurfaceOptions)
        self.horizontalSliderDisplayRadius.setMinimum(1)
        self.horizontalSliderDisplayRadius.setMaximum(255)
        self.horizontalSliderDisplayRadius.setPageStep(1)
        self.horizontalSliderDisplayRadius.setProperty("value",QtCore.QVariant(255))
        self.horizontalSliderDisplayRadius.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderDisplayRadius.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderDisplayRadius.setTickInterval(0)
        self.horizontalSliderDisplayRadius.setObjectName("horizontalSliderDisplayRadius")
        self.gridlayout1.addWidget(self.horizontalSliderDisplayRadius,3,1,1,1)

        self.labelDisplayRadiusDisplay = QtGui.QLabel(self.groupBoxSurfaceOptions)
        self.labelDisplayRadiusDisplay.setLayoutDirection(QtCore.Qt.RightToLeft)
        self.labelDisplayRadiusDisplay.setObjectName("labelDisplayRadiusDisplay")
        self.gridlayout1.addWidget(self.labelDisplayRadiusDisplay,3,2,1,1)
        self.gridlayout.addWidget(self.groupBoxSurfaceOptions,0,0,1,1)

        self.retranslateUi(DialogVolumeSurfaceEditor)
        QtCore.QObject.connect(self.horizontalSliderSampling,QtCore.SIGNAL("valueChanged(int)"),self.labelSamplingDisplay.setNum)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.horizontalSliderSampling.setEnabled)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.labelSamplingInterval.setEnabled)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.labelSamplingDisplay.setEnabled)
        QtCore.QObject.connect(self.horizontalSliderDisplayRadius,QtCore.SIGNAL("valueChanged(int)"),self.labelDisplayRadiusDisplay.setNum)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeSurfaceEditor)
        DialogVolumeSurfaceEditor.setTabOrder(self.radioButtonIsoSurface,self.radioButtonCrossSection)
        DialogVolumeSurfaceEditor.setTabOrder(self.radioButtonCrossSection,self.radioButtonSolid)
        DialogVolumeSurfaceEditor.setTabOrder(self.radioButtonSolid,self.horizontalSliderIsoLevel)
        DialogVolumeSurfaceEditor.setTabOrder(self.horizontalSliderIsoLevel,self.horizontalSliderSampling)

    def retranslateUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setWindowTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Volume - Surface Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBoxSurfaceOptions.setTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Surface Parameters:", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonIsoSurface.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Iso-surface view", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonCrossSection.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Cross-Section view", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonSolid.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Solid view", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Density Threshold:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoValueDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "128", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingInterval.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Sampling Interval:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "1", None, QtGui.QApplication.UnicodeUTF8))
        self.labelDisplayRadius.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Display Radius:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelDisplayRadiusDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "255", None, QtGui.QApplication.UnicodeUTF8))

