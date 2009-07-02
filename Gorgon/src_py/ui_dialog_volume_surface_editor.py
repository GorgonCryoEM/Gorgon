# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_surface_editor.ui'
#
# Created: Wed Jul 01 17:32:26 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeSurfaceEditor(object):
    def setupUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setObjectName("DialogVolumeSurfaceEditor")
        DialogVolumeSurfaceEditor.resize(QtCore.QSize(QtCore.QRect(0,0,399,140).size()).expandedTo(DialogVolumeSurfaceEditor.minimumSizeHint()))
        DialogVolumeSurfaceEditor.setMinimumSize(QtCore.QSize(230,124))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeSurfaceEditor)
        self.gridlayout.setObjectName("gridlayout")

        self.gridlayout1 = QtGui.QGridLayout()
        self.gridlayout1.setObjectName("gridlayout1")

        self.radioButtonIsoSurface = QtGui.QRadioButton(DialogVolumeSurfaceEditor)
        self.radioButtonIsoSurface.setChecked(True)
        self.radioButtonIsoSurface.setObjectName("radioButtonIsoSurface")
        self.gridlayout1.addWidget(self.radioButtonIsoSurface,0,0,1,1)

        self.radioButtonCrossSection = QtGui.QRadioButton(DialogVolumeSurfaceEditor)
        self.radioButtonCrossSection.setObjectName("radioButtonCrossSection")
        self.gridlayout1.addWidget(self.radioButtonCrossSection,0,1,1,1)

        self.radioButtonSolid = QtGui.QRadioButton(DialogVolumeSurfaceEditor)
        self.radioButtonSolid.setObjectName("radioButtonSolid")
        self.gridlayout1.addWidget(self.radioButtonSolid,0,2,1,1)
        self.gridlayout.addLayout(self.gridlayout1,0,0,1,2)

        self.labelIsoLevel = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelIsoLevel.setObjectName("labelIsoLevel")
        self.gridlayout.addWidget(self.labelIsoLevel,1,0,1,1)

        self.horizontalSliderIsoLevel = QtGui.QSlider(DialogVolumeSurfaceEditor)
        self.horizontalSliderIsoLevel.setMaximum(25500)
        self.horizontalSliderIsoLevel.setSingleStep(1)
        self.horizontalSliderIsoLevel.setPageStep(10)
        self.horizontalSliderIsoLevel.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderIsoLevel.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderIsoLevel.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderIsoLevel.setObjectName("horizontalSliderIsoLevel")
        self.gridlayout.addWidget(self.horizontalSliderIsoLevel,1,1,1,1)

        self.doubleSpinBoxDensity = QtGui.QDoubleSpinBox(DialogVolumeSurfaceEditor)
        self.doubleSpinBoxDensity.setDecimals(3)
        self.doubleSpinBoxDensity.setMaximum(25500.0)
        self.doubleSpinBoxDensity.setObjectName("doubleSpinBoxDensity")
        self.gridlayout.addWidget(self.doubleSpinBoxDensity,1,2,1,1)

        self.labelIsoLevelMax = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelIsoLevelMax.setObjectName("labelIsoLevelMax")
        self.gridlayout.addWidget(self.labelIsoLevelMax,2,0,1,1)

        self.horizontalSliderIsoLevelMax = QtGui.QSlider(DialogVolumeSurfaceEditor)
        self.horizontalSliderIsoLevelMax.setMaximum(25500)
        self.horizontalSliderIsoLevelMax.setSingleStep(1)
        self.horizontalSliderIsoLevelMax.setPageStep(10)
        self.horizontalSliderIsoLevelMax.setProperty("value",QtCore.QVariant(12800))
        self.horizontalSliderIsoLevelMax.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderIsoLevelMax.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderIsoLevelMax.setObjectName("horizontalSliderIsoLevelMax")
        self.gridlayout.addWidget(self.horizontalSliderIsoLevelMax,2,1,1,1)

        self.doubleSpinBoxDensityMax = QtGui.QDoubleSpinBox(DialogVolumeSurfaceEditor)
        self.doubleSpinBoxDensityMax.setDecimals(3)
        self.doubleSpinBoxDensityMax.setMaximum(25500.0)
        self.doubleSpinBoxDensityMax.setObjectName("doubleSpinBoxDensityMax")
        self.gridlayout.addWidget(self.doubleSpinBoxDensityMax,2,2,1,1)

        self.labelSamplingInterval = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelSamplingInterval.setObjectName("labelSamplingInterval")
        self.gridlayout.addWidget(self.labelSamplingInterval,3,0,1,1)

        self.horizontalSliderSampling = QtGui.QSlider(DialogVolumeSurfaceEditor)
        self.horizontalSliderSampling.setMinimum(1)
        self.horizontalSliderSampling.setMaximum(8)
        self.horizontalSliderSampling.setPageStep(1)
        self.horizontalSliderSampling.setProperty("value",QtCore.QVariant(1))
        self.horizontalSliderSampling.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderSampling.setTickPosition(QtGui.QSlider.TicksBelow)
        self.horizontalSliderSampling.setTickInterval(0)
        self.horizontalSliderSampling.setObjectName("horizontalSliderSampling")
        self.gridlayout.addWidget(self.horizontalSliderSampling,3,1,1,1)

        self.labelSamplingDisplay = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelSamplingDisplay.setObjectName("labelSamplingDisplay")
        self.gridlayout.addWidget(self.labelSamplingDisplay,3,2,1,1)

        self.checkBoxUseRadius = QtGui.QCheckBox(DialogVolumeSurfaceEditor)
        self.checkBoxUseRadius.setObjectName("checkBoxUseRadius")
        self.gridlayout.addWidget(self.checkBoxUseRadius,4,0,1,1)

        self.horizontalSliderDisplayRadius = QtGui.QSlider(DialogVolumeSurfaceEditor)
        self.horizontalSliderDisplayRadius.setEnabled(False)
        self.horizontalSliderDisplayRadius.setMinimum(1)
        self.horizontalSliderDisplayRadius.setMaximum(255)
        self.horizontalSliderDisplayRadius.setPageStep(1)
        self.horizontalSliderDisplayRadius.setProperty("value",QtCore.QVariant(255))
        self.horizontalSliderDisplayRadius.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderDisplayRadius.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderDisplayRadius.setTickInterval(0)
        self.horizontalSliderDisplayRadius.setObjectName("horizontalSliderDisplayRadius")
        self.gridlayout.addWidget(self.horizontalSliderDisplayRadius,4,1,1,1)

        self.labelDisplayRadiusDisplay = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelDisplayRadiusDisplay.setEnabled(False)
        self.labelDisplayRadiusDisplay.setObjectName("labelDisplayRadiusDisplay")
        self.gridlayout.addWidget(self.labelDisplayRadiusDisplay,4,2,1,1)

        self.retranslateUi(DialogVolumeSurfaceEditor)
        QtCore.QObject.connect(self.horizontalSliderSampling,QtCore.SIGNAL("valueChanged(int)"),self.labelSamplingDisplay.setNum)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.horizontalSliderSampling.setVisible)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.labelSamplingInterval.setVisible)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.labelSamplingDisplay.setVisible)
        QtCore.QObject.connect(self.horizontalSliderDisplayRadius,QtCore.SIGNAL("valueChanged(int)"),self.labelDisplayRadiusDisplay.setNum)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.horizontalSliderDisplayRadius.setVisible)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.labelDisplayRadiusDisplay.setVisible)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.labelIsoLevelMax.setHidden)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.horizontalSliderIsoLevelMax.setHidden)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxDensityMax.setHidden)
        QtCore.QObject.connect(self.checkBoxUseRadius,QtCore.SIGNAL("toggled(bool)"),self.horizontalSliderDisplayRadius.setEnabled)
        QtCore.QObject.connect(self.checkBoxUseRadius,QtCore.SIGNAL("toggled(bool)"),self.labelDisplayRadiusDisplay.setEnabled)
        QtCore.QObject.connect(self.radioButtonIsoSurface,QtCore.SIGNAL("toggled(bool)"),self.checkBoxUseRadius.setVisible)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeSurfaceEditor)
        DialogVolumeSurfaceEditor.setTabOrder(self.radioButtonIsoSurface,self.radioButtonCrossSection)
        DialogVolumeSurfaceEditor.setTabOrder(self.radioButtonCrossSection,self.radioButtonSolid)
        DialogVolumeSurfaceEditor.setTabOrder(self.radioButtonSolid,self.horizontalSliderIsoLevel)
        DialogVolumeSurfaceEditor.setTabOrder(self.horizontalSliderIsoLevel,self.doubleSpinBoxDensity)
        DialogVolumeSurfaceEditor.setTabOrder(self.doubleSpinBoxDensity,self.horizontalSliderIsoLevelMax)
        DialogVolumeSurfaceEditor.setTabOrder(self.horizontalSliderIsoLevelMax,self.doubleSpinBoxDensityMax)
        DialogVolumeSurfaceEditor.setTabOrder(self.doubleSpinBoxDensityMax,self.horizontalSliderSampling)
        DialogVolumeSurfaceEditor.setTabOrder(self.horizontalSliderSampling,self.checkBoxUseRadius)
        DialogVolumeSurfaceEditor.setTabOrder(self.checkBoxUseRadius,self.horizontalSliderDisplayRadius)

    def retranslateUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setWindowTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Volume - Surface Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonIsoSurface.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Iso-surface view", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonCrossSection.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Cross-Section view", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonSolid.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Solid view", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Density Threshold:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevelMax.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Maximum Density:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingInterval.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Sampling Interval:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "1", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxUseRadius.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Display Radius:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelDisplayRadiusDisplay.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "255", None, QtGui.QApplication.UnicodeUTF8))

