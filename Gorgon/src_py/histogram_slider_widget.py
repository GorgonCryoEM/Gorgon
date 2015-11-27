# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A histogram panel to display histogram based data.


from PyQt4 import QtCore, QtGui
from histogram_widget import HistogramWidget
from math import log

class HistogramSliderWidget(HistogramWidget):
    
    HistogramSliderTypeRange, HistogramSliderTypeValue = range(2)
    
    def __init__(self, parent=None):
        HistogramWidget.__init__(self, parent)
        self.sliderType = HistogramSliderWidget.HistogramSliderTypeValue
        self.values = [0,0]
        self.minGapBetweenSliders = 0.01
        self.sliderBrush = QtGui.QBrush(QtGui.QColor.fromRgb(255,255,255,255))
        self.setMouseTracking(True)
        self.moveLower = False
        self.moveHigher = False  
        self.sliderRadius = 3      
        
        
        
    def paintEvent(self, event):
        HistogramWidget.paintEvent(self, event)
        painter = QtGui.QPainter()
        painter.begin(self)
        drawObjects = [False, False]
        
        if(self.sliderType == HistogramSliderWidget.HistogramSliderTypeValue) :
            drawObjects[0] = True
        elif (self.sliderType == HistogramSliderWidget.HistogramSliderTypeRange) :
            drawObjects[0] = True
            drawObjects[1] = True
        else:
            pass
            
        for i in range(2):
            if (drawObjects[i]):
                valLoc = (self.width()-2*self.verticalBorderSize) * (self.values[i] - self.minValue) / (self.maxValue - self.minValue)
                painter.fillRect(valLoc+self.sliderRadius, 0, 2*self.sliderRadius+1, self.height()-1, self.sliderBrush)
                painter.setPen(QtGui.QColor.fromRgb(100,100,100))
                painter.drawRect(valLoc+self.sliderRadius, 0, 2*self.sliderRadius+1, self.height()-1)
                
        painter.end()
        
        
    def setSliderType(self, type):
        if(self.sliderType != type):
            self.sliderType = type
        self.update()     
        
    def setHistogram(self, histogram, minDataValue, maxDataValue):
        HistogramWidget.setHistogram(self, histogram)
        self.minValue = minDataValue
        self.maxValue = maxDataValue
        self.update()
        
    def setValue(self, newValue):
        value = min(max(newValue, self.minValue), self.maxValue)
        
        if(self.values[0] != value):
           if(self.sliderType == self.HistogramSliderTypeValue):
               self.values[0] = value
               self.update()
               self.emitValueChanged(value)
               self.emitLowerValueChanged(value)
           elif (value + self.minGapBetweenSliders <= self.values[1]):
               self.values[0] = value
               self.update()
               self.emitValueChanged(value)
               self.emitLowerValueChanged(value)
            

    
    def setLowerValue(self, newValue):
        self.setValue(newValue)
        
    def setHigherValue(self, newValue):
        value = min(max(newValue, self.minValue), self.maxValue)
        if(self.values[1] != value) and (self.values[0] + self.minGapBetweenSliders <= value):
            self.values[1] = value
            self.update()
            self.emitHigherValueChanged(value)
        
    def value(self):
        return self.values[0]
    
    def lowerValue(self):
        return self.value()
    
    def higherValue(self):
        return self.values[1]
        
    def mouseCoordToValue(self, x, y):
        return self.minValue + (self.maxValue-self.minValue) * (float(x-self.verticalBorderSize) / float(self.width()-2*self.verticalBorderSize))
    
    def isMouseInLowerValue(self, x, y):
        valLoc = (self.width()-2*self.verticalBorderSize) * (self.values[0] - self.minValue) / (self.maxValue - self.minValue)
        return QtCore.QRect(valLoc+self.sliderRadius, 0, self.sliderRadius*2+1, self.height()-1).contains(x, y)

    def isMouseInHigherValue(self, x, y):
        valLoc = (self.width()-2*self.verticalBorderSize) * (self.values[1] - self.minValue) / (self.maxValue - self.minValue)
        return QtCore.QRect(valLoc+self.sliderRadius, 0, self.sliderRadius*2+1, self.height()-1).contains(x, y)
    
    def mousePressEvent(self, event):
        self.moveLower = False
        self.moveHigher = False
        if(self.isMouseInLowerValue(event.x(), event.y())) :
            self.moveLower = True
        elif(self.isMouseInHigherValue(event.x(), event.y())) :
             self.moveHigher = True

    def mouseMoveEvent(self, event):
        if (self.moveLower):
            self.setLowerValue(self.mouseCoordToValue(event.x(), event.y()))
        elif(self.moveHigher):
            self.setHigherValue(self.mouseCoordToValue(event.x(), event.y()))
            
    
    def mouseReleaseEvent(self, event):
        if(self.sliderType == self.HistogramSliderTypeValue):
            self.setLowerValue(self.mouseCoordToValue(event.x(), event.y()))
        self.moveLower = False
        self.moveHigher = False
        
    def resizeEvent(self, event):
        self.minGapBetweenSliders = self.minValue + (self.maxValue-self.minValue) * (float(10) / float(self.width()-2*self.verticalBorderSize))
        HistogramWidget.resizeEvent(self, event)
        
    def emitValueChanged(self, value):
        self.emit(QtCore.SIGNAL("valueChanged(float)"), value)
        
    def emitLowerValueChanged(self, value):
        self.emit(QtCore.SIGNAL("lowerValueChanged(float)"), value)
        
    def emitHigherValueChanged(self, value):
        self.emit(QtCore.SIGNAL("higherValueChanged(float)"), value)
