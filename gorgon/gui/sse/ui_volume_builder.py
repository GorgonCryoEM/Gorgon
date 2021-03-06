# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_sse_builder.ui'
#
# Created by: PyQt4 UI code generator 4.11.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_DialogVolumeSSEBuilder(object):
    def setupUi(self, DialogVolumeSSEBuilder):
        DialogVolumeSSEBuilder.setObjectName(_fromUtf8("DialogVolumeSSEBuilder"))
        DialogVolumeSSEBuilder.resize(397, 759)
        self.verticalLayout_3 = QtGui.QVBoxLayout(DialogVolumeSSEBuilder)
        self.verticalLayout_3.setObjectName(_fromUtf8("verticalLayout_3"))
        self.labelAtomScore = QtGui.QLabel(DialogVolumeSSEBuilder)
        self.labelAtomScore.setObjectName(_fromUtf8("labelAtomScore"))
        self.verticalLayout_3.addWidget(self.labelAtomScore)
        self.tabWidgetScoredAtoms = QtGui.QTabWidget(DialogVolumeSSEBuilder)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidgetScoredAtoms.sizePolicy().hasHeightForWidth())
        self.tabWidgetScoredAtoms.setSizePolicy(sizePolicy)
        self.tabWidgetScoredAtoms.setMinimumSize(QtCore.QSize(330, 320))
        self.tabWidgetScoredAtoms.setObjectName(_fromUtf8("tabWidgetScoredAtoms"))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.layoutWidget = QtGui.QWidget(self.tab)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 0, 341, 373))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout_4.setObjectName(_fromUtf8("verticalLayout_4"))
        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName(_fromUtf8("hboxlayout"))
        self.verticalLayout_4.addLayout(self.hboxlayout)
        self.horizontalLayout_6 = QtGui.QHBoxLayout()
        self.horizontalLayout_6.setObjectName(_fromUtf8("horizontalLayout_6"))
        self.label_Min = QtGui.QLabel(self.layoutWidget)
        self.label_Min.setObjectName(_fromUtf8("label_Min"))
        self.horizontalLayout_6.addWidget(self.label_Min)
        self.lineEditMin = QtGui.QLineEdit(self.layoutWidget)
        self.lineEditMin.setObjectName(_fromUtf8("lineEditMin"))
        self.horizontalLayout_6.addWidget(self.lineEditMin)
        self.label_Mean = QtGui.QLabel(self.layoutWidget)
        self.label_Mean.setObjectName(_fromUtf8("label_Mean"))
        self.horizontalLayout_6.addWidget(self.label_Mean)
        self.lineEditMean = QtGui.QSpinBox(self.layoutWidget)
        self.lineEditMean.setObjectName(_fromUtf8("lineEditMean"))
        self.horizontalLayout_6.addWidget(self.lineEditMean)
        self.label_Max = QtGui.QLabel(self.layoutWidget)
        self.label_Max.setObjectName(_fromUtf8("label_Max"))
        self.horizontalLayout_6.addWidget(self.label_Max)
        self.lineEditMax = QtGui.QLineEdit(self.layoutWidget)
        self.lineEditMax.setObjectName(_fromUtf8("lineEditMax"))
        self.horizontalLayout_6.addWidget(self.lineEditMax)
        self.verticalLayout_4.addLayout(self.horizontalLayout_6)
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.labelSkeletonScore = QtGui.QLabel(self.layoutWidget)
        self.labelSkeletonScore.setEnabled(True)
        self.labelSkeletonScore.setObjectName(_fromUtf8("labelSkeletonScore"))
        self.gridLayout.addWidget(self.labelSkeletonScore, 3, 0, 1, 1)
        self.doubleSpinBoxCorrelation = QtGui.QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBoxCorrelation.setEnabled(True)
        self.doubleSpinBoxCorrelation.setMaximum(1.0)
        self.doubleSpinBoxCorrelation.setSingleStep(0.1)
        self.doubleSpinBoxCorrelation.setProperty("value", 1.0)
        self.doubleSpinBoxCorrelation.setObjectName(_fromUtf8("doubleSpinBoxCorrelation"))
        self.gridLayout.addWidget(self.doubleSpinBoxCorrelation, 4, 1, 1, 1)
        self.doubleSpinBoxSkeleton = QtGui.QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBoxSkeleton.setEnabled(True)
        self.doubleSpinBoxSkeleton.setMaximum(1.0)
        self.doubleSpinBoxSkeleton.setSingleStep(0.1)
        self.doubleSpinBoxSkeleton.setProperty("value", 1.0)
        self.doubleSpinBoxSkeleton.setObjectName(_fromUtf8("doubleSpinBoxSkeleton"))
        self.gridLayout.addWidget(self.doubleSpinBoxSkeleton, 3, 1, 1, 1)
        self.labelGeometricScore = QtGui.QLabel(self.layoutWidget)
        self.labelGeometricScore.setEnabled(True)
        self.labelGeometricScore.setObjectName(_fromUtf8("labelGeometricScore"))
        self.gridLayout.addWidget(self.labelGeometricScore, 5, 0, 1, 1)
        self.labelThreshold = QtGui.QLabel(self.layoutWidget)
        self.labelThreshold.setEnabled(True)
        self.labelThreshold.setObjectName(_fromUtf8("labelThreshold"))
        self.gridLayout.addWidget(self.labelThreshold, 0, 0, 1, 1)
        self.doubleSpinBoxResolution = QtGui.QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBoxResolution.setEnabled(True)
        self.doubleSpinBoxResolution.setMaximum(1000.0)
        self.doubleSpinBoxResolution.setSingleStep(0.1)
        self.doubleSpinBoxResolution.setProperty("value", 8.0)
        self.doubleSpinBoxResolution.setObjectName(_fromUtf8("doubleSpinBoxResolution"))
        self.gridLayout.addWidget(self.doubleSpinBoxResolution, 2, 1, 1, 1)
        self.labelCrossCorrelationScore = QtGui.QLabel(self.layoutWidget)
        self.labelCrossCorrelationScore.setEnabled(True)
        self.labelCrossCorrelationScore.setObjectName(_fromUtf8("labelCrossCorrelationScore"))
        self.gridLayout.addWidget(self.labelCrossCorrelationScore, 4, 0, 1, 1)
        self.doubleSpinBoxGeometry = QtGui.QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBoxGeometry.setEnabled(True)
        self.doubleSpinBoxGeometry.setMaximum(1.0)
        self.doubleSpinBoxGeometry.setSingleStep(0.1)
        self.doubleSpinBoxGeometry.setProperty("value", 1.0)
        self.doubleSpinBoxGeometry.setObjectName(_fromUtf8("doubleSpinBoxGeometry"))
        self.gridLayout.addWidget(self.doubleSpinBoxGeometry, 5, 1, 1, 1)
        self.labelVolumeResolution = QtGui.QLabel(self.layoutWidget)
        self.labelVolumeResolution.setEnabled(True)
        self.labelVolumeResolution.setObjectName(_fromUtf8("labelVolumeResolution"))
        self.gridLayout.addWidget(self.labelVolumeResolution, 2, 0, 1, 1)
        self.doubleSpinBoxThreshold = QtGui.QDoubleSpinBox(self.layoutWidget)
        self.doubleSpinBoxThreshold.setEnabled(True)
        self.doubleSpinBoxThreshold.setMinimum(-100.0)
        self.doubleSpinBoxThreshold.setMaximum(100.0)
        self.doubleSpinBoxThreshold.setSingleStep(0.1)
        self.doubleSpinBoxThreshold.setProperty("value", 0.38)
        self.doubleSpinBoxThreshold.setObjectName(_fromUtf8("doubleSpinBoxThreshold"))
        self.gridLayout.addWidget(self.doubleSpinBoxThreshold, 0, 1, 1, 1)
        self.verticalLayout_4.addLayout(self.gridLayout)
        self.pushButtonSSEHunter = QtGui.QPushButton(self.layoutWidget)
        self.pushButtonSSEHunter.setEnabled(True)
        self.pushButtonSSEHunter.setObjectName(_fromUtf8("pushButtonSSEHunter"))
        self.verticalLayout_4.addWidget(self.pushButtonSSEHunter)
        self.horizontalLayout_5 = QtGui.QHBoxLayout()
        self.horizontalLayout_5.setObjectName(_fromUtf8("horizontalLayout_5"))
        spacerItem = QtGui.QSpacerItem(134, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem)
        self.pushButtonSaveHelices = QtGui.QPushButton(self.layoutWidget)
        self.pushButtonSaveHelices.setEnabled(True)
        self.pushButtonSaveHelices.setObjectName(_fromUtf8("pushButtonSaveHelices"))
        self.horizontalLayout_5.addWidget(self.pushButtonSaveHelices)
        self.pushButtonSavePseudoatoms = QtGui.QPushButton(self.layoutWidget)
        self.pushButtonSavePseudoatoms.setEnabled(False)
        self.pushButtonSavePseudoatoms.setObjectName(_fromUtf8("pushButtonSavePseudoatoms"))
        self.horizontalLayout_5.addWidget(self.pushButtonSavePseudoatoms)
        self.verticalLayout_4.addLayout(self.horizontalLayout_5)
        self.tabWidgetScoredAtoms.addTab(self.tab, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.gridlayout = QtGui.QGridLayout(self.tab_2)
        self.gridlayout.setObjectName(_fromUtf8("gridlayout"))
        self.labelAtomScore_2 = QtGui.QLabel(self.tab_2)
        self.labelAtomScore_2.setObjectName(_fromUtf8("labelAtomScore_2"))
        self.gridlayout.addWidget(self.labelAtomScore_2, 0, 0, 1, 1)
        self.gridlayout1 = QtGui.QGridLayout()
        self.gridlayout1.setObjectName(_fromUtf8("gridlayout1"))
        self.lineEditAtomScore = QtGui.QLineEdit(self.tab_2)
        self.lineEditAtomScore.setReadOnly(True)
        self.lineEditAtomScore.setObjectName(_fromUtf8("lineEditAtomScore"))
        self.gridlayout1.addWidget(self.lineEditAtomScore, 0, 0, 1, 1)
        self.pushButtonBrowseAtomScore = QtGui.QPushButton(self.tab_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButtonBrowseAtomScore.sizePolicy().hasHeightForWidth())
        self.pushButtonBrowseAtomScore.setSizePolicy(sizePolicy)
        self.pushButtonBrowseAtomScore.setObjectName(_fromUtf8("pushButtonBrowseAtomScore"))
        self.gridlayout1.addWidget(self.pushButtonBrowseAtomScore, 0, 1, 1, 1)
        self.gridlayout.addLayout(self.gridlayout1, 1, 0, 1, 1)
        spacerItem1 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.gridlayout.addItem(spacerItem1, 2, 0, 1, 1)
        self.tabWidgetScoredAtoms.addTab(self.tab_2, _fromUtf8(""))
        self.verticalLayout_3.addWidget(self.tabWidgetScoredAtoms)
        self.line = QtGui.QFrame(DialogVolumeSSEBuilder)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName(_fromUtf8("line"))
        self.verticalLayout_3.addWidget(self.line)
        self.tabWidgetAddSSEs = QtGui.QTabWidget(DialogVolumeSSEBuilder)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.MinimumExpanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidgetAddSSEs.sizePolicy().hasHeightForWidth())
        self.tabWidgetAddSSEs.setSizePolicy(sizePolicy)
        self.tabWidgetAddSSEs.setMinimumSize(QtCore.QSize(330, 290))
        self.tabWidgetAddSSEs.setObjectName(_fromUtf8("tabWidgetAddSSEs"))
        self.tab_3 = QtGui.QWidget()
        self.tab_3.setObjectName(_fromUtf8("tab_3"))
        self.layoutWidget1 = QtGui.QWidget(self.tab_3)
        self.layoutWidget1.setGeometry(QtCore.QRect(10, 0, 347, 261))
        self.layoutWidget1.setObjectName(_fromUtf8("layoutWidget1"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget1)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.labelSelection = QtGui.QLabel(self.layoutWidget1)
        self.labelSelection.setObjectName(_fromUtf8("labelSelection"))
        self.verticalLayout.addWidget(self.labelSelection)
        self.tableWidgetSelection = QtGui.QTableWidget(self.layoutWidget1)
        self.tableWidgetSelection.setEnabled(True)
        self.tableWidgetSelection.setAlternatingRowColors(True)
        self.tableWidgetSelection.setObjectName(_fromUtf8("tableWidgetSelection"))
        self.tableWidgetSelection.setColumnCount(2)
        self.tableWidgetSelection.setRowCount(0)
        item = QtGui.QTableWidgetItem()
        self.tableWidgetSelection.setHorizontalHeaderItem(0, item)
        item = QtGui.QTableWidgetItem()
        self.tableWidgetSelection.setHorizontalHeaderItem(1, item)
        self.verticalLayout.addWidget(self.tableWidgetSelection)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.pushButtonRemoveSSE = QtGui.QPushButton(self.layoutWidget1)
        self.pushButtonRemoveSSE.setObjectName(_fromUtf8("pushButtonRemoveSSE"))
        self.horizontalLayout.addWidget(self.pushButtonRemoveSSE)
        spacerItem2 = QtGui.QSpacerItem(306, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem2)
        self.pushButtonSelectionToSheet = QtGui.QPushButton(self.layoutWidget1)
        self.pushButtonSelectionToSheet.setObjectName(_fromUtf8("pushButtonSelectionToSheet"))
        self.horizontalLayout.addWidget(self.pushButtonSelectionToSheet)
        self.pushButtonSelectionToHelix = QtGui.QPushButton(self.layoutWidget1)
        self.pushButtonSelectionToHelix.setObjectName(_fromUtf8("pushButtonSelectionToHelix"))
        self.horizontalLayout.addWidget(self.pushButtonSelectionToHelix)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.tabWidgetAddSSEs.addTab(self.tab_3, _fromUtf8(""))
        self.tab_4 = QtGui.QWidget()
        self.tab_4.setObjectName(_fromUtf8("tab_4"))
        self.layoutWidget2 = QtGui.QWidget(self.tab_4)
        self.layoutWidget2.setGeometry(QtCore.QRect(0, 0, 281, 110))
        self.layoutWidget2.setObjectName(_fromUtf8("layoutWidget2"))
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.layoutWidget2)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.label = QtGui.QLabel(self.layoutWidget2)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout_2.addWidget(self.label)
        self.horizontalSliderLinearityThresh = QtGui.QSlider(self.layoutWidget2)
        self.horizontalSliderLinearityThresh.setMinimum(0)
        self.horizontalSliderLinearityThresh.setMaximum(24)
        self.horizontalSliderLinearityThresh.setProperty("value", 12)
        self.horizontalSliderLinearityThresh.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSliderLinearityThresh.setTickPosition(QtGui.QSlider.NoTicks)
        self.horizontalSliderLinearityThresh.setTickInterval(0)
        self.horizontalSliderLinearityThresh.setObjectName(_fromUtf8("horizontalSliderLinearityThresh"))
        self.horizontalLayout_2.addWidget(self.horizontalSliderLinearityThresh)
        self.verticalLayout_2.addLayout(self.horizontalLayout_2)
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.label_2 = QtGui.QLabel(self.layoutWidget2)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.horizontalLayout_3.addWidget(self.label_2)
        self.doubleSpinBoxScoreThresh = QtGui.QDoubleSpinBox(self.layoutWidget2)
        self.doubleSpinBoxScoreThresh.setMinimum(-3.0)
        self.doubleSpinBoxScoreThresh.setMaximum(3.0)
        self.doubleSpinBoxScoreThresh.setSingleStep(0.1)
        self.doubleSpinBoxScoreThresh.setObjectName(_fromUtf8("doubleSpinBoxScoreThresh"))
        self.horizontalLayout_3.addWidget(self.doubleSpinBoxScoreThresh)
        self.verticalLayout_2.addLayout(self.horizontalLayout_3)
        self.horizontalLayout_4 = QtGui.QHBoxLayout()
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.pushButtonRemoveHelices = QtGui.QPushButton(self.layoutWidget2)
        self.pushButtonRemoveHelices.setObjectName(_fromUtf8("pushButtonRemoveHelices"))
        self.horizontalLayout_4.addWidget(self.pushButtonRemoveHelices)
        self.pushButtonAddHelices = QtGui.QPushButton(self.layoutWidget2)
        self.pushButtonAddHelices.setObjectName(_fromUtf8("pushButtonAddHelices"))
        self.horizontalLayout_4.addWidget(self.pushButtonAddHelices)
        self.verticalLayout_2.addLayout(self.horizontalLayout_4)
        self.tabWidgetAddSSEs.addTab(self.tab_4, _fromUtf8(""))
        self.verticalLayout_3.addWidget(self.tabWidgetAddSSEs)

        self.retranslateUi(DialogVolumeSSEBuilder)
        self.tabWidgetScoredAtoms.setCurrentIndex(0)
        self.tabWidgetAddSSEs.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeSSEBuilder)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxThreshold, self.doubleSpinBoxResolution)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxResolution, self.doubleSpinBoxSkeleton)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxSkeleton, self.doubleSpinBoxCorrelation)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxCorrelation, self.pushButtonSSEHunter)
        DialogVolumeSSEBuilder.setTabOrder(self.pushButtonSSEHunter, self.pushButtonSavePseudoatoms)
        DialogVolumeSSEBuilder.setTabOrder(self.pushButtonSavePseudoatoms, self.lineEditAtomScore)
        DialogVolumeSSEBuilder.setTabOrder(self.lineEditAtomScore, self.pushButtonBrowseAtomScore)

    def retranslateUi(self, DialogVolumeSSEBuilder):
        DialogVolumeSSEBuilder.setWindowTitle(_translate("DialogVolumeSSEBuilder", "SSE Builder", None))
        self.labelAtomScore.setText(_translate("DialogVolumeSSEBuilder", "Scored Pseudoatoms:", None))
        self.label_Min.setText(_translate("DialogVolumeSSEBuilder", "Min:", None))
        self.label_Mean.setText(_translate("DialogVolumeSSEBuilder", "Mean:", None))
        self.label_Max.setText(_translate("DialogVolumeSSEBuilder", "Max:", None))
        self.labelSkeletonScore.setText(_translate("DialogVolumeSSEBuilder", "Skeleton Influence:", None))
        self.labelGeometricScore.setText(_translate("DialogVolumeSSEBuilder", "Geometric Influence:", None))
        self.labelThreshold.setText(_translate("DialogVolumeSSEBuilder", "Threshold:", None))
        self.labelCrossCorrelationScore.setText(_translate("DialogVolumeSSEBuilder", "Helix Correlation Influence:", None))
        self.labelVolumeResolution.setText(_translate("DialogVolumeSSEBuilder", "Volume Resolution (A):", None))
        self.pushButtonSSEHunter.setText(_translate("DialogVolumeSSEBuilder", "Find Scored Pseudoatoms (SSEHunter)", None))
        self.pushButtonSaveHelices.setText(_translate("DialogVolumeSSEBuilder", "Save Helices", None))
        self.pushButtonSavePseudoatoms.setText(_translate("DialogVolumeSSEBuilder", "Save Pseudoatoms", None))
        self.tabWidgetScoredAtoms.setTabText(self.tabWidgetScoredAtoms.indexOf(self.tab), _translate("DialogVolumeSSEBuilder", "Calculate using SSEHunter", None))
        self.labelAtomScore_2.setText(_translate("DialogVolumeSSEBuilder", "Scored Pseudoatom File:", None))
        self.pushButtonBrowseAtomScore.setText(_translate("DialogVolumeSSEBuilder", "...", None))
        self.tabWidgetScoredAtoms.setTabText(self.tabWidgetScoredAtoms.indexOf(self.tab_2), _translate("DialogVolumeSSEBuilder", "Load from file", None))
        self.labelSelection.setText(_translate("DialogVolumeSSEBuilder", "Atom Selection:", None))
        item = self.tableWidgetSelection.horizontalHeaderItem(0)
        item.setText(_translate("DialogVolumeSSEBuilder", "ID", None))
        item = self.tableWidgetSelection.horizontalHeaderItem(1)
        item.setText(_translate("DialogVolumeSSEBuilder", "SSE Hunter Score", None))
        self.pushButtonRemoveSSE.setText(_translate("DialogVolumeSSEBuilder", "Remove SSE", None))
        self.pushButtonSelectionToSheet.setText(_translate("DialogVolumeSSEBuilder", "Add Sheet", None))
        self.pushButtonSelectionToHelix.setText(_translate("DialogVolumeSSEBuilder", "Add Helix", None))
        self.tabWidgetAddSSEs.setTabText(self.tabWidgetAddSSEs.indexOf(self.tab_3), _translate("DialogVolumeSSEBuilder", "Manual", None))
        self.label.setText(_translate("DialogVolumeSSEBuilder", "Linearity Threshold:", None))
        self.label_2.setText(_translate("DialogVolumeSSEBuilder", "Score Threshold:", None))
        self.pushButtonRemoveHelices.setText(_translate("DialogVolumeSSEBuilder", "Remove All", None))
        self.pushButtonAddHelices.setText(_translate("DialogVolumeSSEBuilder", "Add Helices", None))
        self.tabWidgetAddSSEs.setTabText(self.tabWidgetAddSSEs.indexOf(self.tab_4), _translate("DialogVolumeSSEBuilder", "Auto Helix", None))

