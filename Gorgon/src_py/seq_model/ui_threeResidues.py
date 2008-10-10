# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_threeResidues.ui'
#
# Created: Wed Oct  8 17:24:58 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_threeResidues(object):
    def setupUi(self, threeResidues):
        threeResidues.setObjectName("threeResidues")
        threeResidues.resize(QtCore.QSize(QtCore.QRect(0,0,559,264).size()).expandedTo(threeResidues.minimumSizeHint()))

        self.layoutWidget = QtGui.QWidget(threeResidues)
        self.layoutWidget.setGeometry(QtCore.QRect(0,0,551,251))
        self.layoutWidget.setObjectName("layoutWidget")

        self.hboxlayout = QtGui.QHBoxLayout(self.layoutWidget)
        self.hboxlayout.setSizeConstraint(QtGui.QLayout.SetDefaultConstraint)
        self.hboxlayout.setObjectName("hboxlayout")

        self.vboxlayout = QtGui.QVBoxLayout()
        self.vboxlayout.setSpacing(6)
        self.vboxlayout.setObjectName("vboxlayout")

        self.vboxlayout1 = QtGui.QVBoxLayout()
        self.vboxlayout1.setObjectName("vboxlayout1")

        self.hboxlayout1 = QtGui.QHBoxLayout()
        self.hboxlayout1.setObjectName("hboxlayout1")

        self.undoButton = QtGui.QToolButton(self.layoutWidget)
        self.undoButton.setFocusPolicy(QtCore.Qt.NoFocus)
        self.undoButton.setObjectName("undoButton")
        self.hboxlayout1.addWidget(self.undoButton)

        self.redoButton = QtGui.QToolButton(self.layoutWidget)
        self.redoButton.setFocusPolicy(QtCore.Qt.NoFocus)
        self.redoButton.setObjectName("redoButton")
        self.hboxlayout1.addWidget(self.redoButton)

        spacerItem = QtGui.QSpacerItem(20,51,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout1.addItem(spacerItem)
        self.vboxlayout1.addLayout(self.hboxlayout1)

        self.mockSidechainsCheckBox = QtGui.QCheckBox(self.layoutWidget)
        self.mockSidechainsCheckBox.setObjectName("mockSidechainsCheckBox")
        self.vboxlayout1.addWidget(self.mockSidechainsCheckBox)

        self.hboxlayout2 = QtGui.QHBoxLayout()
        self.hboxlayout2.setObjectName("hboxlayout2")

        self.CAdoubleSpinBox = QtGui.QDoubleSpinBox(self.layoutWidget)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.CAdoubleSpinBox.sizePolicy().hasHeightForWidth())
        self.CAdoubleSpinBox.setSizePolicy(sizePolicy)
        self.CAdoubleSpinBox.setMinimumSize(QtCore.QSize(50,0))
        self.CAdoubleSpinBox.setMaximumSize(QtCore.QSize(50,16777215))
        self.CAdoubleSpinBox.setDecimals(1)
        self.CAdoubleSpinBox.setSingleStep(0.1)
        self.CAdoubleSpinBox.setProperty("value",QtCore.QVariant(3.8))
        self.CAdoubleSpinBox.setObjectName("CAdoubleSpinBox")
        self.hboxlayout2.addWidget(self.CAdoubleSpinBox)

        self.CAlabel = QtGui.QLabel(self.layoutWidget)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.CAlabel.sizePolicy().hasHeightForWidth())
        self.CAlabel.setSizePolicy(sizePolicy)
        self.CAlabel.setObjectName("CAlabel")
        self.hboxlayout2.addWidget(self.CAlabel)
        self.vboxlayout1.addLayout(self.hboxlayout2)
        self.vboxlayout.addLayout(self.vboxlayout1)

        self.hboxlayout3 = QtGui.QHBoxLayout()
        self.hboxlayout3.setObjectName("hboxlayout3")

        spacerItem1 = QtGui.QSpacerItem(20,51,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout3.addItem(spacerItem1)

        self.acceptButton = QtGui.QPushButton(self.layoutWidget)
        self.acceptButton.setObjectName("acceptButton")
        self.hboxlayout3.addWidget(self.acceptButton)

        spacerItem2 = QtGui.QSpacerItem(20,51,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout3.addItem(spacerItem2)
        self.vboxlayout.addLayout(self.hboxlayout3)
        self.hboxlayout.addLayout(self.vboxlayout)

        self.tabWidget = QtGui.QTabWidget(self.layoutWidget)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidget.sizePolicy().hasHeightForWidth())
        self.tabWidget.setSizePolicy(sizePolicy)

        font = QtGui.QFont()
        font.setPointSize(13)
        self.tabWidget.setFont(font)
        self.tabWidget.setObjectName("tabWidget")

        self.helixTab = QtGui.QWidget()

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.helixTab.sizePolicy().hasHeightForWidth())
        self.helixTab.setSizePolicy(sizePolicy)
        self.helixTab.setObjectName("helixTab")
        self.tabWidget.addTab(self.helixTab,"")

        self.atomicTab = QtGui.QWidget()

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.atomicTab.sizePolicy().hasHeightForWidth())
        self.atomicTab.setSizePolicy(sizePolicy)
        self.atomicTab.setObjectName("atomicTab")

        self.layoutWidget_3 = QtGui.QWidget(self.atomicTab)
        self.layoutWidget_3.setGeometry(QtCore.QRect(10,0,177,216))
        self.layoutWidget_3.setObjectName("layoutWidget_3")

        self.vboxlayout2 = QtGui.QVBoxLayout(self.layoutWidget_3)
        self.vboxlayout2.setObjectName("vboxlayout2")

        self.hboxlayout4 = QtGui.QHBoxLayout()
        self.hboxlayout4.setSpacing(6)
        self.hboxlayout4.setSizeConstraint(QtGui.QLayout.SetDefaultConstraint)
        self.hboxlayout4.setContentsMargins(-1,-1,0,0)
        self.hboxlayout4.setObjectName("hboxlayout4")

        self.possibilityNumSpinBox = QtGui.QSpinBox(self.layoutWidget_3)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.possibilityNumSpinBox.sizePolicy().hasHeightForWidth())
        self.possibilityNumSpinBox.setSizePolicy(sizePolicy)
        self.possibilityNumSpinBox.setMinimumSize(QtCore.QSize(50,0))
        self.possibilityNumSpinBox.setMaximumSize(QtCore.QSize(50,16777215))
        self.possibilityNumSpinBox.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.possibilityNumSpinBox.setProperty("value",QtCore.QVariant(1))
        self.possibilityNumSpinBox.setObjectName("possibilityNumSpinBox")
        self.hboxlayout4.addWidget(self.possibilityNumSpinBox)

        self.label_4 = QtGui.QLabel(self.layoutWidget_3)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_4.sizePolicy().hasHeightForWidth())
        self.label_4.setSizePolicy(sizePolicy)
        self.label_4.setObjectName("label_4")
        self.hboxlayout4.addWidget(self.label_4)
        self.vboxlayout2.addLayout(self.hboxlayout4)

        self.hboxlayout5 = QtGui.QHBoxLayout()
        self.hboxlayout5.setObjectName("hboxlayout5")

        self.vboxlayout3 = QtGui.QVBoxLayout()
        self.vboxlayout3.setObjectName("vboxlayout3")

        self.prevNum = QtGui.QLabel(self.layoutWidget_3)
        self.prevNum.setAlignment(QtCore.Qt.AlignCenter)
        self.prevNum.setObjectName("prevNum")
        self.vboxlayout3.addWidget(self.prevNum)

        self.prevName = QtGui.QLabel(self.layoutWidget_3)

        font = QtGui.QFont()
        font.setPointSize(41)
        self.prevName.setFont(font)
        self.prevName.setAlignment(QtCore.Qt.AlignCenter)
        self.prevName.setObjectName("prevName")
        self.vboxlayout3.addWidget(self.prevName)
        self.hboxlayout5.addLayout(self.vboxlayout3)

        self.vboxlayout4 = QtGui.QVBoxLayout()
        self.vboxlayout4.setObjectName("vboxlayout4")

        self.curNum = QtGui.QLabel(self.layoutWidget_3)
        self.curNum.setAlignment(QtCore.Qt.AlignCenter)
        self.curNum.setObjectName("curNum")
        self.vboxlayout4.addWidget(self.curNum)

        self.curName = QtGui.QLabel(self.layoutWidget_3)

        font = QtGui.QFont()
        font.setPointSize(41)
        self.curName.setFont(font)
        self.curName.setAlignment(QtCore.Qt.AlignCenter)
        self.curName.setObjectName("curName")
        self.vboxlayout4.addWidget(self.curName)
        self.hboxlayout5.addLayout(self.vboxlayout4)

        self.vboxlayout5 = QtGui.QVBoxLayout()
        self.vboxlayout5.setObjectName("vboxlayout5")

        self.nextNum = QtGui.QLabel(self.layoutWidget_3)
        self.nextNum.setAlignment(QtCore.Qt.AlignCenter)
        self.nextNum.setObjectName("nextNum")
        self.vboxlayout5.addWidget(self.nextNum)

        self.nextName = QtGui.QLabel(self.layoutWidget_3)

        font = QtGui.QFont()
        font.setPointSize(41)
        self.nextName.setFont(font)
        self.nextName.setAlignment(QtCore.Qt.AlignCenter)
        self.nextName.setObjectName("nextName")
        self.vboxlayout5.addWidget(self.nextName)
        self.hboxlayout5.addLayout(self.vboxlayout5)
        self.vboxlayout2.addLayout(self.hboxlayout5)

        self.hboxlayout6 = QtGui.QHBoxLayout()
        self.hboxlayout6.setObjectName("hboxlayout6")

        self.back1resButton = QtGui.QPushButton(self.layoutWidget_3)
        self.back1resButton.setObjectName("back1resButton")
        self.hboxlayout6.addWidget(self.back1resButton)

        self.forward1resButton = QtGui.QPushButton(self.layoutWidget_3)
        self.forward1resButton.setObjectName("forward1resButton")
        self.hboxlayout6.addWidget(self.forward1resButton)
        self.vboxlayout2.addLayout(self.hboxlayout6)
        self.tabWidget.addTab(self.atomicTab,"")

        self.loopTab = QtGui.QWidget()

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.loopTab.sizePolicy().hasHeightForWidth())
        self.loopTab.setSizePolicy(sizePolicy)
        self.loopTab.setObjectName("loopTab")

        self.verticalLayout = QtGui.QWidget(self.loopTab)
        self.verticalLayout.setGeometry(QtCore.QRect(0,10,277,201))
        self.verticalLayout.setObjectName("verticalLayout")

        self.vboxlayout6 = QtGui.QVBoxLayout(self.verticalLayout)
        self.vboxlayout6.setObjectName("vboxlayout6")

        self.hboxlayout7 = QtGui.QHBoxLayout()
        self.hboxlayout7.setObjectName("hboxlayout7")

        self.label = QtGui.QLabel(self.verticalLayout)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setObjectName("label")
        self.hboxlayout7.addWidget(self.label)

        self.spinBox = QtGui.QSpinBox(self.verticalLayout)
        self.spinBox.setObjectName("spinBox")
        self.hboxlayout7.addWidget(self.spinBox)
        self.vboxlayout6.addLayout(self.hboxlayout7)

        self.hboxlayout8 = QtGui.QHBoxLayout()
        self.hboxlayout8.setObjectName("hboxlayout8")

        self.label_2 = QtGui.QLabel(self.verticalLayout)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy)
        self.label_2.setObjectName("label_2")
        self.hboxlayout8.addWidget(self.label_2)

        self.spinBox_2 = QtGui.QSpinBox(self.verticalLayout)
        self.spinBox_2.setObjectName("spinBox_2")
        self.hboxlayout8.addWidget(self.spinBox_2)
        self.vboxlayout6.addLayout(self.hboxlayout8)

        self.hboxlayout9 = QtGui.QHBoxLayout()
        self.hboxlayout9.setObjectName("hboxlayout9")

        self.label_3 = QtGui.QLabel(self.verticalLayout)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)
        self.label_3.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_3.setObjectName("label_3")
        self.hboxlayout9.addWidget(self.label_3)
        self.vboxlayout6.addLayout(self.hboxlayout9)

        self.hboxlayout10 = QtGui.QHBoxLayout()
        self.hboxlayout10.setObjectName("hboxlayout10")

        self.pushButton = QtGui.QPushButton(self.verticalLayout)
        self.pushButton.setObjectName("pushButton")
        self.hboxlayout10.addWidget(self.pushButton)

        self.comboBox = QtGui.QComboBox(self.verticalLayout)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.comboBox.sizePolicy().hasHeightForWidth())
        self.comboBox.setSizePolicy(sizePolicy)
        self.comboBox.setObjectName("comboBox")
        self.hboxlayout10.addWidget(self.comboBox)
        self.vboxlayout6.addLayout(self.hboxlayout10)

        self.hboxlayout11 = QtGui.QHBoxLayout()
        self.hboxlayout11.setObjectName("hboxlayout11")

        self.pushButton_3 = QtGui.QPushButton(self.verticalLayout)
        self.pushButton_3.setObjectName("pushButton_3")
        self.hboxlayout11.addWidget(self.pushButton_3)

        self.pushButton_2 = QtGui.QPushButton(self.verticalLayout)
        self.pushButton_2.setObjectName("pushButton_2")
        self.hboxlayout11.addWidget(self.pushButton_2)
        self.vboxlayout6.addLayout(self.hboxlayout11)
        self.tabWidget.addTab(self.loopTab,"")
        self.hboxlayout.addWidget(self.tabWidget)

        self.retranslateUi(threeResidues)
        self.tabWidget.setCurrentIndex(1)
        QtCore.QMetaObject.connectSlotsByName(threeResidues)

    def retranslateUi(self, threeResidues):
        threeResidues.setWindowTitle(QtGui.QApplication.translate("threeResidues", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.undoButton.setText(QtGui.QApplication.translate("threeResidues", "Undo", None, QtGui.QApplication.UnicodeUTF8))
        self.redoButton.setText(QtGui.QApplication.translate("threeResidues", "Redo", None, QtGui.QApplication.UnicodeUTF8))
        self.mockSidechainsCheckBox.setText(QtGui.QApplication.translate("threeResidues", "Mock Sidechains", None, QtGui.QApplication.UnicodeUTF8))
        self.CAlabel.setText(QtGui.QApplication.translate("threeResidues", "C-Alpha Interval", None, QtGui.QApplication.UnicodeUTF8))
        self.acceptButton.setText(QtGui.QApplication.translate("threeResidues", "Accept", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.helixTab), QtGui.QApplication.translate("threeResidues", "Helix Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("threeResidues", "of 3", None, QtGui.QApplication.UnicodeUTF8))
        self.prevNum.setText(QtGui.QApplication.translate("threeResidues", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Lucida Grande\'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#666666;\">167</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.prevName.setText(QtGui.QApplication.translate("threeResidues", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Lucida Grande\'; font-size:41pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#666666;\">A</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.curNum.setText(QtGui.QApplication.translate("threeResidues", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Lucida Grande\'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#ffff00;\">168</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.curName.setText(QtGui.QApplication.translate("threeResidues", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Lucida Grande\'; font-size:41pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#ffff00;\">Q</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.nextNum.setText(QtGui.QApplication.translate("threeResidues", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Lucida Grande\'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#008000;\">169</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.nextName.setText(QtGui.QApplication.translate("threeResidues", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Lucida Grande\'; font-size:41pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#008000;\">F</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.back1resButton.setText(QtGui.QApplication.translate("threeResidues", "<-", None, QtGui.QApplication.UnicodeUTF8))
        self.forward1resButton.setText(QtGui.QApplication.translate("threeResidues", "->", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.atomicTab), QtGui.QApplication.translate("threeResidues", "Atomic Editor", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("threeResidues", "Start Residue", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("threeResidues", "Stop Residue", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("threeResidues", "Loop ID/ Score", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton.setText(QtGui.QApplication.translate("threeResidues", "Find Loops", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_3.setText(QtGui.QApplication.translate("threeResidues", "Accept Loop", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_2.setText(QtGui.QApplication.translate("threeResidues", "Reject Loop", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.loopTab), QtGui.QApplication.translate("threeResidues", "Loop Editor", None, QtGui.QApplication.UnicodeUTF8))

