from PyQt4 import QtCore, QtGui


class Form(QtGui.QDialog):

    def __init__(self, parent, ui, dock_title):
        self.parent = parent
        self.ui = ui()
        self.volume = self.parent.volume
        self.skeleton = self.parent.skeleton
        super(Form, self).__init__()
        dock = QtGui.QDockWidget(dock_title, self.volume)
        dock.setWidget(self)
        dock.setAllowedAreas(QtCore.Qt.AllDockWidgetAreas)
        self.parent.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)
        self.connect(self.volume, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.volume, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)
        self.createUI()

    def createUI(self):
        self.ui.setupUi(self)
        self.connect(self.ui.horizontalSliderIsoLevel,QtCore.SIGNAL("valueChanged(int)"),self.isoValueChanged)
        self.connect(self.ui.comboBoxMethod, QtCore.SIGNAL("currentIndexChanged (int)"), self.methodChanged)
        self.methodChanged(0)
        
    def modelLoaded(self):
        maxDensity = self.volume.renderer.getMaxDensity()
        minDensity = self.volume.renderer.getMinDensity()
        self.ui.horizontalSliderIsoLevel.setMinimum(int(minDensity*100))
        self.ui.horizontalSliderIsoLevel.setMaximum(int(maxDensity*100))
        defaultDensity = (int(minDensity*100) + int(maxDensity*100.0)) / 2
        self.ui.horizontalSliderIsoLevel.setValue(defaultDensity)
    
    def modelUnloaded(self):
        self.close()

    def isoValueChanged(self, newLevel):
        self.ui.labelIsoValueDisplay.setNum(newLevel/100.0)

    def getDensityThreshold(self):
        return self.ui.horizontalSliderIsoLevel.value()/100.0
    
    def getSkeletonizationMethod(self):
        return self.ui.comboBoxMethod.currentIndex()
    
    def getMinCurveLength(self):
        return self.ui.spinBoxMinCurve.value()

    def getMinSurfaceSize(self):
        return self.ui.spinBoxMinSurface.value()
          
    def accept(self):
        print "form.... accept"
        print self.volume.loaded
        if(self.volume.loaded):
            self.setCursor(QtCore.Qt.BusyCursor)
            method = self.getSkeletonizationMethod()
            print method
            if(method == 0):
                self.modelLoaded()
                print "       Values:", self.getDensityThreshold(), self.getMinCurveLength(), self.getMinSurfaceSize()
                skeleton = self.volume.renderer.performBinarySkeletonizationJu2007(self.getDensityThreshold(), self.getMinCurveLength(), self.getMinSurfaceSize())
                print "Skeleton after skeletonization: ", skeleton.getSize()
                print "   Origin: ", [skeleton.getOriginX(), skeleton.getOriginY(), skeleton.getOriginZ()]
                self.skeleton.loadVolume(skeleton)
                print "After loadVolume()"
                print "   Origin: ", [self.skeleton.renderer.getOriginX(), self.skeleton.renderer.getOriginY(), self.skeleton.renderer.getOriginZ()]
            self.setCursor(QtCore.Qt.ArrowCursor)
            self.close()
        else:
            QtGui.QMessageBox.critical(None, "Source volume unloaded", "A volume must be loaded to perform skeletonization", QtGui.QMessageBox.Ok, QtGui.QMessageBox.NoButton)
        super(Form, self).accept()

    def loadSelf(self):
        self.ui.horizontalSliderIsoLevel.setValue(int(self.volume.getIsoValue() * 100))
        
    def getCitationHtml(self, title, author, journal):
        return "<b>" + title + "</b><br>" + author + "<br><i>" + journal + "</i>"
                  
    def methodChanged(self, id):
        if(id == 0):
            self.ui.textCitation.setHtml(self.getCitationHtml("Computing a family of skeletons of volumetric models for shape description", "Tao Ju, Matthew Baker and Wah Chiu", "Computer Aided Design 39(5) 2007"))
        else:
            self.ui.textCitation.setHtml("")
