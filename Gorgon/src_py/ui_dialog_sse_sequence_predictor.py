# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_sse_sequence_predictor.ui'
#
# Created: Thu Dec 18 14:09:21 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogSSESequencePredictor(object):
    def setupUi(self, DialogSSESequencePredictor):
        DialogSSESequencePredictor.setObjectName("DialogSSESequencePredictor")
        DialogSSESequencePredictor.resize(QtCore.QSize(QtCore.QRect(0,0,283,313).size()).expandedTo(DialogSSESequencePredictor.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogSSESequencePredictor)
        self.gridlayout.setObjectName("gridlayout")

        self.labelStartingResidue = QtGui.QLabel(DialogSSESequencePredictor)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum,QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.labelStartingResidue.sizePolicy().hasHeightForWidth())
        self.labelStartingResidue.setSizePolicy(sizePolicy)
        self.labelStartingResidue.setObjectName("labelStartingResidue")
        self.gridlayout.addWidget(self.labelStartingResidue,0,0,1,2)

        self.spinBoxStart = QtGui.QSpinBox(DialogSSESequencePredictor)
        self.spinBoxStart.setMinimum(1)
        self.spinBoxStart.setMaximum(999999999)
        self.spinBoxStart.setObjectName("spinBoxStart")
        self.gridlayout.addWidget(self.spinBoxStart,0,2,1,2)

        self.labelSequence = QtGui.QLabel(DialogSSESequencePredictor)
        self.labelSequence.setObjectName("labelSequence")
        self.gridlayout.addWidget(self.labelSequence,1,0,1,2)

        self.textEditSequence = QtGui.QTextEdit(DialogSSESequencePredictor)
        self.textEditSequence.setObjectName("textEditSequence")
        self.gridlayout.addWidget(self.textEditSequence,2,0,1,4)

        self.labelServer = QtGui.QLabel(DialogSSESequencePredictor)
        self.labelServer.setObjectName("labelServer")
        self.gridlayout.addWidget(self.labelServer,3,0,1,1)

        self.checkBoxJpred = QtGui.QCheckBox(DialogSSESequencePredictor)
        self.checkBoxJpred.setChecked(True)
        self.checkBoxJpred.setObjectName("checkBoxJpred")
        self.gridlayout.addWidget(self.checkBoxJpred,3,1,1,1)

        self.checkBoxPsipred = QtGui.QCheckBox(DialogSSESequencePredictor)
        self.checkBoxPsipred.setEnabled(True)
        self.checkBoxPsipred.setChecked(True)
        self.checkBoxPsipred.setObjectName("checkBoxPsipred")
        self.gridlayout.addWidget(self.checkBoxPsipred,3,2,1,1)

        self.checkBoxScratch = QtGui.QCheckBox(DialogSSESequencePredictor)
        self.checkBoxScratch.setEnabled(True)
        self.checkBoxScratch.setChecked(True)
        self.checkBoxScratch.setObjectName("checkBoxScratch")
        self.gridlayout.addWidget(self.checkBoxScratch,3,3,1,1)

        self.pushButtonPredict = QtGui.QPushButton(DialogSSESequencePredictor)
        self.pushButtonPredict.setEnabled(False)
        self.pushButtonPredict.setObjectName("pushButtonPredict")
        self.gridlayout.addWidget(self.pushButtonPredict,4,3,1,1)

        self.labelPrediction = QtGui.QLabel(DialogSSESequencePredictor)
        self.labelPrediction.setObjectName("labelPrediction")
        self.gridlayout.addWidget(self.labelPrediction,5,0,1,2)

        self.textEditPrediction = QtGui.QTextEdit(DialogSSESequencePredictor)
        self.textEditPrediction.setEnabled(True)
        self.textEditPrediction.setObjectName("textEditPrediction")
        self.gridlayout.addWidget(self.textEditPrediction,6,0,1,4)

        self.pushButtonSave = QtGui.QPushButton(DialogSSESequencePredictor)
        self.pushButtonSave.setEnabled(False)
        self.pushButtonSave.setObjectName("pushButtonSave")
        self.gridlayout.addWidget(self.pushButtonSave,7,2,1,1)

        self.pushButtonClose = QtGui.QPushButton(DialogSSESequencePredictor)
        self.pushButtonClose.setObjectName("pushButtonClose")
        self.gridlayout.addWidget(self.pushButtonClose,7,3,1,1)

        self.retranslateUi(DialogSSESequencePredictor)
        QtCore.QMetaObject.connectSlotsByName(DialogSSESequencePredictor)
        DialogSSESequencePredictor.setTabOrder(self.spinBoxStart,self.textEditSequence)
        DialogSSESequencePredictor.setTabOrder(self.textEditSequence,self.checkBoxJpred)
        DialogSSESequencePredictor.setTabOrder(self.checkBoxJpred,self.checkBoxPsipred)
        DialogSSESequencePredictor.setTabOrder(self.checkBoxPsipred,self.checkBoxScratch)
        DialogSSESequencePredictor.setTabOrder(self.checkBoxScratch,self.pushButtonPredict)
        DialogSSESequencePredictor.setTabOrder(self.pushButtonPredict,self.textEditPrediction)

    def retranslateUi(self, DialogSSESequencePredictor):
        DialogSSESequencePredictor.setWindowTitle(QtGui.QApplication.translate("DialogSSESequencePredictor", "Sequence Predictor", None, QtGui.QApplication.UnicodeUTF8))
        self.labelStartingResidue.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "Starting Residue No:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSequence.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "Sequence:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelServer.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "Server:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxJpred.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "Jpred", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxPsipred.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "PSIPRED", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxScratch.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "Scratch", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonPredict.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "&Predict", None, QtGui.QApplication.UnicodeUTF8))
        self.labelPrediction.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "Prediction:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonSave.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "&Save", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonClose.setText(QtGui.QApplication.translate("DialogSSESequencePredictor", "&Close", None, QtGui.QApplication.UnicodeUTF8))

