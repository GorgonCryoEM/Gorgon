# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_sse_helix_correspondence_viewer.ui'
#
# Created: Mon Apr 28 10:53:14 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogSSEHelixCorrespondenceViewer(object):
    def setupUi(self, DialogSSEHelixCorrespondenceViewer):
        DialogSSEHelixCorrespondenceViewer.setObjectName("DialogSSEHelixCorrespondenceViewer")
        DialogSSEHelixCorrespondenceViewer.resize(QtCore.QSize(QtCore.QRect(0,0,337,268).size()).expandedTo(DialogSSEHelixCorrespondenceViewer.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogSSEHelixCorrespondenceViewer)
        self.gridlayout.setObjectName("gridlayout")

        self.tableWidgetResults = QtGui.QTableWidget(DialogSSEHelixCorrespondenceViewer)
        self.tableWidgetResults.setEnabled(False)
        self.tableWidgetResults.setObjectName("tableWidgetResults")
        self.gridlayout.addWidget(self.tableWidgetResults,0,0,1,1)

        self.retranslateUi(DialogSSEHelixCorrespondenceViewer)
        QtCore.QMetaObject.connectSlotsByName(DialogSSEHelixCorrespondenceViewer)

    def retranslateUi(self, DialogSSEHelixCorrespondenceViewer):
        DialogSSEHelixCorrespondenceViewer.setWindowTitle(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceViewer", "Helix Correspondence Viewer", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetResults.setRowCount(0)
        self.tableWidgetResults.setColumnCount(2)
        self.tableWidgetResults.clear()
        self.tableWidgetResults.setColumnCount(2)
        self.tableWidgetResults.setRowCount(0)

        headerItem = QtGui.QTableWidgetItem()
        headerItem.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceViewer", "Result String", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetResults.setHorizontalHeaderItem(0,headerItem)

        headerItem1 = QtGui.QTableWidgetItem()
        headerItem1.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceViewer", "Cost", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetResults.setHorizontalHeaderItem(1,headerItem1)

