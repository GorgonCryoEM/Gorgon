# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_calpha_choose_chain_to_load.ui'
#
# Created: Fri Apr 03 13:28:12 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogCAlphaChooseChainToLoad(object):
    def setupUi(self, DialogCAlphaChooseChainToLoad):
        DialogCAlphaChooseChainToLoad.setObjectName("DialogCAlphaChooseChainToLoad")
        DialogCAlphaChooseChainToLoad.resize(QtCore.QSize(QtCore.QRect(0,0,357,300).size()).expandedTo(DialogCAlphaChooseChainToLoad.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogCAlphaChooseChainToLoad)
        self.gridlayout.setObjectName("gridlayout")

        self.labelDescription = QtGui.QLabel(DialogCAlphaChooseChainToLoad)
        self.labelDescription.setObjectName("labelDescription")
        self.gridlayout.addWidget(self.labelDescription,0,0,1,1)

        self.listWidgetChainIDList = QtGui.QListWidget(DialogCAlphaChooseChainToLoad)
        self.listWidgetChainIDList.setObjectName("listWidgetChainIDList")
        self.gridlayout.addWidget(self.listWidgetChainIDList,1,0,1,1)

        self.buttonBox = QtGui.QDialogButtonBox(DialogCAlphaChooseChainToLoad)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,2,0,1,1)

        self.retranslateUi(DialogCAlphaChooseChainToLoad)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogCAlphaChooseChainToLoad.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogCAlphaChooseChainToLoad.reject)
        QtCore.QMetaObject.connectSlotsByName(DialogCAlphaChooseChainToLoad)

    def retranslateUi(self, DialogCAlphaChooseChainToLoad):
        DialogCAlphaChooseChainToLoad.setWindowTitle(QtGui.QApplication.translate("DialogCAlphaChooseChainToLoad", "Load Chain", None, QtGui.QApplication.UnicodeUTF8))
        self.labelDescription.setText(QtGui.QApplication.translate("DialogCAlphaChooseChainToLoad", "Choose the chain that you want to load", None, QtGui.QApplication.UnicodeUTF8))

