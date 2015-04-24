# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved.
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   Shows the help menus

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/help_menus.py,v $
#   $Revision: 1.2 $
#   $Date: 2010/02/23 21:17:31 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: help_menus.py,v $
#   Revision 1.2  2010/02/23 21:17:31  ssa1
#   Changing gorgon website.
#
#   Revision 1.1  2008/12/18 15:19:31  ssa1
#   Moving About Form functionality into HelpMenus
#
#   Revision 1.15  2008/12/17 16:00:04  ssa1
#   Changing Version information for next public release
#
#   Revision 1.14  2008/12/04 20:55:16  ssa1
#   fixing bugs when running bug report in macos
#
#   Revision 1.13  2008/12/04 20:23:09  ssa1
#   Adding link to license
#
#   Revision 1.12  2008/12/04 20:02:01  ssa1
#   sending system information when submitting bug report
#
#   Revision 1.11  2008/12/04 01:03:31  ssa1
#   adding in bug tracker
#
#   Revision 1.10  2008/12/03 20:29:55  ssa1
#   adding in bug tracker
#
#   Revision 1.9  2008/12/03 20:27:05  ssa1
#   adding in bug tracker
#
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

import sys
from PyQt4 import QtCore, QtGui
import webbrowser

class HelpMenus(QtGui.QWidget):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)        
        self.app = main
        self.createActions()
        self.createMenus()

    def createActions(self):               
        aboutAct = QtGui.QAction(self.tr("&About..."), self)
        aboutAct.setStatusTip(self.tr("About Gorgon"))
        self.connect(aboutAct, QtCore.SIGNAL("triggered()"), self.showAbout)
        self.app.actions.addAction("show_AboutForm", aboutAct)
        
        updatesAct = QtGui.QAction(self.tr("Get &Updates..."), self)
        updatesAct.setStatusTip(self.tr("Download Gorgon Updates"))
        self.connect(updatesAct, QtCore.SIGNAL("triggered()"), self.getUpdates)
        self.app.actions.addAction("show_GetUpdatesForm", updatesAct)        

        guideAct = QtGui.QAction(self.tr("User &Guide..."), self)
        guideAct.setStatusTip(self.tr("User Guide"))
        self.connect(guideAct, QtCore.SIGNAL("triggered()"), self.userGuide)
        self.app.actions.addAction("show_UserGuideForm", guideAct)   

        bugAct = QtGui.QAction(self.tr("Report &Bugs..."), self)
        bugAct.setStatusTip(self.tr("Report bug"))
        self.connect(bugAct, QtCore.SIGNAL("triggered()"), self.bugReport)
        self.app.actions.addAction("show_BugReportForm", bugAct)   
        
        licenseAct = QtGui.QAction(self.tr("&License Agreement..."), self)
        licenseAct.setStatusTip(self.tr("License Agreement"))
        self.connect(licenseAct, QtCore.SIGNAL("triggered()"), self.license)
        self.app.actions.addAction("show_LicenseForm", licenseAct)   
                
        pubAct = QtGui.QAction(self.tr("Related &Publications..."), self)
        pubAct.setStatusTip(self.tr("Related Publications"))
        self.connect(pubAct, QtCore.SIGNAL("triggered()"), self.publications)
        self.app.actions.addAction("show_PublicationsForm", pubAct)   
  
    def createMenus(self):
        self.app.menus.addAction("help-about", self.app.actions.getAction("show_AboutForm"), "help")
        self.app.menus.addAction("help-updates", self.app.actions.getAction("show_GetUpdatesForm"), "help")
        self.app.menus.addAction("help-guide", self.app.actions.getAction("show_UserGuideForm"), "help")
        self.app.menus.addAction("help-bugs", self.app.actions.getAction("show_BugReportForm"), "help")
        self.app.menus.addAction("help-license", self.app.actions.getAction("show_LicenseForm"), "help")
        self.app.menus.addAction("help-publications", self.app.actions.getAction("show_PublicationsForm"), "help")
        
    def openPage(self, url):
        try:
            webbrowser.open(url)
        except:
            pass;
        

    def showAbout(self):
        self.openPage('http://gorgon.wustl.edu/')
        
    def getUpdates(self):          
        self.openPage('http://gorgon.wustl.edu/pages/download.php')      
        
    def userGuide(self):          
        self.openPage('http://gorgon.wustl.edu/pages/userGuide.php')    
        
    def publications(self):
        self.openPage('http://gorgon.wustl.edu/pages/publications.php')
    
    def license(self):
        self.openPage('http://gorgon.wustl.edu/pages/viewLicense.php')
        
    def bugReport(self):
        systemInfo = "Gorgon Version: " + self.app.version + " | "
        systemInfo = systemInfo + "Platform: " + sys.platform + " | "
        if(sys.platform == "win32"):
            systemInfo = systemInfo +  "Windows version: " + str(sys.getwindowsversion()) + " | "
        systemInfo = systemInfo + "File system encoding: " + str(sys.getfilesystemencoding()) + " | "
        systemInfo = systemInfo + "Executable: " + sys.executable + " | "
        systemInfo = systemInfo + "Python Version: " + sys.version + "," + str(sys.version_info) + " | "
        #systemInfo = systemInfo + "Installed Modules: " + str(sys.modules) + " | "
        systemInfo = systemInfo.replace("'", "")
        self.openPage('http://gorgon.wustl.edu/pages/contactUs.php?sysinfo=' + systemInfo)
        