# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   Maintains all the menu items in the application 

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/menu_manager.py,v $
#   $Revision: 1.4 $
#   $Date: 2008/06/18 18:15:41 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: menu_manager.py,v $
#   Revision 1.4  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtGui

class MenuManager(object):
    def __init__(self, main, parent=None):
        self.app = main
        self.menuList = {}

    def getMenu(self, tag):
        if(tag in self.menuList) :
            return self.menuList[tag]
        else :
            raise Exception("Attempting to use non-existant menu item: " + tag)
        
    def addMenu(self, tag, text, parentTag=""):        
        if (parentTag == "") :
            parentMenu = self.app.menuBar()
        else :
            parentMenu = self.getMenu(parentTag) 
            
        self.menuList[tag] = parentMenu.addMenu(text)
        return self.menuList[tag]
    
    def addAction(self, tag, action, parentTag=""):
        if (parentTag == "") :
            parentMenu = self.app.menuBar()
        else :
            parentMenu = self.getMenu(parentTag)         
        parentMenu.addAction(action)