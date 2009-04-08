# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_plugin_browser.ui'
#
# Created: Wed Apr 08 14:10:49 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogPluginBrowser(object):
    def setupUi(self, DialogPluginBrowser):
        DialogPluginBrowser.setObjectName("DialogPluginBrowser")
        DialogPluginBrowser.resize(QtCore.QSize(QtCore.QRect(0,0,435,342).size()).expandedTo(DialogPluginBrowser.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogPluginBrowser)
        self.gridlayout.setObjectName("gridlayout")

        self.labelLoadedPlugins = QtGui.QLabel(DialogPluginBrowser)
        self.labelLoadedPlugins.setObjectName("labelLoadedPlugins")
        self.gridlayout.addWidget(self.labelLoadedPlugins,0,0,1,1)

        self.tableWidgetLoadedPlugins = QtGui.QTableWidget(DialogPluginBrowser)
        self.tableWidgetLoadedPlugins.setObjectName("tableWidgetLoadedPlugins")
        self.gridlayout.addWidget(self.tableWidgetLoadedPlugins,1,0,1,1)

        self.labelNotLoadedPlugins = QtGui.QLabel(DialogPluginBrowser)
        self.labelNotLoadedPlugins.setObjectName("labelNotLoadedPlugins")
        self.gridlayout.addWidget(self.labelNotLoadedPlugins,2,0,1,1)

        self.tableWidgetNotLoadedPlugins = QtGui.QTableWidget(DialogPluginBrowser)
        self.tableWidgetNotLoadedPlugins.setObjectName("tableWidgetNotLoadedPlugins")
        self.gridlayout.addWidget(self.tableWidgetNotLoadedPlugins,3,0,1,1)

        self.buttonBox = QtGui.QDialogButtonBox(DialogPluginBrowser)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,4,0,1,1)

        self.retranslateUi(DialogPluginBrowser)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogPluginBrowser.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogPluginBrowser.reject)
        QtCore.QMetaObject.connectSlotsByName(DialogPluginBrowser)
        DialogPluginBrowser.setTabOrder(self.tableWidgetLoadedPlugins,self.tableWidgetNotLoadedPlugins)
        DialogPluginBrowser.setTabOrder(self.tableWidgetNotLoadedPlugins,self.buttonBox)

    def retranslateUi(self, DialogPluginBrowser):
        DialogPluginBrowser.setWindowTitle(QtGui.QApplication.translate("DialogPluginBrowser", "Plugin Browser", None, QtGui.QApplication.UnicodeUTF8))
        self.labelLoadedPlugins.setText(QtGui.QApplication.translate("DialogPluginBrowser", "Loaded Plugins:", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetLoadedPlugins.setColumnCount(3)
        self.tableWidgetLoadedPlugins.clear()
        self.tableWidgetLoadedPlugins.setColumnCount(3)
        self.tableWidgetLoadedPlugins.setRowCount(0)

        headerItem = QtGui.QTableWidgetItem()
        headerItem.setText(QtGui.QApplication.translate("DialogPluginBrowser", "ID", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetLoadedPlugins.setHorizontalHeaderItem(0,headerItem)

        headerItem1 = QtGui.QTableWidgetItem()
        headerItem1.setText(QtGui.QApplication.translate("DialogPluginBrowser", "Name", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetLoadedPlugins.setHorizontalHeaderItem(1,headerItem1)

        headerItem2 = QtGui.QTableWidgetItem()
        headerItem2.setText(QtGui.QApplication.translate("DialogPluginBrowser", "Description", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetLoadedPlugins.setHorizontalHeaderItem(2,headerItem2)
        self.labelNotLoadedPlugins.setText(QtGui.QApplication.translate("DialogPluginBrowser", "Plugins Not Loaded:", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetNotLoadedPlugins.setColumnCount(4)
        self.tableWidgetNotLoadedPlugins.clear()
        self.tableWidgetNotLoadedPlugins.setColumnCount(4)
        self.tableWidgetNotLoadedPlugins.setRowCount(0)

        headerItem3 = QtGui.QTableWidgetItem()
        headerItem3.setText(QtGui.QApplication.translate("DialogPluginBrowser", "File Name", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetNotLoadedPlugins.setHorizontalHeaderItem(0,headerItem3)

        headerItem4 = QtGui.QTableWidgetItem()
        headerItem4.setText(QtGui.QApplication.translate("DialogPluginBrowser", "File Path", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetNotLoadedPlugins.setHorizontalHeaderItem(1,headerItem4)

        headerItem5 = QtGui.QTableWidgetItem()
        headerItem5.setText(QtGui.QApplication.translate("DialogPluginBrowser", "Class Name", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetNotLoadedPlugins.setHorizontalHeaderItem(2,headerItem5)

        headerItem6 = QtGui.QTableWidgetItem()
        headerItem6.setText(QtGui.QApplication.translate("DialogPluginBrowser", "Error Text", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetNotLoadedPlugins.setHorizontalHeaderItem(3,headerItem6)

