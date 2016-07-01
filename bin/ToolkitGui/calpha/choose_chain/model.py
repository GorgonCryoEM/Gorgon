import sys
from PyQt4 import QtCore, QtGui
from ui_model import Ui_DialogCAlphaChooseChainModel
from Toolkit.sse.seq_model.Chain import Chain


class CAlphaChooseChainModel(QtGui.QDialog):

    def __init__(self, main, parent=None):
        QtGui.QDialog.__init__(self, main)
        self.app = main

        dock = QtGui.QDockWidget("CAlpha Choose Chain Model", self.app)
        dock.setWidget(self)
        dock.setAllowedAreas(QtCore.Qt.AllDockWidgetAreas)
        self.app.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)

        self.createUi()
        self.connect(self.ui.refreshPushButton, QtCore.SIGNAL("clicked()"), self.refresh)
        self.connect(self.ui.chainModelsListWidget, QtCore.SIGNAL("currentTextChanged (const QString&)"), self.modelHighlighted)
    
    def acceptButtonPress(self):
        pass
           
    def createUi(self):
        self.ui = Ui_DialogCAlphaChooseChainModel()
        self.ui.setupUi(self)
        self.ui.sequenceTextEdit.setReadOnly(True)
        self.ui.acceptPushButton.setEnabled(False)
        
    def loadWidget(self):
        BaseDockWidget.loadWidget(self)
        if self.app.actions.getAction("perform_chooseModel").isChecked():
            self.refresh()

    def modelHighlighted(self, chainQString):
        currText = str(chainQString)
        currChainKey = tuple( currText.split(' - ') )
        currChain = Chain.getChain(currChainKey)
        text = str(currChain)
        self.ui.sequenceTextEdit.setText(text)
        
    def refresh(self):
        self.ui.chainModelsListWidget.clear()
        chainKeys = Chain.getChainKeys()
        for key in chainKeys:
            item = str(key[0]) + ' - ' + str(key[1])
            self.ui.chainModelsListWidget.addItem(item)

