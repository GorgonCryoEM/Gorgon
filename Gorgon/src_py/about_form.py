# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved.
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A utility class which hooks up events for a form which displays information about Gorgon. 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.5  2008/06/18 17:56:32  ssa1
#   Adding in CVS meta data
#


from PyQt4 import QtCore, QtGui
from ui_dialog_about import Ui_DialogAbout

class AboutForm(QtGui.QDialog):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.ui = Ui_DialogAbout()
        self.ui.setupUi(self)
        self.createActions()
        self.createMenus()

    def createActions(self):               
        aboutAct = QtGui.QAction(self.tr("&About"), self)
        aboutAct.setStatusTip(self.tr("About Gorgon"))
        self.connect(aboutAct, QtCore.SIGNAL("triggered()"), self.show)
        self.app.actions.addAction("show_AboutForm", aboutAct)
  
    def createMenus(self):
        self.app.menus.addAction("help-about", self.app.actions.getAction("show_AboutForm"), "help")        