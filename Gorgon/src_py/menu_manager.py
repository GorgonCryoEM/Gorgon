# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   Maintains all the menu items in the application 


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
