# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_model_visualization.ui'
#
# Created: Mon Apr 07 15:53:55 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogModelVisualization(object):
    def setupUi(self, DialogModelVisualization):
        DialogModelVisualization.setObjectName("DialogModelVisualization")
        DialogModelVisualization.resize(QtCore.QSize(QtCore.QRect(0,0,222,133).size()).expandedTo(DialogModelVisualization.minimumSizeHint()))
        DialogModelVisualization.setMinimumSize(QtCore.QSize(222,133))
        DialogModelVisualization.setMaximumSize(QtCore.QSize(222,133))

        self.gridlayout = QtGui.QGridLayout(DialogModelVisualization)
        self.gridlayout.setObjectName("gridlayout")

        self.groupBoxDisplayStyle = QtGui.QGroupBox(DialogModelVisualization)
        self.groupBoxDisplayStyle.setObjectName("groupBoxDisplayStyle")

        self.gridlayout1 = QtGui.QGridLayout(self.groupBoxDisplayStyle)
        self.gridlayout1.setObjectName("gridlayout1")

        self.radioButtonWireframe = QtGui.QRadioButton(self.groupBoxDisplayStyle)
        self.radioButtonWireframe.setObjectName("radioButtonWireframe")
        self.gridlayout1.addWidget(self.radioButtonWireframe,0,0,1,1)

        self.radioButtonFlat = QtGui.QRadioButton(self.groupBoxDisplayStyle)
        self.radioButtonFlat.setObjectName("radioButtonFlat")
        self.gridlayout1.addWidget(self.radioButtonFlat,0,1,1,1)

        self.radioButtonSmooth = QtGui.QRadioButton(self.groupBoxDisplayStyle)
        self.radioButtonSmooth.setChecked(True)
        self.radioButtonSmooth.setObjectName("radioButtonSmooth")
        self.gridlayout1.addWidget(self.radioButtonSmooth,0,2,1,1)
        self.gridlayout.addWidget(self.groupBoxDisplayStyle,0,0,1,2)

        self.checkBoxBoundingBox = QtGui.QCheckBox(DialogModelVisualization)
        self.checkBoxBoundingBox.setChecked(True)
        self.checkBoxBoundingBox.setObjectName("checkBoxBoundingBox")
        self.gridlayout.addWidget(self.checkBoxBoundingBox,1,0,1,1)

        self.pushButtonBoundingBoxColor = QtGui.QPushButton(DialogModelVisualization)
        self.pushButtonBoundingBoxColor.setObjectName("pushButtonBoundingBoxColor")
        self.gridlayout.addWidget(self.pushButtonBoundingBoxColor,1,1,1,1)

        self.checkBoxModelVisible = QtGui.QCheckBox(DialogModelVisualization)
        self.checkBoxModelVisible.setChecked(True)
        self.checkBoxModelVisible.setObjectName("checkBoxModelVisible")
        self.gridlayout.addWidget(self.checkBoxModelVisible,2,0,1,1)

        self.pushButtonModelColor = QtGui.QPushButton(DialogModelVisualization)
        self.pushButtonModelColor.setObjectName("pushButtonModelColor")
        self.gridlayout.addWidget(self.pushButtonModelColor,2,1,1,1)

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

