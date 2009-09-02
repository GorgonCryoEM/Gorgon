# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_volume_sse_builder.ui'
#
# Created: Wed Sep 02 12:45:47 2009
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogVolumeSSEBuilder(object):
    def setupUi(self, DialogVolumeSSEBuilder):
        DialogVolumeSSEBuilder.setObjectName("DialogVolumeSSEBuilder")
        DialogVolumeSSEBuilder.resize(QtCore.QSize(QtCore.QRect(0,0,242,564).size()).expandedTo(DialogVolumeSSEBuilder.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogVolumeSSEBuilder)
        self.gridlayout.setObjectName("gridlayout")

        self.labelAtomScore = QtGui.QLabel(DialogVolumeSSEBuilder)
        self.labelAtomScore.setObjectName("labelAtomScore")
        self.gridlayout.addWidget(self.labelAtomScore,0,0,1,1)

        self.tabWidgetScoredAtoms = QtGui.QTabWidget(DialogVolumeSSEBuilder)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tabWidgetScoredAtoms.sizePolicy().hasHeightForWidth())
        self.tabWidgetScoredAtoms.setSizePolicy(sizePolicy)
        self.tabWidgetScoredAtoms.setObjectName("tabWidgetScoredAtoms")

        self.tab = QtGui.QWidget()
        self.tab.setObjectName("tab")

        self.gridlayout1 = QtGui.QGridLayout(self.tab)
        self.gridlayout1.setObjectName("gridlayout1")

        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName("hboxlayout")

        self.pushButtonLoadVolume = QtGui.QPushButton(self.tab)
        self.pushButtonLoadVolume.setObjectName("pushButtonLoadVolume")
        self.hboxlayout.addWidget(self.pushButtonLoadVolume)

        self.pushButtonLoadSkeleton = QtGui.QPushButton(self.tab)
        self.pushButtonLoadSkeleton.setObjectName("pushButtonLoadSkeleton")
        self.hboxlayout.addWidget(self.pushButtonLoadSkeleton)
        self.gridlayout1.addLayout(self.hboxlayout,0,0,1,2)

        self.labelThreshold = QtGui.QLabel(self.tab)
        self.labelThreshold.setEnabled(False)
        self.labelThreshold.setObjectName("labelThreshold")
        self.gridlayout1.addWidget(self.labelThreshold,1,0,1,1)

        self.doubleSpinBoxThreshold = QtGui.QDoubleSpinBox(self.tab)
        self.doubleSpinBoxThreshold.setEnabled(False)
        self.doubleSpinBoxThreshold.setMinimum(-100.0)
        self.doubleSpinBoxThreshold.setMaximum(100.0)
        self.doubleSpinBoxThreshold.setProperty("value",QtCore.QVariant(0.38))
        self.doubleSpinBoxThreshold.setObjectName("doubleSpinBoxThreshold")
        self.gridlayout1.addWidget(self.doubleSpinBoxThreshold,1,1,1,1)

        self.labelVolumeResolution = QtGui.QLabel(self.tab)
        self.labelVolumeResolution.setEnabled(False)
        self.labelVolumeResolution.setObjectName("labelVolumeResolution")
        self.gridlayout1.addWidget(self.labelVolumeResolution,2,0,1,1)

        self.doubleSpinBoxResolution = QtGui.QDoubleSpinBox(self.tab)
        self.doubleSpinBoxResolution.setEnabled(False)
        self.doubleSpinBoxResolution.setMaximum(1000.0)
        self.doubleSpinBoxResolution.setProperty("value",QtCore.QVariant(8.0))
        self.doubleSpinBoxResolution.setObjectName("doubleSpinBoxResolution")
        self.gridlayout1.addWidget(self.doubleSpinBoxResolution,2,1,1,1)

        self.labelSkeletonScore = QtGui.QLabel(self.tab)
        self.labelSkeletonScore.setEnabled(False)
        self.labelSkeletonScore.setObjectName("labelSkeletonScore")
        self.gridlayout1.addWidget(self.labelSkeletonScore,3,0,1,1)

        self.doubleSpinBoxSkeleton = QtGui.QDoubleSpinBox(self.tab)
        self.doubleSpinBoxSkeleton.setEnabled(False)
        self.doubleSpinBoxSkeleton.setMaximum(1.0)
        self.doubleSpinBoxSkeleton.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxSkeleton.setObjectName("doubleSpinBoxSkeleton")
        self.gridlayout1.addWidget(self.doubleSpinBoxSkeleton,3,1,1,1)

        self.labelCrossCorrelationScore = QtGui.QLabel(self.tab)
        self.labelCrossCorrelationScore.setEnabled(False)
        self.labelCrossCorrelationScore.setObjectName("labelCrossCorrelationScore")
        self.gridlayout1.addWidget(self.labelCrossCorrelationScore,4,0,1,1)

        self.doubleSpinBoxCorrelation = QtGui.QDoubleSpinBox(self.tab)
        self.doubleSpinBoxCorrelation.setEnabled(False)
        self.doubleSpinBoxCorrelation.setMaximum(1.0)
        self.doubleSpinBoxCorrelation.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxCorrelation.setObjectName("doubleSpinBoxCorrelation")
        self.gridlayout1.addWidget(self.doubleSpinBoxCorrelation,4,1,1,1)

        self.labelGeometricScore = QtGui.QLabel(self.tab)
        self.labelGeometricScore.setEnabled(False)
        self.labelGeometricScore.setObjectName("labelGeometricScore")
        self.gridlayout1.addWidget(self.labelGeometricScore,5,0,1,1)

        self.doubleSpinBoxGeometry = QtGui.QDoubleSpinBox(self.tab)
        self.doubleSpinBoxGeometry.setEnabled(False)
        self.doubleSpinBoxGeometry.setMaximum(1.0)
        self.doubleSpinBoxGeometry.setProperty("value",QtCore.QVariant(1.0))
        self.doubleSpinBoxGeometry.setObjectName("doubleSpinBoxGeometry")
        self.gridlayout1.addWidget(self.doubleSpinBoxGeometry,5,1,1,1)

        self.pushButtonSSEHunter = QtGui.QPushButton(self.tab)
        self.pushButtonSSEHunter.setEnabled(False)
        self.pushButtonSSEHunter.setObjectName("pushButtonSSEHunter")
        self.gridlayout1.addWidget(self.pushButtonSSEHunter,6,0,1,2)
        self.tabWidgetScoredAtoms.addTab(self.tab,"")

        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName("tab_2")

        self.gridlayout2 = QtGui.QGridLayout(self.tab_2)
        self.gridlayout2.setObjectName("gridlayout2")

        self.labelAtomScore_2 = QtGui.QLabel(self.tab_2)
        self.labelAtomScore_2.setObjectName("labelAtomScore_2")
        self.gridlayout2.addWidget(self.labelAtomScore_2,0,0,1,1)

        self.gridlayout3 = QtGui.QGridLayout()
        self.gridlayout3.setObjectName("gridlayout3")

        self.lineEditAtomScore = QtGui.QLineEdit(self.tab_2)
        self.lineEditAtomScore.setReadOnly(True)
        self.lineEditAtomScore.setObjectName("lineEditAtomScore")
        self.gridlayout3.addWidget(self.lineEditAtomScore,0,0,1,1)

        self.pushButtonBrowseAtomScore = QtGui.QPushButton(self.tab_2)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.pushButtonBrowseAtomScore.sizePolicy().hasHeightForWidth())
        self.pushButtonBrowseAtomScore.setSizePolicy(sizePolicy)
        self.pushButtonBrowseAtomScore.setObjectName("pushButtonBrowseAtomScore")
        self.gridlayout3.addWidget(self.pushButtonBrowseAtomScore,0,1,1,1)
        self.gridlayout2.addLayout(self.gridlayout3,1,0,1,1)

        spacerItem = QtGui.QSpacerItem(20,40,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout2.addItem(spacerItem,2,0,1,1)
        self.tabWidgetScoredAtoms.addTab(self.tab_2,"")
        self.gridlayout.addWidget(self.tabWidgetScoredAtoms,1,0,1,2)

        self.line = QtGui.QFrame(DialogVolumeSSEBuilder)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.gridlayout.addWidget(self.line,2,0,1,2)

        self.labelSelection = QtGui.QLabel(DialogVolumeSSEBuilder)
        self.labelSelection.setObjectName("labelSelection")
        self.gridlayout.addWidget(self.labelSelection,3,0,1,1)

        self.tableWidgetSelection = QtGui.QTableWidget(DialogVolumeSSEBuilder)
        self.tableWidgetSelection.setEnabled(False)
        self.tableWidgetSelection.setAlternatingRowColors(True)
        self.tableWidgetSelection.setObjectName("tableWidgetSelection")
        self.gridlayout.addWidget(self.tableWidgetSelection,4,0,1,2)

        self.pushButtonSelectionToHelix = QtGui.QPushButton(DialogVolumeSSEBuilder)
        self.pushButtonSelectionToHelix.setObjectName("pushButtonSelectionToHelix")
        self.gridlayout.addWidget(self.pushButtonSelectionToHelix,5,0,1,1)

        self.pushButtonSelectionToSheet = QtGui.QPushButton(DialogVolumeSSEBuilder)
        self.pushButtonSelectionToSheet.setObjectName("pushButtonSelectionToSheet")
        self.gridlayout.addWidget(self.pushButtonSelectionToSheet,5,1,1,1)

        self.retranslateUi(DialogVolumeSSEBuilder)
        self.tabWidgetScoredAtoms.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(DialogVolumeSSEBuilder)
        DialogVolumeSSEBuilder.setTabOrder(self.tabWidgetScoredAtoms,self.pushButtonLoadVolume)
        DialogVolumeSSEBuilder.setTabOrder(self.pushButtonLoadVolume,self.pushButtonLoadSkeleton)
        DialogVolumeSSEBuilder.setTabOrder(self.pushButtonLoadSkeleton,self.doubleSpinBoxThreshold)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxThreshold,self.doubleSpinBoxResolution)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxResolution,self.doubleSpinBoxSkeleton)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxSkeleton,self.doubleSpinBoxCorrelation)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxCorrelation,self.doubleSpinBoxGeometry)
        DialogVolumeSSEBuilder.setTabOrder(self.doubleSpinBoxGeometry,self.pushButtonSSEHunter)
        DialogVolumeSSEBuilder.setTabOrder(self.pushButtonSSEHunter,self.lineEditAtomScore)
        DialogVolumeSSEBuilder.setTabOrder(self.lineEditAtomScore,self.pushButtonBrowseAtomScore)
        DialogVolumeSSEBuilder.setTabOrder(self.pushButtonBrowseAtomScore,self.tableWidgetSelection)
        DialogVolumeSSEBuilder.setTabOrder(self.tableWidgetSelection,self.pushButtonSelectionToHelix)
        DialogVolumeSSEBuilder.setTabOrder(self.pushButtonSelectionToHelix,self.pushButtonSelectionToSheet)

    def retranslateUi(self, DialogVolumeSSEBuilder):
        DialogVolumeSSEBuilder.setWindowTitle(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "SSE Builder", None, QtGui.QApplication.UnicodeUTF8))
        self.labelAtomScore.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Scored Pseudoatoms:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonLoadVolume.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Load Volume", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonLoadSkeleton.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Load Skeleton", None, QtGui.QApplication.UnicodeUTF8))
        self.labelThreshold.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Threshold:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelVolumeResolution.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Volume Resolution (A):", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSkeletonScore.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Skeleton Influence:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelCrossCorrelationScore.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Helix Correlation Influence:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelGeometricScore.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Geometric Influence:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonSSEHunter.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Find Scored Pseudoatoms (SSEHunter)", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidgetScoredAtoms.setTabText(self.tabWidgetScoredAtoms.indexOf(self.tab), QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Calculate using SSEHunter", None, QtGui.QApplication.UnicodeUTF8))
        self.labelAtomScore_2.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Scored Pseudoatom File:", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonBrowseAtomScore.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "...", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidgetScoredAtoms.setTabText(self.tabWidgetScoredAtoms.indexOf(self.tab_2), QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Load from file", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSelection.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Atom Selection:", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetSelection.clear()
        self.tableWidgetSelection.setColumnCount(2)
        self.tableWidgetSelection.setRowCount(0)

        headerItem = QtGui.QTableWidgetItem()
        headerItem.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "ID", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetSelection.setHorizontalHeaderItem(0,headerItem)

        headerItem1 = QtGui.QTableWidgetItem()
        headerItem1.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "SSE Hunter Score", None, QtGui.QApplication.UnicodeUTF8))
        self.tableWidgetSelection.setHorizontalHeaderItem(1,headerItem1)
        self.pushButtonSelectionToHelix.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Add Helix", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButtonSelectionToSheet.setText(QtGui.QApplication.translate("DialogVolumeSSEBuilder", "Add Sheet", None, QtGui.QApplication.UnicodeUTF8))

