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
#   Revision 1.8  2008/12/01 21:02:14  ssa1
#   Putting in link for related publications
#
#   Revision 1.7  2008/11/29 04:48:28  ssa1
#   Icon support and Redirecting help to website.
#
#   Revision 1.6  2008/06/18 17:59:02  ssa1
#   Adding in CVS meta data
#
#   Revision 1.5  2008/06/18 17:56:32  ssa1
#   Adding in CVS meta data
#


from PyQt4 import QtCore, QtGui
from ui_dialog_about import Ui_DialogAbout
import webbrowser

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
        self.connect(aboutAct, QtCore.SIGNAL("triggered()"), self.showAbout)
        self.app.actions.addAction("show_AboutForm", aboutAct)
        
        updatesAct = QtGui.QAction(self.tr("Get &Updates"), self)
        updatesAct.setStatusTip(self.tr("Download Gorgon Updates"))
        self.connect(updatesAct, QtCore.SIGNAL("triggered()"), self.getUpdates)
        self.app.actions.addAction("show_GetUpdatesForm", updatesAct)        

        guideAct = QtGui.QAction(self.tr("User &Guide"), self)
        guideAct.setStatusTip(self.tr("User Guide"))
        self.connect(guideAct, QtCore.SIGNAL("triggered()"), self.userGuide)
        self.app.actions.addAction("show_UserGuideForm", guideAct)   

        pubAct = QtGui.QAction(self.tr("Related Publications"), self)
        pubAct.setStatusTip(self.tr("Related Publications"))
        self.connect(pubAct, QtCore.SIGNAL("triggered()"), self.publications)
        self.app.actions.addAction("show_PublicationsForm", pubAct)   

        bugAct = QtGui.QAction(self.tr("Report Bugs"), self)
        bugAct.setStatusTip(self.tr("Report bug"))
        self.connect(bugAct, QtCore.SIGNAL("triggered()"), self.bugReport)
        self.app.actions.addAction("show_BugReportForm", bugAct)   
  
    def createMenus(self):
        self.app.menus.addAction("help-about", self.app.actions.getAction("show_AboutForm"), "help")
        self.app.menus.addAction("help-updates", self.app.actions.getAction("show_GetUpdatesForm"), "help")
        self.app.menus.addAction("help-guide", self.app.actions.getAction("show_UserGuideForm"), "help")
        self.app.menus.addAction("help-publications", self.app.actions.getAction("show_PublicationsForm"), "help")
        self.app.menus.addAction("help-bugs", self.app.actions.getAction("show_BugReportForm"), "help")
        
    def openPage(self, url):
        try:
            webbrowser.open(url)
        except WindowsError:
            pass;
        

    def showAbout(self):
        self.openPage('http://cse.wustl.edu/~ssa1/gorgon/')
        
    def getUpdates(self):          
        self.openPage('http://cse.wustl.edu/~ssa1/gorgon/pages/download.php')      
        
    def userGuide(self):          
        self.openPage('http://cse.wustl.edu/~ssa1/gorgon/pages/userGuide.php')    
        
    def publications(self):
        self.openPage('http://www.cs.wustl.edu/~ssa1/gorgon/pages/publications.php')
        
    def bugReport(self):
        self.openPage('http://www.cs.wustl.edu/~ssa1/gorgon/pages/contactUs.php')
        