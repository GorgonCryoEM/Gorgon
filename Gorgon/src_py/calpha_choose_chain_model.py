# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   Widget that lets the user see what chain models are in memory and select the one to work with


import sys
from PyQt4 import QtCore, QtGui
from ui_dialog_calpha_choose_chain_model import Ui_DialogCAlphaChooseChainModel
from seq_model.Chain import Chain
from base_dock_widget import BaseDockWidget

class CAlphaChooseChainModel(BaseDockWidget):
    def __init__(self, main, parent=None):
        BaseDockWidget.__init__(self, 
                                main, 
                                "&Choose Chain Model", 
                                "Choose a chain model", 
                                "perform_chooseModel", 
                                "actions-calpha-chooseModel", 
                                "actions-calpha", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.RightDockWidgetArea, 
                                parent)
        self.app = main
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

