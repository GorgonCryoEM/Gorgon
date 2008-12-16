# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '../src_ui/ui_dialog_choose_chain_model.ui'
#
# Created: Tue Dec 16 17:26:39 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogChooseChainModel(object):
    def setupUi(self, DialogChooseChainModel):
        DialogChooseChainModel.setObjectName("DialogChooseChainModel")
        DialogChooseChainModel.resize(QtCore.QSize(QtCore.QRect(0,0,423,502).size()).expandedTo(DialogChooseChainModel.minimumSizeHint()))

        self.layoutWidget = QtGui.QWidget(DialogChooseChainModel)
        self.layoutWidget.setGeometry(QtCore.QRect(9,9,401,476))
        self.layoutWidget.setObjectName("layoutWidget")

        self.vboxlayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.vboxlayout.setObjectName("vboxlayout")

        self.chainModelsLabel = QtGui.QLabel(self.layoutWidget)
        self.chainModelsLabel.setObjectName("chainModelsLabel")
        self.vboxlayout.addWidget(self.chainModelsLabel)

        self.chainModelsListWidget = QtGui.QListWidget(self.layoutWidget)
        self.chainModelsListWidget.setObjectName("chainModelsListWidget")
        self.vboxlayout.addWidget(self.chainModelsListWidget)

        self.sequenceBrowserLabel = QtGui.QLabel(self.layoutWidget)
        self.sequenceBrowserLabel.setObjectName("sequenceBrowserLabel")
        self.vboxlayout.addWidget(self.sequenceBrowserLabel)

        self.sequenceBrowser = QtGui.QTextBrowser(self.layoutWidget)
        self.sequenceBrowser.setObjectName("sequenceBrowser")
        self.vboxlayout.addWidget(self.sequenceBrowser)

        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName("hboxlayout")

        self.refreshPushButton = QtGui.QPushButton(self.layoutWidget)
        self.refreshPushButton.setObjectName("refreshPushButton")
        self.hboxlayout.addWidget(self.refreshPushButton)

        self.acceptPushButton = QtGui.QPushButton(self.layoutWidget)
        self.acceptPushButton.setObjectName("acceptPushButton")
        self.hboxlayout.addWidget(self.acceptPushButton)

        spacerItem = QtGui.QSpacerItem(40,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout.addItem(spacerItem)
        self.vboxlayout.addLayout(self.hboxlayout)

        self.retranslateUi(DialogChooseChainModel)
        QtCore.QMetaObject.connectSlotsByName(DialogChooseChainModel)

    def retranslateUi(self, DialogChooseChainModel):
        DialogChooseChainModel.setWindowTitle(QtGui.QApplication.translate("DialogChooseChainModel", "Choose Chain Model", None, QtGui.QApplication.UnicodeUTF8))
        self.chainModelsLabel.setText(QtGui.QApplication.translate("DialogChooseChainModel", "Chain Models", None, QtGui.QApplication.UnicodeUTF8))
        self.sequenceBrowserLabel.setText(QtGui.QApplication.translate("DialogChooseChainModel", "Sequence", None, QtGui.QApplication.UnicodeUTF8))
        self.refreshPushButton.setText(QtGui.QApplication.translate("DialogChooseChainModel", "Refresh", None, QtGui.QApplication.UnicodeUTF8))
        self.acceptPushButton.setText(QtGui.QApplication.translate("DialogChooseChainModel", "Accept", None, QtGui.QApplication.UnicodeUTF8))

