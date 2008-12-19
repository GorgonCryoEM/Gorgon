# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Class: ChooseChainModel
# Class Description: Widget that lets the user see what chain models are in memory and select the one to work with

import sys
from PyQt4 import QtCore, QtGui
from seq_model.ui_dialog_choose_chain_model import Ui_DialogChooseChainModel
from seq_model.Chain import Chain

class ChooseChainModel(QtGui.QWidget):
    def __init__(self, parent=None):
        super(ChooseChainModel, self).__init__(parent)
        self.app = parent
        self.createUi()
        self.createActions()
        self.createMenus()
        
        self.connect(self.ui.refreshPushButton, QtCore.SIGNAL("clicked()"), self.refresh)
        self.connect(self.ui.chainModelsListWidget, QtCore.SIGNAL("currentTextChanged (const QString&)"), self.modelHighlighted)
    
    def acceptButtonPress(self):
        pass
        
    def createActions(self):
        chooseModelAct = QtGui.QAction(self.tr("&Choose Chain Model"), self)
        chooseModelAct.setStatusTip(self.tr("Choose Chain Model"))
        chooseModelAct.setCheckable(True)
        chooseModelAct.setChecked(False)
        self.connect(chooseModelAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.app.actions.addAction("perform_chooseModel", chooseModelAct)

    def createMenus(self):
        self.app.menus.addAction("actions-calpha-chooseModel", self.app.actions.getAction("perform_chooseModel"), "actions-calpha")
    
    def createUi(self):
        self.ui = Ui_DialogChooseChainModel()
        self.ui.setupUi(self)
        self.ui.sequenceTextEdit.setReadOnly(True)
        self.dock = QtGui.QDockWidget(self.tr("Choose Chain Model"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()
        
        self.ui.acceptPushButton.setEnabled(False)
    
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("perform_chooseModel").setChecked(visible)
    
    def loadWidget(self):
        if self.app.actions.getAction("perform_chooseModel").isChecked():
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.refresh()
            self.dock.show()
        else:
            self.app.removeDockWidget(self.dock)
    
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
        
        
if __name__=='__main__':
    app = QtGui.QApplication(sys.argv)
    window = ChooseChainModel()
    window.ui.chainModelsListWidget.addItem('aaa')
    window.ui.chainModelsListWidget.addItem('bbb')
    window.ui.chainModelsListWidget.addItem('ccc')
    window.ui.chainModelsListWidget.addItem('ddd')
    window.ui.chainModelsListWidget.addItem('eee')
    window.ui.sequenceBrowser.setText('RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR')
    window.show()
    sys.exit(app.exec_())
