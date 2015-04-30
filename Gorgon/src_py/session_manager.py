# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A utility module for string parsing when storing a session


import pickle

class SessionManager(object):
    
    def __init__(self, main, parent=None):
        self.app = main
        self.sessionRemarkNo = 1    
       
    def getRemarkLine(self, text):
        return "REMARK " + str(self.sessionRemarkNo).rjust(3) + " GORGON " + text.ljust(52) + "\n"
    
    def getRemarkLines(self, component, tag, value):
        lines = []
        newText = pickle.dumps(value).replace("\n", "<GORGON:EOL>")
        
        
        baseLength = len(component) + len(tag) + 2;
        
        while(len(newText) > 0):
            line = component + " " + tag + " " + newText[0:(52-baseLength)]
            newText = newText[(52-baseLength):len(newText)]
            lines.append(self.getRemarkLine(line))
        self.sessionRemarkNo = self.sessionRemarkNo + 1
            
        return lines
    
    def parseTextLine(self, line):
        retVal = None
        if(line[0:7] == "REMARK "):
            try:
                lineNo = int(line[7:10])
                if(line[11:18] == "GORGON "):
                    unit = line[18:21]
                    property = (line[22:70]).split(" ")[0]
                    val = line[23+len(property):70]
                    retVal = [lineNo, unit, property, val]                    
            except:
                retVal = None            
            
        return retVal
            
        
    def parseTextLines(self, textLines):
        lineVals = []
        for i in range(len(textLines)):
            valList = self.parseTextLine(textLines[i])
            if valList:
                lineVals.append(valList)
        
        parsedLines = {}
        textVal = ""
        for i in range(len(lineVals)):
            [lineNo, unit, property, val] = lineVals[i]
            
            if (i < len(lineVals)-1) :
                nextLineNo = lineVals[i+1][0]
            else:
                nextLineNo = lineNo + 1
                
            textVal = textVal + val 
            if (lineNo != nextLineNo):
                actualVal = pickle.loads(textVal.replace("<GORGON:EOL>", "\n"))
                parsedLines[unit + " " + property] = actualVal
                textVal = ""                
        return parsedLines
    
    def getProperty(self, parsedLines, unit, property):
        return parsedLines[unit + " " + property]               
                
        
