# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_calpha_atom_placer.ui'
#
# Created: Thu Apr 17 20:04:44 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogCAlphaAtomPlacer(object):
    def setupUi(self, DialogCAlphaAtomPlacer):
        DialogCAlphaAtomPlacer.setObjectName("DialogCAlphaAtomPlacer")
        DialogCAlphaAtomPlacer.resize(QtCore.QSize(QtCore.QRect(0,0,450,224).size()).expandedTo(DialogCAlphaAtomPlacer.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogCAlphaAtomPlacer)
        self.gridlayout.setObjectName("gridlayout")

        self.labelSerial = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelSerial.setObjectName("labelSerial")
        self.gridlayout.addWidget(self.labelSerial,0,0,1,1)

        self.spinBoxSerial = QtGui.QSpinBox(DialogCAlphaAtomPlacer)
        self.spinBoxSerial.setMinimum(0)
        self.spinBoxSerial.setMaximum(9999)
        self.spinBoxSerial.setObjectName("spinBoxSerial")
        self.gridlayout.addWidget(self.spinBoxSerial,0,1,1,1)

        self.labelName = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName.setObjectName("labelName")
        self.gridlayout.addWidget(self.labelName,0,2,1,1)

        self.lineEditAtomName = QtGui.QLineEdit(DialogCAlphaAtomPlacer)
        self.lineEditAtomName.setObjectName("lineEditAtomName")
        self.gridlayout.addWidget(self.lineEditAtomName,0,3,1,1)

        self.labelName_2 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_2.setObjectName("labelName_2")
        self.gridlayout.addWidget(self.labelName_2,1,0,1,1)

        self.lineEditAlternateLocation = QtGui.QLineEdit(DialogCAlphaAtomPlacer)
        self.lineEditAlternateLocation.setObjectName("lineEditAlternateLocation")
        self.gridlayout.addWidget(self.lineEditAlternateLocation,1,1,1,1)

        self.labelName_3 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_3.setObjectName("labelName_3")
        self.gridlayout.addWidget(self.labelName_3,1,2,1,1)

        self.lineEditResidueName = QtGui.QLineEdit(DialogCAlphaAtomPlacer)
        self.lineEditResidueName.setObjectName("lineEditResidueName")
        self.gridlayout.addWidget(self.lineEditResidueName,1,3,1,1)

        self.labelName_5 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_5.setObjectName("labelName_5")
        self.gridlayout.addWidget(self.labelName_5,2,0,1,1)

        self.lineEditChainIdentifier = QtGui.QLineEdit(DialogCAlphaAtomPlacer)
        self.lineEditChainIdentifier.setObjectName("lineEditChainIdentifier")
        self.gridlayout.addWidget(self.lineEditChainIdentifier,2,1,1,1)

        self.labelName_6 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_6.setObjectName("labelName_6")
        self.gridlayout.addWidget(self.labelName_6,2,2,1,1)

        self.spinBoxResidueSequenceNo = QtGui.QSpinBox(DialogCAlphaAtomPlacer)
        self.spinBoxResidueSequenceNo.setMinimum(1)
        self.spinBoxResidueSequenceNo.setMaximum(9999)
        self.spinBoxResidueSequenceNo.setObjectName("spinBoxResidueSequenceNo")
        self.gridlayout.addWidget(self.spinBoxResidueSequenceNo,2,3,1,1)

        self.labelName_7 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_7.setObjectName("labelName_7")
        self.gridlayout.addWidget(self.labelName_7,3,0,1,1)

        self.lineEditInsertionCode = QtGui.QLineEdit(DialogCAlphaAtomPlacer)
        self.lineEditInsertionCode.setObjectName("lineEditInsertionCode")
        self.gridlayout.addWidget(self.lineEditInsertionCode,3,1,1,1)

        self.labelName_4 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_4.setObjectName("labelName_4")
        self.gridlayout.addWidget(self.labelName_4,4,0,1,1)

        self.doubleSpinBoxPositionX = QtGui.QDoubleSpinBox(DialogCAlphaAtomPlacer)
        self.doubleSpinBoxPositionX.setMinimumSize(QtCore.QSize(107,0))
        self.doubleSpinBoxPositionX.setDecimals(3)
        self.doubleSpinBoxPositionX.setMinimum(-99999.999)
        self.doubleSpinBoxPositionX.setMaximum(99999.999)
        self.doubleSpinBoxPositionX.setObjectName("doubleSpinBoxPositionX")
        self.gridlayout.addWidget(self.doubleSpinBoxPositionX,4,1,1,1)

        self.doubleSpinBoxPositionY = QtGui.QDoubleSpinBox(DialogCAlphaAtomPlacer)
        self.doubleSpinBoxPositionY.setMinimumSize(QtCore.QSize(107,0))
        self.doubleSpinBoxPositionY.setDecimals(4)
        self.doubleSpinBoxPositionY.setMinimum(-99999999.99)
        self.doubleSpinBoxPositionY.setMaximum(100000000.0)
        self.doubleSpinBoxPositionY.setObjectName("doubleSpinBoxPositionY")
        self.gridlayout.addWidget(self.doubleSpinBoxPositionY,4,2,1,1)

        self.doubleSpinBoxPositionZ = QtGui.QDoubleSpinBox(DialogCAlphaAtomPlacer)
        self.doubleSpinBoxPositionZ.setMinimumSize(QtCore.QSize(107,0))
        self.doubleSpinBoxPositionZ.setDecimals(4)
        self.doubleSpinBoxPositionZ.setMinimum(-99999999.99)
        self.doubleSpinBoxPositionZ.setMaximum(100000000.0)
        self.doubleSpinBoxPositionZ.setObjectName("doubleSpinBoxPositionZ")
        self.gridlayout.addWidget(self.doubleSpinBoxPositionZ,4,3,1,1)

        self.labelName_8 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_8.setObjectName("labelName_8")
        self.gridlayout.addWidget(self.labelName_8,5,0,1,1)

        self.doubleSpinBoxOccupancy = QtGui.QDoubleSpinBox(DialogCAlphaAtomPlacer)
        self.doubleSpinBoxOccupancy.setDecimals(2)
        self.doubleSpinBoxOccupancy.setMinimum(-9999.99)
        self.doubleSpinBoxOccupancy.setMaximum(9999.99)
        self.doubleSpinBoxOccupancy.setObjectName("doubleSpinBoxOccupancy")
        self.gridlayout.addWidget(self.doubleSpinBoxOccupancy,5,1,1,1)

        self.labelName_9 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_9.setObjectName("labelName_9")
        self.gridlayout.addWidget(self.labelName_9,5,2,1,1)

        self.doubleSpinBoxTemperatureFactor = QtGui.QDoubleSpinBox(DialogCAlphaAtomPlacer)
        self.doubleSpinBoxTemperatureFactor.setDecimals(2)
        self.doubleSpinBoxTemperatureFactor.setMinimum(-9999.99)
        self.doubleSpinBoxTemperatureFactor.setMaximum(9999.99)
        self.doubleSpinBoxTemperatureFactor.setObjectName("doubleSpinBoxTemperatureFactor")
        self.gridlayout.addWidget(self.doubleSpinBoxTemperatureFactor,5,3,1,1)

        self.labelName_10 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_10.setObjectName("labelName_10")
        self.gridlayout.addWidget(self.labelName_10,6,0,1,1)

        self.lineEditElement = QtGui.QLineEdit(DialogCAlphaAtomPlacer)
        self.lineEditElement.setObjectName("lineEditElement")
        self.gridlayout.addWidget(self.lineEditElement,6,1,1,1)

        self.labelName_11 = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.labelName_11.setObjectName("labelName_11")
        self.gridlayout.addWidget(self.labelName_11,6,2,1,1)

        self.lineEditCharge = QtGui.QLineEdit(DialogCAlphaAtomPlacer)
        self.lineEditCharge.setObjectName("lineEditCharge")
        self.gridlayout.addWidget(self.lineEditCharge,6,3,1,1)

        self.label = QtGui.QLabel(DialogCAlphaAtomPlacer)
        self.label.setEnabled(False)
        self.label.setObjectName("label")
        self.gridlayout.addWidget(self.label,7,0,1,3)

        self.pushButtonAddAtom = QtGui.QPushButton(DialogCAlphaAtomPlacer)
        self.pushButtonAddAtom.setObjectName("pushButtonAddAtom")
        self.gridlayout.addWidget(self.pushButtonAddAtom,7,3,1,1)

        self.retranslateUi(DialogCAlphaAtomPlacer)
        QtCore.QMetaObject.connectSlotsByName(DialogCAlphaAtomPlacer)
        DialogCAlphaAtomPlacer.setTabOrder(self.spinBoxSerial,self.lineEditAtomName)
        DialogCAlphaAtomPlacer.setTabOrder(self.lineEditAtomName,self.lineEditAlternateLocation)
        DialogCAlphaAtomPlacer.setTabOrder(self.lineEditAlternateLocation,self.lineEditResidueName)
        DialogCAlphaAtomPlacer.setTabOrder(self.lineEditResidueName,self.lineEditChainIdentifier)
        DialogCAlphaAtomPlacer.setTabOrder(self.lineEditChainIdentifier,self.spinBoxResidueSequenceNo)
        DialogCAlphaAtomPlacer.setTabOrder(self.spinBoxResidueSequenceNo,self.lineEditInsertionCode)
        DialogCAlphaAtomPlacer.setTabOrder(self.lineEditInsertionCode,self.doubleSpinBoxPositionX)
        DialogCAlphaAtomPlacer.setTabOrder(self.doubleSpinBoxPositionX,self.doubleSpinBoxPositionY)
        DialogCAlphaAtomPlacer.setTabOrder(self.doubleSpinBoxPositionY,self.doubleSpinBoxPositionZ)
        DialogCAlphaAtomPlacer.setTabOrder(self.doubleSpinBoxPositionZ,self.doubleSpinBoxOccupancy)
        DialogCAlphaAtomPlacer.setTabOrder(self.doubleSpinBoxOccupancy,self.doubleSpinBoxTemperatureFactor)
        DialogCAlphaAtomPlacer.setTabOrder(self.doubleSpinBoxTemperatureFactor,self.lineEditElement)
        DialogCAlphaAtomPlacer.setTabOrder(self.lineEditElement,self.lineEditCharge)
        DialogCAlphaAtomPlacer.setTabOrder(self.lineEditCharge,self.pushButtonAddAtom)
        
        self.lineEditAtomName.text()

    def retranslateUi(self, DialogCAlphaAtomPlacer):
        DialogCAlphaAtomPlacer.setWindowTitle(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "CAlpha Atom Placer", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSerial.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Serial:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Atom Name:", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEditAtomName.setInputMask(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "nnnn; ", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_2.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Alternate Location:", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEditAlternateLocation.setInputMask(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "n; ", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_3.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Residue Name:", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEditResidueName.setInputMask(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "nnn; ", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_5.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Chain Identifier:", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEditChainIdentifier.setInputMask(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "n; ", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_6.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Residue Sequence No:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_7.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Insertion Code:", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEditInsertionCode.setInputMask(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "n; ", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_4.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Position*:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_8.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Occupancy:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_9.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Temperature Factor:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_10.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Element:", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEditElement.setInputMask(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "nn; ", None, QtGui.QApplication.UnicodeUTF8))
        self.labelName_11.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Charge:", None, QtGui.QApplication.UnicodeUTF8))
        self.lineEditCharge.setInputMask(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "nn; ", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "* The position can be picked by clicking on a skeleton.", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonAddAtom.setText(QtGui.QApplication.translate("DialogCAlphaAtomPlacer", "Add Atom", None, QtGui.QApplication.UnicodeUTF8))

