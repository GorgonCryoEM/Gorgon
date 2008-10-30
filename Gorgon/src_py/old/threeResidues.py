import sys
import PyQt4.QtGui as QtGui

class Ui_threeResidues(QtGui.QWidget):
    def __init__(self, parent=None):
		super(Ui_threeResidues, self).__init__(parent)
    def setupUI(self):		
		self.undoButton = QtGui.QPushButton('Undo')
		self.redoButton = QtGui.QPushButton('Redo')
		self.CAdoubleSpinBox = QtGui.QDoubleSpinBox()
		self.CAlabel = QtGui.QLabel('C-Alplha Interval')
		self.mockSidechainsCheckBox = QtGui.QCheckBox('Mock Sidechains')
		self.acceptButton = QtGui.QPushButton('Accept')
		self.tabWidget = QtGui.QTabWidget()
		self.helixTab = QtGui.QWidget()
		self.atomicTab = QtGui.QWidget()
		self.loopTab = QtGui.QWidget()
		
		self.possibilityNumSpinBox = QtGui.QSpinBox()
		self.numPossibilities = QtGui.QLabel('of 3')
		resNameFont = QtGui.QFont(self.font())
		resNameFont.setPointSize(41)
		resIndexFont = QtGui.QFont(self.font())
		resIndexFont.setPointSize(13)
		
		self.prevName = QtGui.QLabel('A')
		self.prevName.setFont(resNameFont)
		self.prevNum = QtGui.QLabel('167')
		self.prevNum.setFont(resIndexFont)
		self.curName = QtGui.QLabel('Q')
		self.curName.setFont(resNameFont)
		self.curNum = QtGui.QLabel('168')
		self.curNum.setFont(resIndexFont)
		self.nextName = QtGui.QLabel('F')
		self.nextName.setFont(resNameFont)
		self.nextNum = QtGui.QLabel('169')
		self.nextNum.setFont(resIndexFont)
		self.back1resButton = QtGui.QPushButton('<-')
		self.forward1resButton = QtGui.QPushButton('->')		
		
		layout = QtGui.QHBoxLayout()
		
		leftLayout = QtGui.QVBoxLayout()
		
		undoRedoLayout = QtGui.QHBoxLayout()
		undoRedoLayout.addWidget(self.undoButton)
		undoRedoLayout.addWidget(self.redoButton)
		
		leftLayout.addLayout(undoRedoLayout)
		leftLayout.addWidget(self.mockSidechainsCheckBox)
		
		CAIntervalLayout = QtGui.QHBoxLayout()
		CAIntervalLayout.addWidget(self.CAdoubleSpinBox)
		CAIntervalLayout.addWidget(self.CAlabel)
		
		leftLayout.addLayout(CAIntervalLayout)
		leftLayout.addWidget(self.acceptButton)
		
		self.tabWidget.addTab(self.helixTab, self.tr('Helix Editor'))
		self.tabWidget.addTab(self.atomicTab, self.tr('Atomic Editor'))
		self.tabWidget.addTab(self.loopTab, self.tr('Loop Editor'))
		
		atomicLayout = QtGui.QVBoxLayout()
		
		atomicPossibilityLayout = QtGui.QHBoxLayout()
		atomicPossibilityLayout.addWidget(self.possibilityNumSpinBox)
		atomicPossibilityLayout.addWidget(self.numPossibilities)
		atomicLayout.addLayout(atomicPossibilityLayout)
		
		atomic3ResLayout = QtGui.QGridLayout()
		atomic3ResLayout.addWidget(self.prevName,1,0)
		atomic3ResLayout.addWidget(self.curName,1,1)
		atomic3ResLayout.addWidget(self.nextName,1,2)
		atomic3ResLayout.addWidget(self.prevNum,0,0)
		atomic3ResLayout.addWidget(self.curNum, 0, 1)
		atomic3ResLayout.addWidget(self.nextNum, 0, 2)
		atomicLayout.addLayout(atomic3ResLayout)

		atomicResButtonLayout = QtGui.QHBoxLayout()
		atomicResButtonLayout.addWidget(self.back1resButton)
		atomicResButtonLayout.addWidget(self.forward1resButton)
		atomicLayout.addLayout(atomicResButtonLayout)
		self.atomicTab.setLayout(atomicLayout)
		
		layout.addLayout(leftLayout)
		layout.addWidget(self.tabWidget)
		self.setLayout(layout)
		
app = QtGui.QApplication(sys.argv)
window = Ui_threeResidues()
window.show()
sys.exit(app.exec_())
