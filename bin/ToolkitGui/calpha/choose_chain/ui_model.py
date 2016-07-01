# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_calpha_choose_chain_model.ui'
#
# Created: Tue Mar 31 14:56:19 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogCAlphaChooseChainModel(object):
    def setupUi(self, DialogCAlphaChooseChainModel):
        DialogCAlphaChooseChainModel.setObjectName("DialogCAlphaChooseChainModel")
        DialogCAlphaChooseChainModel.resize(QtCore.QSize(QtCore.QRect(0,0,275,312).size()).expandedTo(DialogCAlphaChooseChainModel.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogCAlphaChooseChainModel)
        self.gridlayout.setObjectName("gridlayout")

        self.vboxlayout = QtGui.QVBoxLayout()
        self.vboxlayout.setObjectName("vboxlayout")

        self.chainModelsLabel = QtGui.QLabel(DialogCAlphaChooseChainModel)
        self.chainModelsLabel.setObjectName("chainModelsLabel")
        self.vboxlayout.addWidget(self.chainModelsLabel)

        self.chainModelsListWidget = QtGui.QListWidget(DialogCAlphaChooseChainModel)
        self.chainModelsListWidget.setObjectName("chainModelsListWidget")
        self.vboxlayout.addWidget(self.chainModelsListWidget)

        self.sequenceBrowserLabel = QtGui.QLabel(DialogCAlphaChooseChainModel)
        self.sequenceBrowserLabel.setObjectName("sequenceBrowserLabel")
        self.vboxlayout.addWidget(self.sequenceBrowserLabel)

        self.sequenceTextEdit = QtGui.QTextEdit(DialogCAlphaChooseChainModel)
        self.sequenceTextEdit.setReadOnly(False)
        self.sequenceTextEdit.setObjectName("sequenceTextEdit")
        self.vboxlayout.addWidget(self.sequenceTextEdit)

        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName("hboxlayout")

        self.refreshPushButton = QtGui.QPushButton(DialogCAlphaChooseChainModel)
        self.refreshPushButton.setObjectName("refreshPushButton")
        self.hboxlayout.addWidget(self.refreshPushButton)

        self.acceptPushButton = QtGui.QPushButton(DialogCAlphaChooseChainModel)
        self.acceptPushButton.setObjectName("acceptPushButton")
        self.hboxlayout.addWidget(self.acceptPushButton)

        spacerItem = QtGui.QSpacerItem(40,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout.addItem(spacerItem)
        self.vboxlayout.addLayout(self.hboxlayout)
        self.gridlayout.addLayout(self.vboxlayout,0,0,1,1)

        self.retranslateUi(DialogCAlphaChooseChainModel)
        QtCore.QMetaObject.connectSlotsByName(DialogCAlphaChooseChainModel)

    def retranslateUi(self, DialogCAlphaChooseChainModel):
        DialogCAlphaChooseChainModel.setWindowTitle(QtGui.QApplication.translate("DialogCAlphaChooseChainModel", "Choose Chain Model", None, QtGui.QApplication.UnicodeUTF8))
        self.chainModelsLabel.setText(QtGui.QApplication.translate("DialogCAlphaChooseChainModel", "Chain Models", None, QtGui.QApplication.UnicodeUTF8))
        self.sequenceBrowserLabel.setText(QtGui.QApplication.translate("DialogCAlphaChooseChainModel", "Sequence", None, QtGui.QApplication.UnicodeUTF8))
        self.refreshPushButton.setText(QtGui.QApplication.translate("DialogCAlphaChooseChainModel", "Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.acceptPushButton.setText(QtGui.QApplication.translate("DialogCAlphaChooseChainModel", "Accept", None, QtGui.QApplication.UnicodeUTF8))

