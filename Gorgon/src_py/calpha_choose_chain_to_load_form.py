# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   This dialog prompts the user for which chain to load


from PyQt4 import QtGui
from seq_model.Chain import Chain
from ui_dialog_calpha_choose_chain_to_load import Ui_DialogCAlphaChooseChainToLoad

class CAlphaChooseChainToLoadForm(QtGui.QDialog, Ui_DialogCAlphaChooseChainToLoad):
    def __init__(self, fileName, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.setupUi(self)
        chainIDs = Chain.getChainIDsFromPDB(fileName)
        self.listWidgetChainIDList.addItems(chainIDs)
        self.listWidgetChainIDList.addItem('ALL')
        
    def accept(self):
        self.whichChainID = str( self.listWidgetChainIDList.currentItem().text() )
        QtGui.QDialog.accept(self)
