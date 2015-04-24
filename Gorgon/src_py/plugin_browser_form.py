# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A dialog box which can be used to browse plugins

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/plugin_browser_form.py,v $
#   $Revision: 1.3 $
#   $Date: 2009/12/24 07:25:07 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: plugin_browser_form.py,v $
#   Revision 1.3  2009/12/24 07:25:07  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior
#
#   Revision 1.2  2009/12/24 05:09:30  ssa1
#   Refactoring child window behavior.. Using base classes to encapsulate common behavior
#
#   Revision 1.1  2009/04/08 19:54:59  ssa1
#   Adding in plugin functionality
#

from PyQt4 import QtCore, QtGui
from ui_dialog_plugin_browser import Ui_DialogPluginBrowser
from base_dialog_widget import BaseDialogWidget

class PluginBrowserForm(BaseDialogWidget, Ui_DialogPluginBrowser):
    def __init__(self, main, parent=None):
        BaseDialogWidget.__init__(self, 
                                  main, 
                                  "&Plugin Browser", 
                                  "View loaded plugins", 
                                  "show_PluginBrowser", 
                                  "window-PluginBrowser", 
                                  "window", 
                                  True,
                                  parent)
        self.app = main
        self.setupUi(self)
        
        
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
        BaseDialogWidget.accept(self)
            
            