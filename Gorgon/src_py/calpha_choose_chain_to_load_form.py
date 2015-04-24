# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   This dialog prompts the user for which chain to load

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/calpha_choose_chain_to_load_form.py,v $
#   $Revision: 1.1 $
#   $Date: 2009/04/03 19:44:37 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: calpha_choose_chain_to_load_form.py,v $
#   Revision 1.1  2009/04/03 19:44:37  ssa1
#   CAlpha bug fixes
#

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
