# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_about.ui'
#
# Created: Fri May 30 11:36:37 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogAbout(object):
    def setupUi(self, DialogAbout):
        DialogAbout.setObjectName("DialogAbout")
        DialogAbout.setWindowModality(QtCore.Qt.ApplicationModal)
        DialogAbout.resize(QtCore.QSize(QtCore.QRect(0,0,424,380).size()).expandedTo(DialogAbout.minimumSizeHint()))
        DialogAbout.setContextMenuPolicy(QtCore.Qt.DefaultContextMenu)

        self.gridlayout = QtGui.QGridLayout(DialogAbout)
        self.gridlayout.setObjectName("gridlayout")

        self.textBrowserDetails = QtGui.QTextBrowser(DialogAbout)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(1)
        sizePolicy.setVerticalStretch(1)
        sizePolicy.setHeightForWidth(self.textBrowserDetails.sizePolicy().hasHeightForWidth())
        self.textBrowserDetails.setSizePolicy(sizePolicy)
        self.textBrowserDetails.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.textBrowserDetails.setReadOnly(True)
        self.textBrowserDetails.setObjectName("textBrowserDetails")
        self.gridlayout.addWidget(self.textBrowserDetails,0,0,1,2)

        spacerItem = QtGui.QSpacerItem(40,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout.addItem(spacerItem,1,0,1,1)

        self.pushButtonOk = QtGui.QPushButton(DialogAbout)
        self.pushButtonOk.setObjectName("pushButtonOk")
        self.gridlayout.addWidget(self.pushButtonOk,1,1,1,1)

        self.retranslateUi(DialogAbout)
        QtCore.QObject.connect(self.pushButtonOk,QtCore.SIGNAL("clicked()"),DialogAbout.close)
        QtCore.QMetaObject.connectSlotsByName(DialogAbout)

    def retranslateUi(self, DialogAbout):
        DialogAbout.setWindowTitle(QtGui.QApplication.translate("DialogAbout", "About Gorgon", None, QtGui.QApplication.UnicodeUTF8))
        self.textBrowserDetails.setHtml(QtGui.QApplication.translate("DialogAbout", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:20pt;\">Gorgon</span></p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:10pt;\">Version 0.1 beta</span></p>\n"
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">A interactive tool for volumetric data visualization and processing.</p>\n"
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">Designed and Developed at Washington University in St. Louis in Collaboration with the Baylor College of Medicine by:</p>\n"
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">   Sasakthi S. Abeysinghe</p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">   Mike Marsh</p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">   Troy Ruths</p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">   Tao Ju</p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">   Matthew Baker</p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">   Wah Chiu</p>\n"
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">Copyright (c) 2007-2008, Washington University in St. Louis and Baylor College of Medicine. All rights reserved.</p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonOk.setText(QtGui.QApplication.translate("DialogAbout", "&Ok", None, QtGui.QApplication.UnicodeUTF8))

