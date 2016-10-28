# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_surface_editor.ui'
#
# Created: Sat Aug 27 20:22:16 2011
#      by: PyQt4 UI code generator 4.8.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s


class Ui_DialogVolumeSurfaceEditor(object):

    def setupUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setObjectName(_fromUtf8("DialogVolumeSurfaceEditor"))
        DialogVolumeSurfaceEditor.resize(625, 305)
        DialogVolumeSurfaceEditor.setMinimumSize(QtCore.QSize(230, 124))
        self.verticalLayout = QtGui.QVBoxLayout(DialogVolumeSurfaceEditor)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.verticalLayout.addLayout(self.horizontalLayout_3)
        self.histogram = HistogramSliderWidget(DialogVolumeSurfaceEditor)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(1)
        sizePolicy.setVerticalStretch(1)
        sizePolicy.setHeightForWidth(self.histogram.sizePolicy().hasHeightForWidth())
        self.histogram.setSizePolicy(sizePolicy)
        self.histogram.setMinimumSize(QtCore.QSize(20, 40))
        self.histogram.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.histogram.setObjectName(_fromUtf8("histogram"))
        self.verticalLayout.addWidget(self.histogram)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.labelIsoLevel = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelIsoLevel.setObjectName(_fromUtf8("labelIsoLevel"))
        self.horizontalLayout.addWidget(self.labelIsoLevel)
        self.doubleSpinBoxDensity = QtGui.QDoubleSpinBox(DialogVolumeSurfaceEditor)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.doubleSpinBoxDensity.sizePolicy().hasHeightForWidth())
        self.doubleSpinBoxDensity.setSizePolicy(sizePolicy)
        self.doubleSpinBoxDensity.setDecimals(3)
        self.doubleSpinBoxDensity.setMaximum(25500.0)
        self.doubleSpinBoxDensity.setObjectName(_fromUtf8("doubleSpinBoxDensity"))
        self.horizontalLayout.addWidget(self.doubleSpinBoxDensity)
        self.labelSamplingInterval = QtGui.QLabel(DialogVolumeSurfaceEditor)
        self.labelSamplingInterval.setObjectName(_fromUtf8("labelSamplingInterval"))
        self.horizontalLayout.addWidget(self.labelSamplingInterval)
        self.comboBoxSamplingInterval = QtGui.QComboBox(DialogVolumeSurfaceEditor)
        self.comboBoxSamplingInterval.setObjectName(_fromUtf8("comboBoxSamplingInterval"))
        self.comboBoxSamplingInterval.addItem(_fromUtf8(""))
        self.comboBoxSamplingInterval.addItem(_fromUtf8(""))
        self.comboBoxSamplingInterval.addItem(_fromUtf8(""))
        self.comboBoxSamplingInterval.addItem(_fromUtf8(""))
        self.comboBoxSamplingInterval.addItem(_fromUtf8(""))
        self.horizontalLayout.addWidget(self.comboBoxSamplingInterval)
        self.checkBoxUseRadius = QtGui.QCheckBox(DialogVolumeSurfaceEditor)
        self.checkBoxUseRadius.setCheckable(True)
        self.checkBoxUseRadius.setObjectName(_fromUtf8("checkBoxUseRadius"))
        self.horizontalLayout.addWidget(self.checkBoxUseRadius)
        self.spinBoxDisplayRadius = QtGui.QSpinBox(DialogVolumeSurfaceEditor)
        self.spinBoxDisplayRadius.setObjectName(_fromUtf8("spinBoxDisplayRadius"))
        self.horizontalLayout.addWidget(self.spinBoxDisplayRadius)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(DialogVolumeSurfaceEditor)
        QtCore.QObject.connect(self.checkBoxUseRadius, QtCore.SIGNAL(_fromUtf8("toggled(bool)")), self.spinBoxDisplayRadius.setEnabled)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeSurfaceEditor)

    def retranslateUi(self, DialogVolumeSurfaceEditor):
        DialogVolumeSurfaceEditor.setWindowTitle(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Volume - Surface Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.histogram.setStatusTip(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Volume density histogram", None, QtGui.QApplication.UnicodeUTF8))
        self.histogram.setWhatsThis(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "A histogram showing the distribution of density values in the volume.", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setToolTip(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "The currently selected density of the volume", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setWhatsThis(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "The currently selected density of the volume", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIsoLevel.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Density:", None, QtGui.QApplication.UnicodeUTF8))
        self.doubleSpinBoxDensity.setToolTip(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "The currently selected density of the volume", None, QtGui.QApplication.UnicodeUTF8))
        self.doubleSpinBoxDensity.setWhatsThis(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "The currently selected density of the volume", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingInterval.setToolTip(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", " Sampling rate of the density.", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingInterval.setWhatsThis(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Sampling rate of the density.", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSamplingInterval.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Sampling Interval:", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxSamplingInterval.setToolTip(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", " Sampling rate of the density.", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxSamplingInterval.setWhatsThis(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Sampling rate of the density.", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxSamplingInterval.setItemText(0, QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "1", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxSamplingInterval.setItemText(1, QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "2", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxSamplingInterval.setItemText(2, QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "4", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxSamplingInterval.setItemText(3, QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "8", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBoxSamplingInterval.setItemText(4, QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "16", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxUseRadius.setToolTip(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "The volume will be displayed for this radius around the selected point", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxUseRadius.setWhatsThis(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "The volume will be displayed for this radius around the selected point", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxUseRadius.setText(QtGui.QApplication.translate("DialogVolumeSurfaceEditor", "Display Radius:", None, QtGui.QApplication.UnicodeUTF8))

from slider_widget import HistogramSliderWidget
