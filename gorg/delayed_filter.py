# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A filter which transmits events based on a timer.. (simply bundles events into one event) 


from PyQt4 import QtGui, QtCore
import thread

class DelayedFilter(QtGui.QWidget):
    def __init__(self, mainThread, parent=None):
        QtGui.QWidget.__init__(self, parent)
        #self.mainThread = mainThread
        #self.installEventFilter(self)
        #self.event = None
        self.delay = 200
        self.value = 0
        self.timer = QtCore.QTimer(self)
        self.connect(self.timer, QtCore.SIGNAL("timeout()"), self.emitValue)
        self.enabled = True

#    def eventFilter(self,obj,event):
#        if event.type() == QtCore.QEvent.User:
#            cb = event.__dict__.get('callback')
#            if cb: 
#                self._doEvent(event)
#            return False
#        return QtGui.QWidget.eventFilter(self, obj, event)
#
#    def _doEvent(self,event):
#        cb = event.__dict__.get('callback')
#        if not cb: 
#            return
#        data = event.__dict__.get('data')
#        if data or type(data)==type(False): 
#            cb(data)
#        else: 
#            cb()
#        del event
#
#    def event(self, event):
#        if event.type() == QtCore.QEvent.User:
#            self._doEvent(event)
#            return True
#        return QtGui.QWidget.event(self, event)
#
#    def postEventWithCallback(self, callback, data=None):
#        if QtCore.QThread.currentThreadId() == QtGui.QApplication.instance().thread().currentThreadId():
#            if data or type(data)==type(False): 
#                callback(data)
#            else: 
#                callback()
#        else:
#            event = QtCore.QEvent(QtCore.QEvent.User)
#            event.callback = callback
#            if data or type(data)==type(False): 
#                event.data = data
#            QtGui.QApplication.instance().sendEvent(self, event)

    
    def setValue(self, value):
        self.value = value
        if(self.enabled):
            self.timer.stop()
            self.timer.start(self.delay)
        
    def emitValue(self):
        print "emitValue", QtCore.QThread.currentThreadId()
        self.timer.stop()
        #self.postEventWithCallback(self.emitValueChanged)
        #self.moveToThread(self.mainThread)
        self.emitValueChanged();
        
    def emitValueChanged(self):
        print "emitValueChanged", QtCore.QThread.currentThreadId()
        self.emit(QtCore.SIGNAL("valueChanged(float)"), self.value)
