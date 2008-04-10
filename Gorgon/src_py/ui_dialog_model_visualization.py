# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_model_visualization.ui'
#
# Created: Wed Apr 09 21:54:38 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogModelVisualization(object):
    def setupUi(self, DialogModelVisualization):
        DialogModelVisualization.setObjectName("DialogModelVisualization")
        DialogModelVisualization.resize(QtCore.QSize(QtCore.QRect(0,0,224,204).size()).expandedTo(DialogModelVisualization.minimumSizeHint()))
        DialogModelVisualization.setMinimumSize(QtCore.QSize(222,174))
        DialogModelVisualization.setMaximumSize(QtCore.QSize(224,204))

        self.gridlayout = QtGui.QGridLayout(DialogModelVisualization)
        self.gridlayout.setObjectName("gridlayout")

        self.gridlayout1 = QtGui.QGridLayout()
        self.gridlayout1.setObjectName("gridlayout1")

        self.groupBoxDisplayStyle = QtGui.QGroupBox(DialogModelVisualization)
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

        self.checkBoxBoundingBox = QtGui.QCheckBox(DialogModelVisualization)
        self.checkBoxBoundingBox.setChecked(True)
        self.checkBoxBoundingBox.setObjectName("checkBoxBoundingBox")
        self.gridlayout1.addWidget(self.checkBoxBoundingBox,1,0,1,1)

        self.pushButtonBoundingBoxColor = ColoredPushButton(DialogModelVisualization)
        self.pushButtonBoundingBoxColor.setObjectName("pushButtonBoundingBoxColor")
        self.gridlayout1.addWidget(self.pushButtonBoundingBoxColor,1,1,1,1)

        self.checkBoxModelVisible = QtGui.QCheckBox(DialogModelVisualization)
        self.checkBoxModelVisible.setChecked(True)
        self.checkBoxModelVisible.setObjectName("checkBoxModelVisible")
        self.gridlayout1.addWidget(self.checkBoxModelVisible,2,0,1,1)

        self.pushButtonModelColor = ColoredPushButton(DialogModelVisualization)
        self.pushButtonModelColor.setObjectName("pushButtonModelColor")
        self.gridlayout1.addWidget(self.pushButtonModelColor,2,1,1,1)

        self.checkBoxModel2Visible = QtGui.QCheckBox(DialogModelVisualization)
        self.checkBoxModel2Visible.setChecked(True)
        self.checkBoxModel2Visible.setObjectName("checkBoxModel2Visible")
        self.gridlayout1.addWidget(self.checkBoxModel2Visible,3,0,1,1)

        self.pushButtonModel2Color = ColoredPushButton(DialogModelVisualization)
        self.pushButtonModel2Color.setObjectName("pushButtonModel2Color")
        self.gridlayout1.addWidget(self.pushButtonModel2Color,3,1,1,1)

        self.line = QtGui.QFrame(DialogModelVisualization)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.gridlayout1.addWidget(self.line,4,0,1,2)
        self.gridlayout.addLayout(self.gridlayout1,0,0,1,2)

        self.pushButtonCenter = QtGui.QPushButton(DialogModelVisualization)
        self.pushButtonCenter.setObjectName("pushButtonCenter")
        self.gridlayout.addWidget(self.pushButtonCenter,1,0,1,1)

        self.pushButtonClose = QtGui.QPushButton(DialogModelVisualization)
        self.pushButtonClose.setObjectName("pushButtonClose")
        self.gridlayout.addWidget(self.pushButtonClose,1,1,1,1)

        self.retranslateUi(DialogModelVisualization)
        QtCore.QMetaObject.connectSlotsByName(DialogModelVisualization)

    def retranslateUi(self, DialogModelVisualization):
        DialogModelVisualization.setWindowTitle(QtGui.QApplication.translate("DialogModelVisualization", "Visualization", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBoxDisplayStyle.setTitle(QtGui.QApplication.translate("DialogModelVisualization", "Display Style:", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonWireframe.setText(QtGui.QApplication.translate("DialogModelVisualization", "Wireframe", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonFlat.setText(QtGui.QApplication.translate("DialogModelVisualization", "Flat", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonSmooth.setText(QtGui.QApplication.translate("DialogModelVisualization", "Smooth", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxBoundingBox.setText(QtGui.QApplication.translate("DialogModelVisualization", "Show outline box colored:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxModelVisible.setText(QtGui.QApplication.translate("DialogModelVisualization", "Show model colored:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxModel2Visible.setText(QtGui.QApplication.translate("DialogModelVisualization", "Show 2nd model colored:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonCenter.setText(QtGui.QApplication.translate("DialogModelVisualization", "Center", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonClose.setText(QtGui.QApplication.translate("DialogModelVisualization", "Close", None, QtGui.QApplication.UnicodeUTF8))

from colored_push_button import ColoredPushButton
