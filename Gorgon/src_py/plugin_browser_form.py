# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A dialog box which can be used to browse plugins

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$

from PyQt4 import QtCore, QtGui
from ui_dialog_plugin_browser import Ui_DialogPluginBrowser

class PluginBrowserForm(QtGui.QDialog, Ui_DialogPluginBrowser):
    def __init__(self, main, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.app = main
        self.setupUi(self)
        self.createActions()
        self.createMenus()
        
    def createActions(self):
        self.loadPluginsAct = QtGui.QAction(self.tr("&Plugin Browser"), self)        
        self.loadPluginsAct.setStatusTip(self.tr("View Loaded Plugins"))
        self.loadPluginsAct.setCheckable(True)
        self.loadPluginsAct.setChecked(False)   
        self.connect(self.loadPluginsAct, QtCore.SIGNAL("triggered()"), self.toggleVisibility)
        self.app.actions.addAction("show_PluginBrowser", self.loadPluginsAct)
        
    def createMenus(self):
        self.app.menus.addAction("window-PluginBrowser", self.app.actions.getAction("show_PluginBrowser"), "window")
        
    def toggleVisibility(self):
        if(self.isVisible()):
            self.close()
        else:
            self.show()        
        
    def show(self):
        QtGui.QDialog.show(self)
        self.loadPluginsAct.setChecked(self.isVisible())
        
    def closeEvent(self, event):
        QtGui.QDialog.closeEvent(self, event)
        self.loadPluginsAct.setChecked(self.isVisible())
        
    def addLoadedPlugin(self, id, name, description):
        rownum = self.tableWidgetLoadedPlugins.rowCount()
        self.tableWidgetLoadedPlugins.insertRow(rownum)
        self.tableWidgetLoadedPlugins.setItem(rownum, 0, QtGui.QTableWidgetItem(id))
        self.tableWidgetLoadedPlugins.setItem(rownum, 1, QtGui.QTableWidgetItem(name))
        self.tableWidgetLoadedPlugins.setItem(rownum, 2, QtGui.QTableWidgetItem(description))
        
    
    def addFailedModule(self, fileName, filePath, errorText):
        rownum = self.tableWidgetNotLoadedPlugins.rowCount()
        self.tableWidgetNotLoadedPlugins.insertRow(rownum)
        self.tableWidgetNotLoadedPlugins.setItem(rownum, 0, QtGui.QTableWidgetItem(fileName))
        self.tableWidgetNotLoadedPlugins.setItem(rownum, 1, QtGui.QTableWidgetItem(filePath))
        self.tableWidgetNotLoadedPlugins.setItem(rownum, 3, QtGui.QTableWidgetItem(errorText))
    
    def addFailedPlugin(self, fileName, filePath, className, errorText):
        rownum = self.tableWidgetNotLoadedPlugins.rowCount()
        self.tableWidgetNotLoadedPlugins.insertRow(rownum)
        self.tableWidgetNotLoadedPlugins.setItem(rownum, 0, QtGui.QTableWidgetItem(fileName))
        self.tableWidgetNotLoadedPlugins.setItem(rownum, 1, QtGui.QTableWidgetItem(filePath))
        self.tableWidgetNotLoadedPlugins.setItem(rownum, 2, QtGui.QTableWidgetItem(className))
        self.tableWidgetNotLoadedPlugins.setItem(rownum, 3, QtGui.QTableWidgetItem(errorText))
        
    def accept(self):
        self.close()
            
            