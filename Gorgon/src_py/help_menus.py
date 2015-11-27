# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved.
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   Shows the help menus


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
        
