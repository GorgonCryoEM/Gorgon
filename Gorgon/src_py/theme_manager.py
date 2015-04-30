# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that manages the colors of the application 


from PyQt4 import QtGui, QtCore
import pickle

class ThemeManager(QtGui.QWidget):
    
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.colorList = {} 
        self.createActions()
        self.createMenus()   
        
    def createActions(self):
        openAct = QtGui.QAction(self.tr("&Load theme..."), self)
        openAct.setStatusTip(self.tr("Load color settings"))        
        self.connect(openAct, QtCore.SIGNAL("triggered()"), self.loadThemeAction)
        self.app.actions.addAction("load_theme", openAct)
        
        saveAct = QtGui.QAction(self.tr("&Save theme..."), self)
        saveAct.setStatusTip(self.tr("Save color settings"))        
        self.connect(saveAct, QtCore.SIGNAL("triggered()"), self.saveThemeAction)
        self.app.actions.addAction("save_theme", saveAct)        
    
    def createMenus(self):
        self.app.menus.addAction("themes-open", self.app.actions.getAction("load_theme"), "themes")    
        self.app.menus.addAction("themes-save", self.app.actions.getAction("save_theme"), "themes")  
    
    def loadThemeAction(self):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Theme"), "", self.tr("Themes (*.thm)"))
        if not fileName.isEmpty():  
            self.loadTheme(fileName)
    
    def saveThemeAction(self):
        fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Theme"), "", self.tr("Themes (*.thm)"))
        if not fileName.isEmpty(): 
            self.saveTheme(fileName)
    
    def loadTheme(self, filename):
        infile = open(filename, "r")
        self.colorList = pickle.load(infile)
        infile.close()
        self.emitThemeChanged()
    
    def saveTheme(self, filename):       
        outFile = open(filename, "w")
        pickle.dump(self.colorList, outFile)
        outFile.close()
                      
    
    def getColor(self, tag):
        if(not(tag in self.colorList)) :            
            self.addRGB(tag, 255, 255, 255, 255)
            print("Color " +  tag + " not found, returning default black!")
                                
        return self.colorList[tag]
        
    def addColor(self, tag, color):
        if(not(tag in self.colorList) or (self.colorList[tag] != color)):               
           self.colorList[tag] = color
        
    def addRGB(self, tag, r, g, b, a):
        self.addColor(tag, QtGui.QColor.fromRgb(r, g, b, a))    
        
    def addDefaultRGB(self, tag, r, g, b, a):       
        if(not(tag in self.colorList)) :            
            self.colorList[tag] = QtGui.QColor.fromRgb(r, g, b, a)      
            
    def emitThemeChanged(self):
        print "emitThemeChanged"
        self.emit(QtCore.SIGNAL("themeChanged()"))         
    
