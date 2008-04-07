from PyQt4 import QtCore, QtGui
from ui_dialog_model_visualization import Ui_DialogModelVisualization
from color_picker_form import ColorPickerForm

class ModelVisualizationForm(QtGui.QWidget):
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.viewer = viewer
        self.title = viewer.title + " Visualization Options"
        self.connect(self.viewer, QtCore.SIGNAL("modelLoaded()"), self.modelLoaded)
        self.connect(self.viewer, QtCore.SIGNAL("modelUnloaded()"), self.modelUnloaded)        
        self.setWindowTitle(self.title)
        self.createUI()
        self.createActions()
        self.createMenus()

    def createUI(self):
        self.ui = Ui_DialogModelVisualization()
        self.ui.setupUi(self)    
        self.colorPicker = ColorPickerForm();
        self.dock = QtGui.QDockWidget(self.tr(self.title), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()        
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.radioButtonWireframe, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonFlat, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.radioButtonSmooth, QtCore.SIGNAL("toggled (bool)"), self.setDisplayStyle)
        self.connect(self.ui.checkBoxBoundingBox, QtCore.SIGNAL("toggled (bool)"), self.setBoundingBox)
        self.connect(self.ui.checkBoxModelVisible, QtCore.SIGNAL("toggled (bool)"), self.setModelVisibility)
        self.connect(self.ui.pushButtonBoundingBoxColor, QtCore.SIGNAL("pressed ()"), self.setBoundingBoxColor)        
        self.connect(self.ui.pushButtonModelColor, QtCore.SIGNAL("pressed ()"), self.setModelColor)
        
        
        
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
        self.showWidget(True)
    
    def modelUnloaded(self):
        self.visualizerAct.setEnabled(False)
        self.showWidget(False)    

    def setBoundingBoxColor(self):
        self.colorPicker.show()
        
        pass;
        
    def setModelColor(self):
        pass;
    
    def setDisplayStyle(self, dummy):
        if(self.ui.radioButtonWireframe.isChecked()) :
            self.viewer.displayStyle = 0       
        elif(self.ui.radioButtonFlat.isChecked()) :
            self.viewer.displayStyle = 1        
        elif(self.ui.radioButtonSmooth.isChecked()) :
            self.viewer.displayStyle = 2
        self.viewer.emitModelVisualizationChanged()
    
    def setBoundingBox(self, visible):
        self.viewer.showBox = visible
        self.viewer.emitModelChanged();
    
    def setModelVisibility(self, visible):
        self.viewer.modelVisible = visible
        self.viewer.emitModelVisualizationChanged()
                    
    def dockVisibilityChanged(self, visible):
        self.visualizerAct.setChecked(visible)
                                 