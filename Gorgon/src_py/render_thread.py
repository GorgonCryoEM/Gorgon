from PyQt4 import QtCore
from gorgon_cpp_wrapper import MarchingCubes

class RenderThread(QtCore.QThread):

    def __init__(self, parent=None):
        QtCore.QThread.__init__(self, parent)

        self.mutex = QtCore.QMutex()
        self.condition = QtCore.QWaitCondition()
        self.engine = MarchingCubes()
        self.surface = 0
        self.sample = 0
        self.surfaceChanged = False
        self.isLoading = False
        self.restart = False
        self.abort = False

    def __del__(self):
        self.mutex.lock()
        self.abort = True
        self.condition.wakeOne()
        self.mutex.unlock()

        self.wait()

    def loadMRC(self, filename):
        locker = QtCore.QMutexLocker(self.mutex)
        self.filename = filename
        self.isLoading = True

        if not self.isRunning():
            self.start(QtCore.QThread.LowPriority)
        else:
            self.restart = True
            self.condition.wakeOne()


    def setSurfaceValue(self, value):
        locker = QtCore.QMutexLocker(self.mutex)

        self.surface = value
        self.surfaceChanged = True
        
        if not self.isRunning():
            self.start(QtCore.QThread.LowPriority)
        else:
            self.restart = True
            self.condition.wakeOne()

    def drawMesh(self):
        locker = QtCore.QMutexLocker(self.mutex)
        self.engine.drawMesh(True)

    def run(self):
        while True:
            self.mutex.lock()
            surface = self.surface
            engine = self.engine
            isLoading = self.isLoading
            self.mutex.unlock()

            if self.isLoading:
                engine.loadMRC(str(self.filename))
                self.isLoading = False
                self.emit(QtCore.SIGNAL("meshCreated()"))
            elif self.surfaceChanged:
                level = 1
                engine.setSampleDensity(level)
                engine.setSurfaceValue(surface)
                level = 5
                engine.setSampleDensity(level)
                self.mutex.lock()
                self.engine = engine
                self.mutex.unlock()
                
                self.emit(QtCore.SIGNAL("meshCreated()"))
                
                while level <= 7 and not self.restart:
                    # refine structure
                    engine.setSampleDensity(level)
                    self.mutex.lock()
                    self.engine = engine
                    self.mutex.unlock()
                    self.emit(QtCore.SIGNAL("meshCreated()"))
                    self.msleep(level*90)
                    level = level + 1
                if not self.restart:
                    self.surfaceChanged = False
            
            self.mutex.lock()
            self.engine = engine
            if not self.restart:
                self.condition.wait(self.mutex)
            self.restart = False
            self.mutex.unlock()
