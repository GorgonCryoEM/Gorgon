# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_model_visualization.ui'
#
# Created: Tue Apr 22 14:14:23 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogModelVisualization(object):
    def setupUi(self, DialogModelVisualization):
        DialogModelVisualization.setObjectName("DialogModelVisualization")
        DialogModelVisualization.resize(QtCore.QSize(QtCore.QRect(0,0,244,335).size()).expandedTo(DialogModelVisualization.minimumSizeHint()))
        DialogModelVisualization.setMinimumSize(QtCore.QSize(240,335))
        DialogModelVisualization.setMaximumSize(QtCore.QSize(244,335))

        self.gridlayout = QtGui.QGridLayout(DialogModelVisualization)
        self.gridlayout.setMargin(1)
        self.gridlayout.setSpacing(6)
        self.gridlayout.setObjectName("gridlayout")

        self.tabWidget = QtGui.QTabWidget(DialogModelVisualization)
        self.tabWidget.setObjectName("tabWidget")

        self.tab = QtGui.QWidget()
        self.tab.setObjectName("tab")

        self.gridlayout1 = QtGui.QGridLayout(self.tab)
        self.gridlayout1.setObjectName("gridlayout1")

        self.groupBoxDisplayStyle = QtGui.QGroupBox(self.tab)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBoxDisplayStyle.sizePolicy().hasHeightForWidth())
        self.groupBoxDisplayStyle.setSizePolicy(sizePolicy)
        self.groupBoxDisplayStyle.setObjectName("groupBoxDisplayStyle")

        self.gridlayout2 = QtGui.QGridLayout(self.groupBoxDisplayStyle)
        self.gridlayout2.setObjectName("gridlayout2")

        self.radioButtonWireframe = QtGui.QRadioButton(self.groupBoxDisplayStyle)
        self.radioButtonWireframe.setObjectName("radioButtonWireframe")
        self.gridlayout2.addWidget(self.radioButtonWireframe,0,0,1,1)

        self.radioButtonFlat = QtGui.QRadioButton(self.groupBoxDisplayStyle)
        self.radioButtonFlat.setObjectName("radioButtonFlat")
        self.gridlayout2.addWidget(self.radioButtonFlat,0,1,1,1)

        self.radioButtonSmooth = QtGui.QRadioButton(self.groupBoxDisplayStyle)
        self.radioButtonSmooth.setChecked(True)
        self.radioButtonSmooth.setObjectName("radioButtonSmooth")
        self.gridlayout2.addWidget(self.radioButtonSmooth,0,2,1,1)
        self.gridlayout1.addWidget(self.groupBoxDisplayStyle,0,0,1,2)

        self.checkBoxBoundingBox = QtGui.QCheckBox(self.tab)
        self.checkBoxBoundingBox.setChecked(True)
        self.checkBoxBoundingBox.setObjectName("checkBoxBoundingBox")
        self.gridlayout1.addWidget(self.checkBoxBoundingBox,1,0,1,1)

        self.pushButtonBoundingBoxColor = ColoredPushButton(self.tab)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButtonBoundingBoxColor.sizePolicy().hasHeightForWidth())
        self.pushButtonBoundingBoxColor.setSizePolicy(sizePolicy)
        self.pushButtonBoundingBoxColor.setObjectName("pushButtonBoundingBoxColor")
        self.gridlayout1.addWidget(self.pushButtonBoundingBoxColor,1,1,1,1)

        self.checkBoxModelVisible = QtGui.QCheckBox(self.tab)
        self.checkBoxModelVisible.setChecked(True)
        self.checkBoxModelVisible.setObjectName("checkBoxModelVisible")
        self.gridlayout1.addWidget(self.checkBoxModelVisible,2,0,1,1)

        self.pushButtonModelColor = ColoredPushButton(self.tab)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButtonModelColor.sizePolicy().hasHeightForWidth())
        self.pushButtonModelColor.setSizePolicy(sizePolicy)
        self.pushButtonModelColor.setObjectName("pushButtonModelColor")
        self.gridlayout1.addWidget(self.pushButtonModelColor,2,1,1,1)

        self.checkBoxModel2Visible = QtGui.QCheckBox(self.tab)
        self.checkBoxModel2Visible.setChecked(True)
        self.checkBoxModel2Visible.setObjectName("checkBoxModel2Visible")
        self.gridlayout1.addWidget(self.checkBoxModel2Visible,3,0,1,1)

        self.pushButtonModel2Color = ColoredPushButton(self.tab)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButtonModel2Color.sizePolicy().hasHeightForWidth())
        self.pushButtonModel2Color.setSizePolicy(sizePolicy)
        self.pushButtonModel2Color.setObjectName("pushButtonModel2Color")
        self.gridlayout1.addWidget(self.pushButtonModel2Color,3,1,1,1)
        self.tabWidget.addTab(self.tab,"")

        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName("tab_2")

        self.gridlayout3 = QtGui.QGridLayout(self.tab_2)
        self.gridlayout3.setObjectName("gridlayout3")

        self.GroupBoxLocation_2 = QtGui.QGroupBox(self.tab_2)
        self.GroupBoxLocation_2.setObjectName("GroupBoxLocation_2")

        self.gridlayout4 = QtGui.QGridLayout(self.GroupBoxLocation_2)
        self.gridlayout4.setMargin(6)
        self.gridlayout4.setSpacing(0)
        self.gridlayout4.setObjectName("gridlayout4")

        self.labelSizeX = QtGui.QLabel(self.GroupBoxLocation_2)
        self.labelSizeX.setObjectName("labelSizeX")
        self.gridlayout4.addWidget(self.labelSizeX,0,0,1,1)

        self.doubleSpinBoxSizeX = QtGui.QDoubleSpinBox(self.GroupBoxLocation_2)
        self.doubleSpinBoxSizeX.setMaximum(1e+014)
        self.doubleSpinBoxSizeX.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxSizeX.setObjectName("doubleSpinBoxSizeX")
        self.gridlayout4.addWidget(self.doubleSpinBoxSizeX,0,1,1,1)

        self.labelSizeY = QtGui.QLabel(self.GroupBoxLocation_2)
        self.labelSizeY.setObjectName("labelSizeY")
        self.gridlayout4.addWidget(self.labelSizeY,1,0,1,1)

        self.doubleSpinBoxSizeY = QtGui.QDoubleSpinBox(self.GroupBoxLocation_2)
        self.doubleSpinBoxSizeY.setMaximum(1e+014)
        self.doubleSpinBoxSizeY.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxSizeY.setObjectName("doubleSpinBoxSizeY")
        self.gridlayout4.addWidget(self.doubleSpinBoxSizeY,1,1,1,1)

        self.labelSizeZ = QtGui.QLabel(self.GroupBoxLocation_2)
        self.labelSizeZ.setObjectName("labelSizeZ")
        self.gridlayout4.addWidget(self.labelSizeZ,2,0,1,1)

        self.doubleSpinBoxSizeZ = QtGui.QDoubleSpinBox(self.GroupBoxLocation_2)
        self.doubleSpinBoxSizeZ.setMaximum(1e+014)
        self.doubleSpinBoxSizeZ.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxSizeZ.setObjectName("doubleSpinBoxSizeZ")
        self.gridlayout4.addWidget(self.doubleSpinBoxSizeZ,2,1,1,1)
        self.gridlayout3.addWidget(self.GroupBoxLocation_2,0,0,1,1)

        self.GroupBoxLocation = QtGui.QGroupBox(self.tab_2)
        self.GroupBoxLocation.setObjectName("GroupBoxLocation")

        self.gridlayout5 = QtGui.QGridLayout(self.GroupBoxLocation)
        self.gridlayout5.setMargin(6)
        self.gridlayout5.setSpacing(0)
        self.gridlayout5.setObjectName("gridlayout5")

        self.labelLocationX = QtGui.QLabel(self.GroupBoxLocation)
        self.labelLocationX.setObjectName("labelLocationX")
        self.gridlayout5.addWidget(self.labelLocationX,0,0,1,1)

        self.doubleSpinBoxLocationX = QtGui.QDoubleSpinBox(self.GroupBoxLocation)
        self.doubleSpinBoxLocationX.setMaximum(1e+014)
        self.doubleSpinBoxLocationX.setObjectName("doubleSpinBoxLocationX")
        self.gridlayout5.addWidget(self.doubleSpinBoxLocationX,0,1,1,1)

        self.labelLocationY = QtGui.QLabel(self.GroupBoxLocation)
        self.labelLocationY.setObjectName("labelLocationY")
        self.gridlayout5.addWidget(self.labelLocationY,1,0,1,1)

        self.doubleSpinBoxLocationY = QtGui.QDoubleSpinBox(self.GroupBoxLocation)
        self.doubleSpinBoxLocationY.setMaximum(1e+014)
        self.doubleSpinBoxLocationY.setObjectName("doubleSpinBoxLocationY")
        self.gridlayout5.addWidget(self.doubleSpinBoxLocationY,1,1,1,1)

        self.labelLocationZ = QtGui.QLabel(self.GroupBoxLocation)
        self.labelLocationZ.setObjectName("labelLocationZ")
        self.gridlayout5.addWidget(self.labelLocationZ,2,0,1,1)

        self.doubleSpinBoxLocationZ = QtGui.QDoubleSpinBox(self.GroupBoxLocation)
        self.doubleSpinBoxLocationZ.setMaximum(1e+014)
        self.doubleSpinBoxLocationZ.setObjectName("doubleSpinBoxLocationZ")
        self.gridlayout5.addWidget(self.doubleSpinBoxLocationZ,2,1,1,1)
        self.gridlayout3.addWidget(self.GroupBoxLocation,1,0,1,1)
        self.tabWidget.addTab(self.tab_2,"")

        self.tab_3 = QtGui.QWidget()
        self.tab_3.setObjectName("tab_3")

        self.gridlayout6 = QtGui.QGridLayout(self.tab_3)
        self.gridlayout6.setObjectName("gridlayout6")

        self.label = QtGui.QLabel(self.tab_3)
        self.label.setObjectName("label")
        self.gridlayout6.addWidget(self.label,0,0,1,1)

        self.labelModelSize = QtGui.QLabel(self.tab_3)
        self.labelModelSize.setObjectName("labelModelSize")
        self.gridlayout6.addWidget(self.labelModelSize,0,1,1,1)
        self.tabWidget.addTab(self.tab_3,"")
        self.gridlayout.addWidget(self.tabWidget,0,0,1,2)

        self.pushButtonCenter = QtGui.QPushButton(DialogModelVisualization)
        self.pushButtonCenter.setObjectName("pushButtonCenter")
        self.gridlayout.addWidget(self.pushButtonCenter,1,0,1,1)

        self.pushButtonClose = QtGui.QPushButton(DialogModelVisualization)
        self.pushButtonClose.setObjectName("pushButtonClose")
        self.gridlayout.addWidget(self.pushButtonClose,1,1,1,1)

        self.retranslateUi(DialogModelVisualization)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(DialogModelVisualization)
        DialogModelVisualization.setTabOrder(self.pushButtonCenter,self.pushButtonClose)

    def retranslateUi(self, DialogModelVisualization):
        DialogModelVisualization.setWindowTitle(QtGui.QApplication.translate("DialogModelVisualization", "Visualization", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBoxDisplayStyle.setTitle(QtGui.QApplication.translate("DialogModelVisualization", "Display Style:", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonWireframe.setText(QtGui.QApplication.translate("DialogModelVisualization", "Wireframe", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonFlat.setText(QtGui.QApplication.translate("DialogModelVisualization", "Flat", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonSmooth.setText(QtGui.QApplication.translate("DialogModelVisualization", "Smooth", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxBoundingBox.setText(QtGui.QApplication.translate("DialogModelVisualization", "Show outline box colored:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxModelVisible.setText(QtGui.QApplication.translate("DialogModelVisualization", "Show model colored:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxModel2Visible.setText(QtGui.QApplication.translate("DialogModelVisualization", "Show 2nd model colored:", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), QtGui.QApplication.translate("DialogModelVisualization", "Visualization", None, QtGui.QApplication.UnicodeUTF8))
        self.GroupBoxLocation_2.setTitle(QtGui.QApplication.translate("DialogModelVisualization", "Scale:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSizeX.setText(QtGui.QApplication.translate("DialogModelVisualization", "X:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSizeY.setText(QtGui.QApplication.translate("DialogModelVisualization", "Y:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSizeZ.setText(QtGui.QApplication.translate("DialogModelVisualization", "Z:", None, QtGui.QApplication.UnicodeUTF8))
        self.GroupBoxLocation.setTitle(QtGui.QApplication.translate("DialogModelVisualization", "Location:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelLocationX.setText(QtGui.QApplication.translate("DialogModelVisualization", "X:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelLocationY.setText(QtGui.QApplication.translate("DialogModelVisualization", "Y:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelLocationZ.setText(QtGui.QApplication.translate("DialogModelVisualization", "Z:", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), QtGui.QApplication.translate("DialogModelVisualization", "Positioning", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("DialogModelVisualization", "Model Size:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelModelSize.setText(QtGui.QApplication.translate("DialogModelVisualization", "{X, Y, Z}", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_3), QtGui.QApplication.translate("DialogModelVisualization", "Properties", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonCenter.setText(QtGui.QApplication.translate("DialogModelVisualization", "Center", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonClose.setText(QtGui.QApplication.translate("DialogModelVisualization", "Unload Model", None, QtGui.QApplication.UnicodeUTF8))

from colored_push_button import ColoredPushButton
