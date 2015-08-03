# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_dialog_skeleton_laplacian_smoothing.ui'
#
# Created: Fri May 30 11:47:45 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_DialogSkeletonLaplacianSmoothing(object):
    def setupUi(self, DialogSkeletonLaplacianSmoothing):
        DialogSkeletonLaplacianSmoothing.setObjectName("DialogSkeletonLaplacianSmoothing")
        DialogSkeletonLaplacianSmoothing.resize(QtCore.QSize(QtCore.QRect(0,0,281,96).size()).expandedTo(DialogSkeletonLaplacianSmoothing.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(DialogSkeletonLaplacianSmoothing)
        self.gridlayout.setObjectName("gridlayout")

        self.labelLaplacianSmoothingConvergenceRate = QtGui.QLabel(DialogSkeletonLaplacianSmoothing)
        self.labelLaplacianSmoothingConvergenceRate.setObjectName("labelLaplacianSmoothingConvergenceRate")
        self.gridlayout.addWidget(self.labelLaplacianSmoothingConvergenceRate,0,0,1,1)

        self.doubleSpinBoxConvergenceRate = QtGui.QDoubleSpinBox(DialogSkeletonLaplacianSmoothing)
        self.doubleSpinBoxConvergenceRate.setMaximum(1.0)
        self.doubleSpinBoxConvergenceRate.setSingleStep(0.01)
        self.doubleSpinBoxConvergenceRate.setProperty("value",QtCore.QVariant(0.25))
        self.doubleSpinBoxConvergenceRate.setObjectName("doubleSpinBoxConvergenceRate")
        self.gridlayout.addWidget(self.doubleSpinBoxConvergenceRate,0,1,1,1)

        self.labelNoOfIterations = QtGui.QLabel(DialogSkeletonLaplacianSmoothing)
        self.labelNoOfIterations.setObjectName("labelNoOfIterations")
        self.gridlayout.addWidget(self.labelNoOfIterations,1,0,1,1)

        self.spinBoxNoOfIterations = QtGui.QSpinBox(DialogSkeletonLaplacianSmoothing)
        self.spinBoxNoOfIterations.setMinimum(1)
        self.spinBoxNoOfIterations.setMaximum(100)
        self.spinBoxNoOfIterations.setProperty("value",QtCore.QVariant(5))
        self.spinBoxNoOfIterations.setObjectName("spinBoxNoOfIterations")
        self.gridlayout.addWidget(self.spinBoxNoOfIterations,1,1,1,1)

        self.buttonBox = QtGui.QDialogButtonBox(DialogSkeletonLaplacianSmoothing)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridlayout.addWidget(self.buttonBox,2,0,1,2)

        self.retranslateUi(DialogSkeletonLaplacianSmoothing)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("accepted()"),DialogSkeletonLaplacianSmoothing.accept)
        QtCore.QObject.connect(self.buttonBox,QtCore.SIGNAL("rejected()"),DialogSkeletonLaplacianSmoothing.reject)
        QtCore.QMetaObject.connectSlotsByName(DialogSkeletonLaplacianSmoothing)
        DialogSkeletonLaplacianSmoothing.setTabOrder(self.doubleSpinBoxConvergenceRate,self.spinBoxNoOfIterations)
        DialogSkeletonLaplacianSmoothing.setTabOrder(self.spinBoxNoOfIterations,self.buttonBox)

    def retranslateUi(self, DialogSkeletonLaplacianSmoothing):
        DialogSkeletonLaplacianSmoothing.setWindowTitle(QtGui.QApplication.translate("DialogSkeletonLaplacianSmoothing", "Skeleton - Laplacian Smoothing", None, QtGui.QApplication.UnicodeUTF8))
        self.labelLaplacianSmoothingConvergenceRate.setText(QtGui.QApplication.translate("DialogSkeletonLaplacianSmoothing", "Convergence rate:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelNoOfIterations.setText(QtGui.QApplication.translate("DialogSkeletonLaplacianSmoothing", "No of iterations:", None, QtGui.QApplication.UnicodeUTF8))

