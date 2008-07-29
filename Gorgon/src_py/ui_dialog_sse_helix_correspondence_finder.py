# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_sse_helix_correspondence_finder.ui'
#
# Created: Tue Jul 29 11:47:08 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogSSEHelixCorrespondenceFinder(object):
    def setupUi(self, DialogSSEHelixCorrespondenceFinder):
        DialogSSEHelixCorrespondenceFinder.setObjectName("DialogSSEHelixCorrespondenceFinder")
        DialogSSEHelixCorrespondenceFinder.resize(QtCore.QSize(QtCore.QRect(0,0,534,396).size()).expandedTo(DialogSSEHelixCorrespondenceFinder.minimumSizeHint()))

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(DialogSSEHelixCorrespondenceFinder.sizePolicy().hasHeightForWidth())
        DialogSSEHelixCorrespondenceFinder.setSizePolicy(sizePolicy)

        self.gridlayout = QtGui.QGridLayout(DialogSSEHelixCorrespondenceFinder)
        self.gridlayout.setObjectName("gridlayout")

        self.tabWidget = QtGui.QTabWidget(DialogSSEHelixCorrespondenceFinder)
        self.tabWidget.setObjectName("tabWidget")

        self.tabDataSources = QtGui.QWidget()
        self.tabDataSources.setObjectName("tabDataSources")

        self.gridlayout1 = QtGui.QGridLayout(self.tabDataSources)
        self.gridlayout1.setObjectName("gridlayout1")

        self.gridlayout2 = QtGui.QGridLayout()
        self.gridlayout2.setObjectName("gridlayout2")

        self.label = QtGui.QLabel(self.tabDataSources)
        self.label.setObjectName("label")
        self.gridlayout2.addWidget(self.label,0,0,1,1)

        self.lineEditHelixLengthFile = QtGui.QLineEdit(self.tabDataSources)
        self.lineEditHelixLengthFile.setEnabled(False)
        self.lineEditHelixLengthFile.setObjectName("lineEditHelixLengthFile")
        self.gridlayout2.addWidget(self.lineEditHelixLengthFile,0,1,1,1)

        self.pushButtonGetHelixLengthFile = QtGui.QPushButton(self.tabDataSources)
        self.pushButtonGetHelixLengthFile.setObjectName("pushButtonGetHelixLengthFile")
        self.gridlayout2.addWidget(self.pushButtonGetHelixLengthFile,0,2,1,1)

        self.label_2 = QtGui.QLabel(self.tabDataSources)
        self.label_2.setObjectName("label_2")
        self.gridlayout2.addWidget(self.label_2,1,0,1,1)

        self.lineEditHelixLocationFile = QtGui.QLineEdit(self.tabDataSources)
        self.lineEditHelixLocationFile.setEnabled(False)
        self.lineEditHelixLocationFile.setObjectName("lineEditHelixLocationFile")
        self.gridlayout2.addWidget(self.lineEditHelixLocationFile,1,1,1,1)

        self.pushButtonGetHelixLocationFile = QtGui.QPushButton(self.tabDataSources)
        self.pushButtonGetHelixLocationFile.setObjectName("pushButtonGetHelixLocationFile")
        self.gridlayout2.addWidget(self.pushButtonGetHelixLocationFile,1,2,1,1)

        self.label_3 = QtGui.QLabel(self.tabDataSources)
        self.label_3.setObjectName("label_3")
        self.gridlayout2.addWidget(self.label_3,2,0,1,1)

        self.lineEditSkeletonFile = QtGui.QLineEdit(self.tabDataSources)
        self.lineEditSkeletonFile.setEnabled(False)
        self.lineEditSkeletonFile.setObjectName("lineEditSkeletonFile")
        self.gridlayout2.addWidget(self.lineEditSkeletonFile,2,1,1,1)

        self.pushButtonGetSkeletonFile = QtGui.QPushButton(self.tabDataSources)
        self.pushButtonGetSkeletonFile.setObjectName("pushButtonGetSkeletonFile")
        self.gridlayout2.addWidget(self.pushButtonGetSkeletonFile,2,2,1,1)

        self.label_4 = QtGui.QLabel(self.tabDataSources)
        self.label_4.setObjectName("label_4")
        self.gridlayout2.addWidget(self.label_4,3,0,1,1)

        self.lineEditSequenceFile = QtGui.QLineEdit(self.tabDataSources)
        self.lineEditSequenceFile.setEnabled(False)
        self.lineEditSequenceFile.setObjectName("lineEditSequenceFile")
        self.gridlayout2.addWidget(self.lineEditSequenceFile,3,1,1,1)

        self.pushButtonGetSequenceFile = QtGui.QPushButton(self.tabDataSources)
        self.pushButtonGetSequenceFile.setObjectName("pushButtonGetSequenceFile")
        self.gridlayout2.addWidget(self.pushButtonGetSequenceFile,3,2,1,1)
        self.gridlayout1.addLayout(self.gridlayout2,0,0,1,1)

        spacerItem = QtGui.QSpacerItem(20,40,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout1.addItem(spacerItem,1,0,1,1)
        self.tabWidget.addTab(self.tabDataSources,"")

        self.tabStandardSettings = QtGui.QWidget()
        self.tabStandardSettings.setObjectName("tabStandardSettings")

        self.gridlayout3 = QtGui.QGridLayout(self.tabStandardSettings)
        self.gridlayout3.setObjectName("gridlayout3")

        self.groupBoxCostFunction = QtGui.QGroupBox(self.tabStandardSettings)
        self.groupBoxCostFunction.setObjectName("groupBoxCostFunction")

        self.gridlayout4 = QtGui.QGridLayout(self.groupBoxCostFunction)
        self.gridlayout4.setObjectName("gridlayout4")

        self.radioButtonAbsoluteDifference = QtGui.QRadioButton(self.groupBoxCostFunction)
        self.radioButtonAbsoluteDifference.setChecked(True)
        self.radioButtonAbsoluteDifference.setObjectName("radioButtonAbsoluteDifference")
        self.gridlayout4.addWidget(self.radioButtonAbsoluteDifference,0,0,1,1)

        self.radioButtonNormalizedDifference = QtGui.QRadioButton(self.groupBoxCostFunction)
        self.radioButtonNormalizedDifference.setObjectName("radioButtonNormalizedDifference")
        self.gridlayout4.addWidget(self.radioButtonNormalizedDifference,1,0,1,1)

        self.radioButtonQuadraticError = QtGui.QRadioButton(self.groupBoxCostFunction)
        self.radioButtonQuadraticError.setObjectName("radioButtonQuadraticError")
        self.gridlayout4.addWidget(self.radioButtonQuadraticError,2,0,1,1)
        self.gridlayout3.addWidget(self.groupBoxCostFunction,0,0,1,2)

        self.label_5 = QtGui.QLabel(self.tabStandardSettings)
        self.label_5.setObjectName("label_5")
        self.gridlayout3.addWidget(self.label_5,1,0,1,1)

        self.doubleSpinBoxVoxelSize = QtGui.QDoubleSpinBox(self.tabStandardSettings)
        self.doubleSpinBoxVoxelSize.setDecimals(4)
        self.doubleSpinBoxVoxelSize.setMaximum(1000000000.0)
        self.doubleSpinBoxVoxelSize.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxVoxelSize.setObjectName("doubleSpinBoxVoxelSize")
        self.gridlayout3.addWidget(self.doubleSpinBoxVoxelSize,1,1,1,1)

        self.label_6 = QtGui.QLabel(self.tabStandardSettings)
        self.label_6.setObjectName("label_6")
        self.gridlayout3.addWidget(self.label_6,2,0,1,1)

        self.doubleSpinBoxEuclideanDistance = QtGui.QDoubleSpinBox(self.tabStandardSettings)
        self.doubleSpinBoxEuclideanDistance.setDecimals(4)
        self.doubleSpinBoxEuclideanDistance.setMaximum(1000000000.0)
        self.doubleSpinBoxEuclideanDistance.setProperty("value",QtCore.QVariant(0.0))
        self.doubleSpinBoxEuclideanDistance.setObjectName("doubleSpinBoxEuclideanDistance")
        self.gridlayout3.addWidget(self.doubleSpinBoxEuclideanDistance,2,1,1,1)

        self.checkBoxMissingHelices = QtGui.QCheckBox(self.tabStandardSettings)
        self.checkBoxMissingHelices.setObjectName("checkBoxMissingHelices")
        self.gridlayout3.addWidget(self.checkBoxMissingHelices,3,0,1,1)

        self.spinBoxMissingHelixCount = QtGui.QSpinBox(self.tabStandardSettings)
        self.spinBoxMissingHelixCount.setEnabled(False)
        self.spinBoxMissingHelixCount.setObjectName("spinBoxMissingHelixCount")
        self.gridlayout3.addWidget(self.spinBoxMissingHelixCount,3,1,1,1)

        self.checkBoxRepositionSkeleton = QtGui.QCheckBox(self.tabStandardSettings)
        self.checkBoxRepositionSkeleton.setObjectName("checkBoxRepositionSkeleton")
        self.gridlayout3.addWidget(self.checkBoxRepositionSkeleton,4,0,1,2)

        spacerItem1 = QtGui.QSpacerItem(20,20,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout3.addItem(spacerItem1,5,0,1,1)
        self.tabWidget.addTab(self.tabStandardSettings,"")

        self.tabAdvancedSettings = QtGui.QWidget()
        self.tabAdvancedSettings.setObjectName("tabAdvancedSettings")

        self.gridlayout5 = QtGui.QGridLayout(self.tabAdvancedSettings)
        self.gridlayout5.setObjectName("gridlayout5")

        self.groupBox = QtGui.QGroupBox(self.tabAdvancedSettings)
        self.groupBox.setObjectName("groupBox")

        self.gridlayout6 = QtGui.QGridLayout(self.groupBox)
        self.gridlayout6.setObjectName("gridlayout6")

        self.label_7 = QtGui.QLabel(self.groupBox)
        self.label_7.setObjectName("label_7")
        self.gridlayout6.addWidget(self.label_7,0,0,1,1)

        self.doubleSpinBoxHelixMissingPenalty = QtGui.QDoubleSpinBox(self.groupBox)
        self.doubleSpinBoxHelixMissingPenalty.setDecimals(4)
        self.doubleSpinBoxHelixMissingPenalty.setMaximum(1000000000.0)
        self.doubleSpinBoxHelixMissingPenalty.setProperty("value",QtCore.QVariant(5.0))
        self.doubleSpinBoxHelixMissingPenalty.setObjectName("doubleSpinBoxHelixMissingPenalty")
        self.gridlayout6.addWidget(self.doubleSpinBoxHelixMissingPenalty,0,1,1,1)

        self.label_8 = QtGui.QLabel(self.groupBox)
        self.label_8.setObjectName("label_8")
        self.gridlayout6.addWidget(self.label_8,1,0,1,1)

        self.doubleSpinBoxEndHelixMissingPenalty = QtGui.QDoubleSpinBox(self.groupBox)
        self.doubleSpinBoxEndHelixMissingPenalty.setDecimals(4)
        self.doubleSpinBoxEndHelixMissingPenalty.setMaximum(1000000000.0)
        self.doubleSpinBoxEndHelixMissingPenalty.setProperty("value",QtCore.QVariant(5.0))
        self.doubleSpinBoxEndHelixMissingPenalty.setObjectName("doubleSpinBoxEndHelixMissingPenalty")
        self.gridlayout6.addWidget(self.doubleSpinBoxEndHelixMissingPenalty,1,1,1,1)

        self.label_9 = QtGui.QLabel(self.groupBox)
        self.label_9.setObjectName("label_9")
        self.gridlayout6.addWidget(self.label_9,2,0,1,1)

        self.doubleSpinBoxEuclideanLoopUsedPenalty = QtGui.QDoubleSpinBox(self.groupBox)
        self.doubleSpinBoxEuclideanLoopUsedPenalty.setDecimals(4)
        self.doubleSpinBoxEuclideanLoopUsedPenalty.setMaximum(1000000000.0)
        self.doubleSpinBoxEuclideanLoopUsedPenalty.setProperty("value",QtCore.QVariant(5.0))
        self.doubleSpinBoxEuclideanLoopUsedPenalty.setObjectName("doubleSpinBoxEuclideanLoopUsedPenalty")
        self.gridlayout6.addWidget(self.doubleSpinBoxEuclideanLoopUsedPenalty,2,1,1,1)
        self.gridlayout5.addWidget(self.groupBox,0,0,1,2)

        self.groupBox_2 = QtGui.QGroupBox(self.tabAdvancedSettings)
        self.groupBox_2.setObjectName("groupBox_2")

        self.gridlayout7 = QtGui.QGridLayout(self.groupBox_2)
        self.gridlayout7.setObjectName("gridlayout7")

        self.label_10 = QtGui.QLabel(self.groupBox_2)
        self.label_10.setObjectName("label_10")
        self.gridlayout7.addWidget(self.label_10,0,0,1,1)

        self.doubleSpinBoxHelixImportance = QtGui.QDoubleSpinBox(self.groupBox_2)
        self.doubleSpinBoxHelixImportance.setDecimals(4)
        self.doubleSpinBoxHelixImportance.setMaximum(1.0)
        self.doubleSpinBoxHelixImportance.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxHelixImportance.setObjectName("doubleSpinBoxHelixImportance")
        self.gridlayout7.addWidget(self.doubleSpinBoxHelixImportance,0,1,1,1)

        self.label_11 = QtGui.QLabel(self.groupBox_2)
        self.label_11.setObjectName("label_11")
        self.gridlayout7.addWidget(self.label_11,1,0,1,1)

        self.doubleSpinBoxLoopImportance = QtGui.QDoubleSpinBox(self.groupBox_2)
        self.doubleSpinBoxLoopImportance.setDecimals(4)
        self.doubleSpinBoxLoopImportance.setMaximum(1.0)
        self.doubleSpinBoxLoopImportance.setProperty("value",QtCore.QVariant(0.2))
        self.doubleSpinBoxLoopImportance.setObjectName("doubleSpinBoxLoopImportance")
        self.gridlayout7.addWidget(self.doubleSpinBoxLoopImportance,1,1,1,1)
        self.gridlayout5.addWidget(self.groupBox_2,1,0,1,2)

        self.label_12 = QtGui.QLabel(self.tabAdvancedSettings)
        self.label_12.setObjectName("label_12")
        self.gridlayout5.addWidget(self.label_12,2,0,1,1)

        self.doubleSpinBoxAverageMissingHelixLength = QtGui.QDoubleSpinBox(self.tabAdvancedSettings)
        self.doubleSpinBoxAverageMissingHelixLength.setDecimals(4)
        self.doubleSpinBoxAverageMissingHelixLength.setMaximum(10000000000.0)
        self.doubleSpinBoxAverageMissingHelixLength.setProperty("value",QtCore.QVariant(5.0))
        self.doubleSpinBoxAverageMissingHelixLength.setObjectName("doubleSpinBoxAverageMissingHelixLength")
        self.gridlayout5.addWidget(self.doubleSpinBoxAverageMissingHelixLength,2,1,1,1)

        self.label_13 = QtGui.QLabel(self.tabAdvancedSettings)
        self.label_13.setObjectName("label_13")
        self.gridlayout5.addWidget(self.label_13,3,0,1,1)

        self.doubleSpinBoxEuclideanToPDBRatio = QtGui.QDoubleSpinBox(self.tabAdvancedSettings)
        self.doubleSpinBoxEuclideanToPDBRatio.setDecimals(4)
        self.doubleSpinBoxEuclideanToPDBRatio.setMaximum(10000000.0)
        self.doubleSpinBoxEuclideanToPDBRatio.setProperty("value",QtCore.QVariant(10.0))
        self.doubleSpinBoxEuclideanToPDBRatio.setObjectName("doubleSpinBoxEuclideanToPDBRatio")
        self.gridlayout5.addWidget(self.doubleSpinBoxEuclideanToPDBRatio,3,1,1,1)

        self.label_14 = QtGui.QLabel(self.tabAdvancedSettings)
        self.label_14.setObjectName("label_14")
        self.gridlayout5.addWidget(self.label_14,4,0,1,1)

        self.spinBoxBorderMarginThreshold = QtGui.QSpinBox(self.tabAdvancedSettings)
        self.spinBoxBorderMarginThreshold.setProperty("value",QtCore.QVariant(3))
        self.spinBoxBorderMarginThreshold.setObjectName("spinBoxBorderMarginThreshold")
        self.gridlayout5.addWidget(self.spinBoxBorderMarginThreshold,4,1,1,1)

        spacerItem2 = QtGui.QSpacerItem(20,40,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout5.addItem(spacerItem2,5,0,1,1)
        self.tabWidget.addTab(self.tabAdvancedSettings,"")

        self.tabResults = QtGui.QWidget()
        self.tabResults.setObjectName("tabResults")

        self.gridlayout8 = QtGui.QGridLayout(self.tabResults)
        self.gridlayout8.setObjectName("gridlayout8")

        self.tableWidgetResults = QtGui.QTableWidget(self.tabResults)
        self.tableWidgetResults.setEnabled(False)
        self.tableWidgetResults.setObjectName("tableWidgetResults")
        self.gridlayout8.addWidget(self.tableWidgetResults,0,0,1,1)
        self.tabWidget.addTab(self.tabResults,"")

        self.tab = QtGui.QWidget()
        self.tab.setObjectName("tab")

        self.gridlayout9 = QtGui.QGridLayout(self.tab)
        self.gridlayout9.setObjectName("gridlayout9")

        self.labelCorrespondences = QtGui.QLabel(self.tab)
        self.labelCorrespondences.setObjectName("labelCorrespondences")
        self.gridlayout9.addWidget(self.labelCorrespondences,0,0,1,1)

        self.comboBoxCorrespondences = QtGui.QComboBox(self.tab)
        self.comboBoxCorrespondences.setObjectName("comboBoxCorrespondences")
        self.gridlayout9.addWidget(self.comboBoxCorrespondences,0,1,1,2)

        self.tableWidgetCorrespondenceList = QtGui.QTableWidget(self.tab)
        self.tableWidgetCorrespondenceList.setObjectName("tableWidgetCorrespondenceList")
        self.gridlayout9.addWidget(self.tableWidgetCorrespondenceList,1,0,4,2)

        spacerItem3 = QtGui.QSpacerItem(20,16,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout9.addItem(spacerItem3,1,2,1,1)

        self.pushButtonMoveUp = QtGui.QPushButton(self.tab)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButtonMoveUp.sizePolicy().hasHeightForWidth())
        self.pushButtonMoveUp.setSizePolicy(sizePolicy)
        self.pushButtonMoveUp.setObjectName("pushButtonMoveUp")
        self.gridlayout9.addWidget(self.pushButtonMoveUp,2,2,1,1)

        self.pushButton_2 = QtGui.QPushButton(self.tab)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButton_2.sizePolicy().hasHeightForWidth())
        self.pushButton_2.setSizePolicy(sizePolicy)
        self.pushButton_2.setObjectName("pushButton_2")
        self.gridlayout9.addWidget(self.pushButton_2,3,2,1,1)

        spacerItem4 = QtGui.QSpacerItem(20,231,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout9.addItem(spacerItem4,4,2,1,1)
        self.tabWidget.addTab(self.tab,"")
        self.gridlayout.addWidget(self.tabWidget,0,0,1,1)

        self.gridlayout10 = QtGui.QGridLayout()
        self.gridlayout10.setObjectName("gridlayout10")

        self.pushButtonReset = QtGui.QPushButton(DialogSSEHelixCorrespondenceFinder)
        self.pushButtonReset.setMaximumSize(QtCore.QSize(75,16777215))
        self.pushButtonReset.setObjectName("pushButtonReset")
        self.gridlayout10.addWidget(self.pushButtonReset,0,0,1,1)

        spacerItem5 = QtGui.QSpacerItem(40,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.gridlayout10.addItem(spacerItem5,0,1,1,1)

        self.gridlayout11 = QtGui.QGridLayout()
        self.gridlayout11.setSpacing(0)
        self.gridlayout11.setObjectName("gridlayout11")

        self.pushButtonOk = QtGui.QPushButton(DialogSSEHelixCorrespondenceFinder)
        self.pushButtonOk.setEnabled(False)
        self.pushButtonOk.setMaximumSize(QtCore.QSize(75,16777215))
        self.pushButtonOk.setObjectName("pushButtonOk")
        self.gridlayout11.addWidget(self.pushButtonOk,0,0,1,1)

        self.pushButtonCancel = QtGui.QPushButton(DialogSSEHelixCorrespondenceFinder)
        self.pushButtonCancel.setMaximumSize(QtCore.QSize(75,16777215))
        self.pushButtonCancel.setObjectName("pushButtonCancel")
        self.gridlayout11.addWidget(self.pushButtonCancel,0,1,1,1)
        self.gridlayout10.addLayout(self.gridlayout11,0,2,1,1)
        self.gridlayout.addLayout(self.gridlayout10,1,0,1,1)

        self.retranslateUi(DialogSSEHelixCorrespondenceFinder)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QObject.connect(self.checkBoxMissingHelices,QtCore.SIGNAL("toggled(bool)"),self.spinBoxMissingHelixCount.setEnabled)
        QtCore.QMetaObject.connectSlotsByName(DialogSSEHelixCorrespondenceFinder)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.tabWidget,self.lineEditHelixLengthFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.lineEditHelixLengthFile,self.pushButtonGetHelixLengthFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.pushButtonGetHelixLengthFile,self.lineEditHelixLocationFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.lineEditHelixLocationFile,self.pushButtonGetHelixLocationFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.pushButtonGetHelixLocationFile,self.lineEditSkeletonFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.lineEditSkeletonFile,self.pushButtonGetSkeletonFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.pushButtonGetSkeletonFile,self.lineEditSequenceFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.lineEditSequenceFile,self.pushButtonGetSequenceFile)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.pushButtonGetSequenceFile,self.radioButtonAbsoluteDifference)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.radioButtonAbsoluteDifference,self.radioButtonNormalizedDifference)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.radioButtonNormalizedDifference,self.radioButtonQuadraticError)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.radioButtonQuadraticError,self.doubleSpinBoxVoxelSize)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxVoxelSize,self.doubleSpinBoxEuclideanDistance)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxEuclideanDistance,self.checkBoxMissingHelices)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.checkBoxMissingHelices,self.spinBoxMissingHelixCount)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.spinBoxMissingHelixCount,self.checkBoxRepositionSkeleton)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.checkBoxRepositionSkeleton,self.doubleSpinBoxHelixMissingPenalty)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxHelixMissingPenalty,self.doubleSpinBoxEndHelixMissingPenalty)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxEndHelixMissingPenalty,self.doubleSpinBoxEuclideanLoopUsedPenalty)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxEuclideanLoopUsedPenalty,self.doubleSpinBoxHelixImportance)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxHelixImportance,self.doubleSpinBoxLoopImportance)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxLoopImportance,self.doubleSpinBoxAverageMissingHelixLength)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxAverageMissingHelixLength,self.doubleSpinBoxEuclideanToPDBRatio)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.doubleSpinBoxEuclideanToPDBRatio,self.spinBoxBorderMarginThreshold)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.spinBoxBorderMarginThreshold,self.pushButtonReset)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.pushButtonReset,self.pushButtonOk)
        DialogSSEHelixCorrespondenceFinder.setTabOrder(self.pushButtonOk,self.pushButtonCancel)

    def retranslateUi(self, DialogSSEHelixCorrespondenceFinder):
        DialogSSEHelixCorrespondenceFinder.setWindowTitle(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Helix Correspondence Finder", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Helix Lengths:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonGetHelixLengthFile.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "...", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "3D Helix Locations:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonGetHelixLocationFile.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "...", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Cryo-EM Skeleton:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonGetSkeletonFile.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "...", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Sequence:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonGetSequenceFile.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "...", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabDataSources), QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Data Sources", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBoxCostFunction.setTitle(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Cost Function:", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonAbsoluteDifference.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Absolute Difference", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonNormalizedDifference.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Normalized Difference", None, QtGui.QApplication.UnicodeUTF8))
        self.radioButtonQuadraticError.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Quadratic Error", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setToolTip(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "The ratio between the size of the skeleton and the size of the 3D helix locations", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setStatusTip(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "The ratio between the size of the skeleton and the size of the 3D helix locations", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setWhatsThis(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "The ratio between the size of the skeleton and the size of the 3D helix locations", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Skeleton / Helix Size Ratio:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setToolTip(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "The maximum euclidean distance between possibly connected helices even if they are not connected in the skeleton", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setStatusTip(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "The maximum euclidean distance between possibly connected helices even if they are not connected in the skeleton", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setWhatsThis(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "The maximum euclidean distance between possibly connected helices even if they are not connected in the skeleton", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Max Euclidean Loop Distance:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxMissingHelices.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "No of Missing Helices:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxRepositionSkeleton.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "3D Helixes in Wrong Coordinate Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabStandardSettings), QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Standard Settings", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Penalties", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "\"Helix is missing\" Penalty:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "\"First or last helix is missing\" Penalty:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "\"Euclidean loop used\" Penalty:", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_2.setTitle(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Coefficients:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Helix Importance:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Loop Importance:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Average Missing Helix Length:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Euclidean to PDB Distance Ratio:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_14.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Border Margin Threshold:", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabAdvancedSettings), QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Advanced Settings", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetResults.setRowCount(0)
        self.tableWidgetResults.setColumnCount(2)
        self.tableWidgetResults.clear()
        self.tableWidgetResults.setColumnCount(2)
        self.tableWidgetResults.setRowCount(0)

        headerItem = QtGui.QTableWidgetItem()
        headerItem.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Result String", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetResults.setHorizontalHeaderItem(0,headerItem)

        headerItem1 = QtGui.QTableWidgetItem()
        headerItem1.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Cost", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetResults.setHorizontalHeaderItem(1,headerItem1)
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabResults), QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Debug", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCorrespondences.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Correspondence:", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetCorrespondenceList.clear()
        self.tableWidgetCorrespondenceList.setColumnCount(3)
        self.tableWidgetCorrespondenceList.setRowCount(0)

        headerItem2 = QtGui.QTableWidgetItem()
        headerItem2.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Predicted Helix", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetCorrespondenceList.setHorizontalHeaderItem(0,headerItem2)

        headerItem3 = QtGui.QTableWidgetItem()
        headerItem3.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Observed Helix", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetCorrespondenceList.setHorizontalHeaderItem(1,headerItem3)

        headerItem4 = QtGui.QTableWidgetItem()
        headerItem4.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Constrained", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetCorrespondenceList.setHorizontalHeaderItem(2,headerItem4)
        self.pushButtonMoveUp.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "▲", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_2.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "▼", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Results", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonReset.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Reset", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonOk.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "OK", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonCancel.setText(QtGui.QApplication.translate("DialogSSEHelixCorrespondenceFinder", "Cancel", None, QtGui.QApplication.UnicodeUTF8))

