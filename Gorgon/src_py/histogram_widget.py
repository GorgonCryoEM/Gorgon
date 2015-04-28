# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A histogram panel to display histogram based data.


from PyQt4 import QtCore, QtGui
from math import log

class HistogramWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.verticalBorderSize = 7
        self.horizontalBorderSize = 2
        
        self.clearData()
        self.binCount = 4
        self.automaticBinning = False
        self.useLogScale = False;
        self.brush = QtGui.QBrush(QtGui.QColor.fromRgb(128, 128, 128, 255))
        self.binBrush = QtGui.QBrush(QtGui.QColor.fromRgb(0,0,0))
        self.setMinimumSize(QtCore.QSize(0,40))
        
    def paintEvent(self, event):
        QtGui.QWidget.paintEvent(self, event)
        painter = QtGui.QPainter()
        painter.begin(self)
        painter.fillRect(self.verticalBorderSize, 
                         self.horizontalBorderSize, 
                         self.width()-2*self.verticalBorderSize, 
                         self.height()-2*self.horizontalBorderSize, 
                         self.brush)
        
        if((len(self.histogram) > 0) and (self.maxBinSize > 0)):
            binWidth = (self.width() - 2*self.verticalBorderSize) / self.binCount
            binMaxHeight = self.height() - 2*self.horizontalBorderSize
            for i in range(self.binCount):   
                if(self.useLogScale):
                    if(self.histogram[i] == 0):
                        binHeight = 0
                    else:
                        binHeight = (binMaxHeight*(log(self.histogram[i])/log(self.maxBinSize)))
                else:
                    binHeight = (binMaxHeight*(float(self.histogram[i])/self.maxBinSize))
                    
                painter.fillRect(self.verticalBorderSize + binWidth * i, 
                                 self.height() - binHeight - self.horizontalBorderSize, 
                                 binWidth, 
                                 binHeight, 
                                 self.binBrush)
        painter.end()
        
    def clearData(self):
        self.dataItems = []
        self.histogram = []
        self.minValue = 0
        self.maxValue = 0
        self.maxBinSize = 0;
        
    def setHistogram(self, histogram):
        self.histogram = histogram       
        self.maxBinSize = max(self.histogram)
        self.update()
        
    def setBinCount(self, count):
        self.binCount = count
        
    def setUseLogScale(self, logScale):
        self.useLogScale = logScale
        
    def setAutomaticBinning(self, binning):
        self.automaticBinning = binning
        
    def addData(self, value):
        if(len(self.dataItems) == 0):
            self.minValue = value
            self.maxValue = value
        else:
            self.minValue = min(value, self.minValue)
            self.maxValue = max(value, self.maxValue)
        self.dataItems.append(value)
        
    def minimumValue(self):
        return self.minValue
    
    def maximumValue(self):
        return self.maxValue
    
    def finalizeData(self):
        if(not self.automaticBinning):
            self.binData(self.binCount)
        
    def binData(self, binCount):
        self.histogram = []
        for i in range(binCount):
            self.histogram.append(0)

        if(len(self.dataItems) > 1):
            binSize = (self.maxValue - self.minValue) / float(self.binCount-1)
            
        for i in range(len(self.dataItems)):
            binIx = int((self.dataItems[i] - self.minValue) / binSize)
            self.histogram[binIx] = self.histogram[binIx]+1
        
        self.maxBinSize = max(self.histogram)
        
    def resizeEvent (self, event):
        self.emitWidgetResized()
    
    def emitWidgetResized(self):
        self.emit(QtCore.SIGNAL("widgetResized()"))    
        
