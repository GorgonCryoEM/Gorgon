from PyQt4 import QtGui
from Toolkit.sse.seq_model.Chain import Chain
from ui_dialog_to_load import Ui_DialogCAlphaChooseChainToLoad


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
