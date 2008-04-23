# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: A dialog box which can be used to modify the visualization options of a loaded model

from PyQt4 import QtCore, QtGui
from ui_dialog_model_visualization import Ui_DialogModelVisualization

class ModelVisualizationForm(QtGui.QWidget):
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = viewer
        self.title = viewer.title + " Visualization Options"
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelChanged()"), self.modelChanged)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)   
        self.setWindowTitle(self.title)
        
        self.createUI()
        self.createActions()
        self.createMenus()
        self.updateFromViewer()

    def createUI(self):
        self.ui = Ui_DialogModelVisualization()
        self.ui.setupUi(self)    
        self.ui.checkBoxModel2Visible.setVisible(False)
        self.ui.pushButtonModel2Color.setVisible(False)
        self.dock = QtGui.QDockWidget(self.tr(self.title), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()  
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.radioButtonWireframe, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonFlat, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonSmooth, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.checkBoxBoundingBox, QtCore.SIGNAL("toggled (bool)"), self.viewer.setBoundingBox)
        self.connect(self.ui.checkBoxModelVisible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModelVisibility)
        self.connect(self.ui.checkBoxModel2Visible, QtCore.SIGNAL("toggled (bool)"), self.viewer.setModel2Visibility)
        self.connect(self.ui.pushButtonBoundingBoxColor, QtCore.SIGNAL("colorChanged ()"), self.setBoundingBoxColor)        
        self.connect(self.ui.pushButtonModelColor, QtCore.SIGNAL("colorChanged ()"), self.setModelColor)  
        self.connect(self.ui.pushButtonModel2Color, QtCore.SIGNAL("colorChanged ()"), self.setModel2Color)  
        self.connect(self.ui.pushButtonCenter, QtCore.SIGNAL("pressed ()"), self.viewer.emitViewerSetCenter)
        self.connect(self.ui.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.viewer.unloadData)
        self.connect(self.ui.doubleSpinBoxSizeX, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeY, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxSizeZ, QtCore.SIGNAL("editingFinished ()"), self.scaleChanged)
        self.connect(self.ui.doubleSpinBoxLocationX, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationY, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
        self.connect(self.ui.doubleSpinBoxLocationZ, QtCore.SIGNAL("editingFinished ()"), self.locationChanged)
                                                 
        
    def updateFromViewer(self):
        self.ui.pushButtonModelColor.setColor(self.viewer.modelColor)
        self.ui.pushButtonModel2Color.setColor(self.viewer.model2Color)
        self.ui.pushButtonBoundingBoxColor.setColor(self.viewer.boxColor)            
        self.ui.checkBoxBoundingBox.setChecked(self.viewer.showBox)    
        self.ui.checkBoxModelVisible.setChecked(self.viewer.modelVisible)
        self.ui.checkBoxModel2Visible.setChecked(self.viewer.model2Visible)
         
        if(self.viewer.displayStyle == self.viewer.DisplayStyleWireframe):
            self.ui.radioButtonWireframe.setChecked(True) 
        elif(self.viewer.displayStyle == self.viewer.DisplayStyleFlat):
            self.ui.radioButtonFlat.setChecked(True)   
        else :
            self.ui.radioButtonSmooth.setChecked(True)   
            
        self.ui.doubleSpinBoxSizeX.setValue(self.viewer.scale[0])   
        self.ui.doubleSpinBoxSizeY.setValue(self.viewer.scale[1])
        self.ui.doubleSpinBoxSizeZ.setValue(self.viewer.scale[2])
        self.ui.doubleSpinBoxLocationX.setValue(self.viewer.location[0])
        self.ui.doubleSpinBoxLocationY.setValue(self.viewer.location[1])
        self.ui.doubleSpinBoxLocationZ.setValue(self.viewer.location[2])
        self.ui.labelModelSize.setText("{" + 
                                       str(round(self.viewer.renderer.getMax(0) - self.viewer.renderer.getMin(0) ,2)) + ", " +
                                       str(round(self.viewer.renderer.getMax(1) - self.viewer.renderer.getMin(1) ,2)) + ", " +
                                       str(round(self.viewer.renderer.getMax(2) - self.viewer.renderer.getMin(2) ,2)) + "}")
                                    
    def createActions(self):               
        self.visualizerAct = QtGui.QAction(self.tr(self.title), self)
        self.visualizerAct.setStatusTip(self.tr("Load the " + self.viewer.title + " visualization options"))
        self.visualizerAct.setCheckable(True)
        self.visualizerAct.setChecked(True)
        self.visualizerAct.setEnabled(False)
        self.connect(self.visualizerAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("show_" + self.viewer.title + "Visualization", self.visualizerAct)
  
    def createMenus(self):
        self.app.menus.addAction("window-" + self.viewer.title + "VisualizationOptions", self.visualizerAct, "window")                                   
                
    def loadWidget(self):
        if(self.visualizerAct.isChecked()) :
            self.showWidget(True)
        else:
            self.showWidget(False)

    def showWidget(self, show):
        if(show):
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.dock.show()
        else:
             self.app.removeDockWidget(self.dock)         
             
    def modelLoaded(self):
        self.visualizerAct.setChecked(True)
        self.visualizerAct.setEnabled(True)
        self.updateFromViewer()        
        self.showWidget(True)
    
    def modelChanged(self):
        self.updateFromViewer()
    
    def modelUnloaded(self):
        self.visualizerAct.setEnabled(False)
        self.showWidget(False)    

    def setBoundingBoxColor(self):
        self.viewer.setBoundingBoxColor(self.ui.pushButtonBoundingBoxColor.color())
        
    def setModelColor(self):
        self.viewer.setModelColor(self.ui.pushButtonModelColor.color())

    def setModel2Color(self):
        self.viewer.setModel2Color(self.ui.pushButtonModel2Color.color())
    
    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonWireframe.isChecked()) :
            displayStyle = self.viewer.DisplayStyleWireframe      
        elif(self.ui.radioButtonFlat.isChecked()) :
            displayStyle = self.viewer.DisplayStyleFlat    
        elif(self.ui.radioButtonSmooth.isChecked()) :
            displayStyle = self.viewer.DisplayStyleSmooth
        self.viewer.setDisplayStyle(displayStyle)
                    
    def dockVisibilityChanged(self, visible):
        self.visualizerAct.setChecked(visible)
                                 
    def scaleChanged(self):
        self.viewer.setScale(self.ui.doubleSpinBoxSizeX.value(), self.ui.doubleSpinBoxSizeY.value(), self.ui.doubleSpinBoxSizeZ.value())        
    
    def locationChanged(self):
        self.viewer.setLocation(self.ui.doubleSpinBoxLocationX.value(), self.ui.doubleSpinBoxLocationY.value(), self.ui.doubleSpinBoxLocationZ.value())