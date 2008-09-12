# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_scene_editor.ui'
#
# Created: Fri Sep 12 13:09:28 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogSceneEditor(object):
    def setupUi(self, DialogSceneEditor):
        DialogSceneEditor.setObjectName("DialogSceneEditor")
        DialogSceneEditor.resize(QtCore.QSize(QtCore.QRect(0,0,456,410).size()).expandedTo(DialogSceneEditor.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogSceneEditor)
        self.gridlayout.setObjectName("gridlayout")

        self.tabWidget = QtGui.QTabWidget(DialogSceneEditor)
        self.tabWidget.setObjectName("tabWidget")

        self.tabCamera = QtGui.QWidget()
        self.tabCamera.setObjectName("tabCamera")

        self.gridlayout1 = QtGui.QGridLayout(self.tabCamera)
        self.gridlayout1.setObjectName("gridlayout1")

        self.gridlayout2 = QtGui.QGridLayout()
        self.gridlayout2.setVerticalSpacing(12)
        self.gridlayout2.setObjectName("gridlayout2")

        self.label_3 = QtGui.QLabel(self.tabCamera)
        self.label_3.setObjectName("label_3")
        self.gridlayout2.addWidget(self.label_3,0,0,1,1)

        self.doubleSpinBoxEyeX = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxEyeX.setMinimum(-9999999999.99)
        self.doubleSpinBoxEyeX.setMaximum(9999999999.99)
        self.doubleSpinBoxEyeX.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxEyeX.setObjectName("doubleSpinBoxEyeX")
        self.gridlayout2.addWidget(self.doubleSpinBoxEyeX,0,1,1,1)

        self.doubleSpinBoxEyeY = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxEyeY.setMinimum(-9999999999.99)
        self.doubleSpinBoxEyeY.setMaximum(9999999999.99)
        self.doubleSpinBoxEyeY.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxEyeY.setObjectName("doubleSpinBoxEyeY")
        self.gridlayout2.addWidget(self.doubleSpinBoxEyeY,0,2,1,1)

        self.doubleSpinBoxEyeZ = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxEyeZ.setMinimum(-9999999999.99)
        self.doubleSpinBoxEyeZ.setMaximum(9999999999.99)
        self.doubleSpinBoxEyeZ.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxEyeZ.setObjectName("doubleSpinBoxEyeZ")
        self.gridlayout2.addWidget(self.doubleSpinBoxEyeZ,0,3,1,1)

        self.label_4 = QtGui.QLabel(self.tabCamera)
        self.label_4.setObjectName("label_4")
        self.gridlayout2.addWidget(self.label_4,1,0,1,1)

        self.doubleSpinBoxCenterX = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxCenterX.setMinimum(-9999999999.99)
        self.doubleSpinBoxCenterX.setMaximum(9999999999.99)
        self.doubleSpinBoxCenterX.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxCenterX.setObjectName("doubleSpinBoxCenterX")
        self.gridlayout2.addWidget(self.doubleSpinBoxCenterX,1,1,1,1)

        self.doubleSpinBoxCenterY = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxCenterY.setMinimum(-9999999999.99)
        self.doubleSpinBoxCenterY.setMaximum(9999999999.99)
        self.doubleSpinBoxCenterY.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxCenterY.setObjectName("doubleSpinBoxCenterY")
        self.gridlayout2.addWidget(self.doubleSpinBoxCenterY,1,2,1,1)

        self.doubleSpinBoxCenterZ = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxCenterZ.setMinimum(-9999999999.99)
        self.doubleSpinBoxCenterZ.setMaximum(9999999999.99)
        self.doubleSpinBoxCenterZ.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxCenterZ.setObjectName("doubleSpinBoxCenterZ")
        self.gridlayout2.addWidget(self.doubleSpinBoxCenterZ,1,3,1,1)

        self.label_19 = QtGui.QLabel(self.tabCamera)
        self.label_19.setObjectName("label_19")
        self.gridlayout2.addWidget(self.label_19,2,0,1,1)

        self.doubleSpinBoxUpX = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxUpX.setMinimum(-9999999999.99)
        self.doubleSpinBoxUpX.setMaximum(9999999999.99)
        self.doubleSpinBoxUpX.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxUpX.setObjectName("doubleSpinBoxUpX")
        self.gridlayout2.addWidget(self.doubleSpinBoxUpX,2,1,1,1)

        self.doubleSpinBoxUpY = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxUpY.setMinimum(-9999999999.99)
        self.doubleSpinBoxUpY.setMaximum(9999999999.99)
        self.doubleSpinBoxUpY.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxUpY.setObjectName("doubleSpinBoxUpY")
        self.gridlayout2.addWidget(self.doubleSpinBoxUpY,2,2,1,1)

        self.doubleSpinBoxUpZ = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxUpZ.setMinimum(-9999999999.99)
        self.doubleSpinBoxUpZ.setMaximum(9999999999.99)
        self.doubleSpinBoxUpZ.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxUpZ.setObjectName("doubleSpinBoxUpZ")
        self.gridlayout2.addWidget(self.doubleSpinBoxUpZ,2,3,1,1)

        self.label_20 = QtGui.QLabel(self.tabCamera)
        self.label_20.setObjectName("label_20")
        self.gridlayout2.addWidget(self.label_20,3,0,1,1)

        self.doubleSpinBoxZoom = QtGui.QDoubleSpinBox(self.tabCamera)
        self.doubleSpinBoxZoom.setDecimals(4)
        self.doubleSpinBoxZoom.setMinimum(0.0001)
        self.doubleSpinBoxZoom.setMaximum(0.9999)
        self.doubleSpinBoxZoom.setSingleStep(0.1)
        self.doubleSpinBoxZoom.setProperty("value",QtCore.QVariant(0.25))
        self.doubleSpinBoxZoom.setObjectName("doubleSpinBoxZoom")
        self.gridlayout2.addWidget(self.doubleSpinBoxZoom,3,1,1,1)
        self.gridlayout1.addLayout(self.gridlayout2,0,0,1,1)

        spacerItem = QtGui.QSpacerItem(20,221,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout1.addItem(spacerItem,1,0,1,1)
        self.tabWidget.addTab(self.tabCamera,"")

        self.tabLighting = QtGui.QWidget()
        self.tabLighting.setObjectName("tabLighting")

        self.gridlayout3 = QtGui.QGridLayout(self.tabLighting)
        self.gridlayout3.setObjectName("gridlayout3")

        self.groupBox_4 = QtGui.QGroupBox(self.tabLighting)
        self.groupBox_4.setObjectName("groupBox_4")

        self.gridlayout4 = QtGui.QGridLayout(self.groupBox_4)
        self.gridlayout4.setObjectName("gridlayout4")

        self.label_2 = QtGui.QLabel(self.groupBox_4)
        self.label_2.setObjectName("label_2")
        self.gridlayout4.addWidget(self.label_2,1,0,1,1)

        self.doubleSpinBoxLight1X = QtGui.QDoubleSpinBox(self.groupBox_4)
        self.doubleSpinBoxLight1X.setEnabled(False)
        self.doubleSpinBoxLight1X.setMinimum(-9999999999.99)
        self.doubleSpinBoxLight1X.setMaximum(9999999999.99)
        self.doubleSpinBoxLight1X.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxLight1X.setObjectName("doubleSpinBoxLight1X")
        self.gridlayout4.addWidget(self.doubleSpinBoxLight1X,1,1,1,1)

        self.doubleSpinBoxLight1Y = QtGui.QDoubleSpinBox(self.groupBox_4)
        self.doubleSpinBoxLight1Y.setEnabled(False)
        self.doubleSpinBoxLight1Y.setMinimum(-9999999999.99)
        self.doubleSpinBoxLight1Y.setMaximum(9999999999.99)
        self.doubleSpinBoxLight1Y.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxLight1Y.setObjectName("doubleSpinBoxLight1Y")
        self.gridlayout4.addWidget(self.doubleSpinBoxLight1Y,1,2,1,1)

        self.doubleSpinBoxLight1Z = QtGui.QDoubleSpinBox(self.groupBox_4)
        self.doubleSpinBoxLight1Z.setEnabled(False)
        self.doubleSpinBoxLight1Z.setMinimum(-9999999999.99)
        self.doubleSpinBoxLight1Z.setMaximum(9999999999.99)
        self.doubleSpinBoxLight1Z.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxLight1Z.setObjectName("doubleSpinBoxLight1Z")
        self.gridlayout4.addWidget(self.doubleSpinBoxLight1Z,1,3,1,1)

        self.label = QtGui.QLabel(self.groupBox_4)
        self.label.setObjectName("label")
        self.gridlayout4.addWidget(self.label,2,0,1,1)

        self.pushButtonLight1Color = ColoredPushButton(self.groupBox_4)
        self.pushButtonLight1Color.setObjectName("pushButtonLight1Color")
        self.gridlayout4.addWidget(self.pushButtonLight1Color,2,1,1,1)

        self.checkBoxLight1Enabled = QtGui.QCheckBox(self.groupBox_4)
        self.checkBoxLight1Enabled.setChecked(True)
        self.checkBoxLight1Enabled.setObjectName("checkBoxLight1Enabled")
        self.gridlayout4.addWidget(self.checkBoxLight1Enabled,0,0,1,1)

        self.checkBoxLight1EyePosition = QtGui.QCheckBox(self.groupBox_4)
        self.checkBoxLight1EyePosition.setChecked(True)
        self.checkBoxLight1EyePosition.setObjectName("checkBoxLight1EyePosition")
        self.gridlayout4.addWidget(self.checkBoxLight1EyePosition,0,1,1,1)
        self.gridlayout3.addWidget(self.groupBox_4,0,0,1,2)

        self.groupBox_5 = QtGui.QGroupBox(self.tabLighting)
        self.groupBox_5.setObjectName("groupBox_5")

        self.gridlayout5 = QtGui.QGridLayout(self.groupBox_5)
        self.gridlayout5.setObjectName("gridlayout5")

        self.checkBoxLight2Enabled = QtGui.QCheckBox(self.groupBox_5)
        self.checkBoxLight2Enabled.setObjectName("checkBoxLight2Enabled")
        self.gridlayout5.addWidget(self.checkBoxLight2Enabled,0,0,1,1)

        self.label_5 = QtGui.QLabel(self.groupBox_5)
        self.label_5.setObjectName("label_5")
        self.gridlayout5.addWidget(self.label_5,1,0,1,1)

        self.doubleSpinBoxLight2X = QtGui.QDoubleSpinBox(self.groupBox_5)
        self.doubleSpinBoxLight2X.setEnabled(False)
        self.doubleSpinBoxLight2X.setMinimum(-9999999999.99)
        self.doubleSpinBoxLight2X.setMaximum(9999999999.99)
        self.doubleSpinBoxLight2X.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxLight2X.setObjectName("doubleSpinBoxLight2X")
        self.gridlayout5.addWidget(self.doubleSpinBoxLight2X,1,1,1,1)

        self.doubleSpinBoxLight2Y = QtGui.QDoubleSpinBox(self.groupBox_5)
        self.doubleSpinBoxLight2Y.setEnabled(False)
        self.doubleSpinBoxLight2Y.setMinimum(-9999999999.99)
        self.doubleSpinBoxLight2Y.setMaximum(9999999999.99)
        self.doubleSpinBoxLight2Y.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxLight2Y.setObjectName("doubleSpinBoxLight2Y")
        self.gridlayout5.addWidget(self.doubleSpinBoxLight2Y,1,2,1,1)

        self.doubleSpinBoxLight2Z = QtGui.QDoubleSpinBox(self.groupBox_5)
        self.doubleSpinBoxLight2Z.setEnabled(False)
        self.doubleSpinBoxLight2Z.setMinimum(-9999999999.99)
        self.doubleSpinBoxLight2Z.setMaximum(9999999999.99)
        self.doubleSpinBoxLight2Z.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxLight2Z.setObjectName("doubleSpinBoxLight2Z")
        self.gridlayout5.addWidget(self.doubleSpinBoxLight2Z,1,3,1,1)

        self.label_6 = QtGui.QLabel(self.groupBox_5)
        self.label_6.setObjectName("label_6")
        self.gridlayout5.addWidget(self.label_6,2,0,1,1)

        self.pushButtonLight2Color = ColoredPushButton(self.groupBox_5)
        self.pushButtonLight2Color.setEnabled(False)
        self.pushButtonLight2Color.setObjectName("pushButtonLight2Color")
        self.gridlayout5.addWidget(self.pushButtonLight2Color,2,1,1,1)
        self.gridlayout3.addWidget(self.groupBox_5,1,0,1,2)

        self.groupBox_7 = QtGui.QGroupBox(self.tabLighting)
        self.groupBox_7.setObjectName("groupBox_7")

        self.gridlayout6 = QtGui.QGridLayout(self.groupBox_7)
        self.gridlayout6.setObjectName("gridlayout6")

        self.label_10 = QtGui.QLabel(self.groupBox_7)
        self.label_10.setObjectName("label_10")
        self.gridlayout6.addWidget(self.label_10,0,0,1,1)

        self.pushButtonBackgroundColor = ColoredPushButton(self.groupBox_7)
        self.pushButtonBackgroundColor.setObjectName("pushButtonBackgroundColor")
        self.gridlayout6.addWidget(self.pushButtonBackgroundColor,0,1,1,1)

        spacerItem1 = QtGui.QSpacerItem(20,40,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout6.addItem(spacerItem1,1,0,1,2)
        self.gridlayout3.addWidget(self.groupBox_7,2,0,1,1)

        self.groupBox_6 = QtGui.QGroupBox(self.tabLighting)
        self.groupBox_6.setObjectName("groupBox_6")

        self.gridlayout7 = QtGui.QGridLayout(self.groupBox_6)
        self.gridlayout7.setObjectName("gridlayout7")

        self.checkBoxFogEnabled = QtGui.QCheckBox(self.groupBox_6)
        self.checkBoxFogEnabled.setObjectName("checkBoxFogEnabled")
        self.gridlayout7.addWidget(self.checkBoxFogEnabled,0,0,1,1)

        self.label_7 = QtGui.QLabel(self.groupBox_6)
        self.label_7.setObjectName("label_7")
        self.gridlayout7.addWidget(self.label_7,1,0,1,1)

        self.doubleSpinBoxFogDensity = QtGui.QDoubleSpinBox(self.groupBox_6)
        self.doubleSpinBoxFogDensity.setDecimals(4)
        self.doubleSpinBoxFogDensity.setMinimum(0.0)
        self.doubleSpinBoxFogDensity.setMaximum(1.0)
        self.doubleSpinBoxFogDensity.setSingleStep(0.01)
        self.doubleSpinBoxFogDensity.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxFogDensity.setObjectName("doubleSpinBoxFogDensity")
        self.gridlayout7.addWidget(self.doubleSpinBoxFogDensity,1,1,1,1)

        self.label_8 = QtGui.QLabel(self.groupBox_6)
        self.label_8.setObjectName("label_8")
        self.gridlayout7.addWidget(self.label_8,2,0,1,1)

        self.pushButtonFogColor = ColoredPushButton(self.groupBox_6)
        self.pushButtonFogColor.setObjectName("pushButtonFogColor")
        self.gridlayout7.addWidget(self.pushButtonFogColor,2,1,1,1)
        self.gridlayout3.addWidget(self.groupBox_6,2,1,1,1)
        self.tabWidget.addTab(self.tabLighting,"")
        self.gridlayout.addWidget(self.tabWidget,0,0,1,1)

        self.retranslateUi(DialogSceneEditor)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QObject.connect(self.checkBoxFogEnabled,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxFogDensity.setEnabled)
        QtCore.QObject.connect(self.checkBoxFogEnabled,QtCore.SIGNAL("toggled(bool)"),self.pushButtonFogColor.setEnabled)
        QtCore.QObject.connect(self.checkBoxLight2Enabled,QtCore.SIGNAL("toggled(bool)"),self.pushButtonLight2Color.setEnabled)
        QtCore.QObject.connect(self.checkBoxLight2Enabled,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxLight2Y.setEnabled)
        QtCore.QObject.connect(self.checkBoxLight2Enabled,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxLight2X.setEnabled)
        QtCore.QObject.connect(self.checkBoxLight2Enabled,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxLight2Z.setEnabled)
        QtCore.QObject.connect(self.checkBoxLight1EyePosition,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxLight1X.setDisabled)
        QtCore.QObject.connect(self.checkBoxLight1EyePosition,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxLight1Y.setDisabled)
        QtCore.QObject.connect(self.checkBoxLight1EyePosition,QtCore.SIGNAL("toggled(bool)"),self.doubleSpinBoxLight1Z.setDisabled)
        QtCore.QObject.connect(self.checkBoxLight1Enabled,QtCore.SIGNAL("toggled(bool)"),self.pushButtonLight1Color.setEnabled)
        QtCore.QObject.connect(self.checkBoxLight1Enabled,QtCore.SIGNAL("toggled(bool)"),self.checkBoxLight1EyePosition.setEnabled)
        QtCore.QMetaObject.connectSlotsByName(DialogSceneEditor)
        DialogSceneEditor.setTabOrder(self.tabWidget,self.doubleSpinBoxEyeX)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxEyeX,self.doubleSpinBoxEyeY)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxEyeY,self.doubleSpinBoxEyeZ)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxEyeZ,self.doubleSpinBoxCenterX)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxCenterX,self.doubleSpinBoxCenterY)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxCenterY,self.doubleSpinBoxCenterZ)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxCenterZ,self.doubleSpinBoxUpX)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxUpX,self.doubleSpinBoxUpY)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxUpY,self.doubleSpinBoxUpZ)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxUpZ,self.doubleSpinBoxZoom)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxZoom,self.checkBoxLight1Enabled)
        DialogSceneEditor.setTabOrder(self.checkBoxLight1Enabled,self.checkBoxLight1EyePosition)
        DialogSceneEditor.setTabOrder(self.checkBoxLight1EyePosition,self.doubleSpinBoxLight1X)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxLight1X,self.doubleSpinBoxLight1Y)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxLight1Y,self.doubleSpinBoxLight1Z)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxLight1Z,self.pushButtonLight1Color)
        DialogSceneEditor.setTabOrder(self.pushButtonLight1Color,self.checkBoxLight2Enabled)
        DialogSceneEditor.setTabOrder(self.checkBoxLight2Enabled,self.doubleSpinBoxLight2X)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxLight2X,self.doubleSpinBoxLight2Y)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxLight2Y,self.doubleSpinBoxLight2Z)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxLight2Z,self.pushButtonLight2Color)
        DialogSceneEditor.setTabOrder(self.pushButtonLight2Color,self.pushButtonBackgroundColor)
        DialogSceneEditor.setTabOrder(self.pushButtonBackgroundColor,self.checkBoxFogEnabled)
        DialogSceneEditor.setTabOrder(self.checkBoxFogEnabled,self.doubleSpinBoxFogDensity)
        DialogSceneEditor.setTabOrder(self.doubleSpinBoxFogDensity,self.pushButtonFogColor)

    def retranslateUi(self, DialogSceneEditor):
        DialogSceneEditor.setWindowTitle(QtGui.QApplication.translate("DialogSceneEditor", "Scene Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("DialogSceneEditor", "Eye:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("DialogSceneEditor", "Center:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_19.setText(QtGui.QApplication.translate("DialogSceneEditor", "Up:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_20.setText(QtGui.QApplication.translate("DialogSceneEditor", "Zoom:", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabCamera), QtGui.QApplication.translate("DialogSceneEditor", "Camera", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_4.setTitle(QtGui.QApplication.translate("DialogSceneEditor", "Light 1:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("DialogSceneEditor", "Position:", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("DialogSceneEditor", "Color:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxLight1Enabled.setText(QtGui.QApplication.translate("DialogSceneEditor", "Enabled", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxLight1EyePosition.setText(QtGui.QApplication.translate("DialogSceneEditor", "Use Eye Position", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_5.setTitle(QtGui.QApplication.translate("DialogSceneEditor", "Light 2:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxLight2Enabled.setText(QtGui.QApplication.translate("DialogSceneEditor", "Enabled", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("DialogSceneEditor", "Position:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("DialogSceneEditor", "Color:", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_7.setTitle(QtGui.QApplication.translate("DialogSceneEditor", "Background:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("DialogSceneEditor", "Color:", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_6.setTitle(QtGui.QApplication.translate("DialogSceneEditor", "Fog:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxFogEnabled.setText(QtGui.QApplication.translate("DialogSceneEditor", "Enabled", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("DialogSceneEditor", "Density:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("DialogSceneEditor", "Color:", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabLighting), QtGui.QApplication.translate("DialogSceneEditor", "Lighting", None, QtGui.QApplication.UnicodeUTF8))

from colored_push_button import ColoredPushButton
