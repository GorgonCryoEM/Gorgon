# Author:      Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description: A dialog box which can be used to modify the visualization options of the scene

from PyQt4 import QtCore, QtGui
from ui_dialog_scene_editor import Ui_DialogSceneEditor
from color_picker_form import ColorPickerForm

class SceneEditorForm(QtGui.QWidget):
    def __init__(self, main, camera, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.camera = camera             
        self.createUI()
        self.createActions()
        self.createMenus()
        self.updateFromCamera()

    def createUI(self):
        self.ui = Ui_DialogSceneEditor()
        self.ui.setupUi(self)    
        self.colorPicker = ColorPickerForm();
        self.dock = QtGui.QDockWidget(self.tr("Scene Options"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()  
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.camera, QtCore.SIGNAL("cameraChanged ()"), self.updateFromCamera)
        self.connect(self.ui.doubleSpinBoxEyeX, QtCore.SIGNAL("editingFinished ()"), self.eyePositionChanged)
        self.connect(self.ui.doubleSpinBoxEyeY, QtCore.SIGNAL("editingFinished ()"), self.eyePositionChanged)
        self.connect(self.ui.doubleSpinBoxEyeZ, QtCore.SIGNAL("editingFinished ()"), self.eyePositionChanged)
        self.connect(self.ui.doubleSpinBoxCenterX, QtCore.SIGNAL("editingFinished ()"), self.centerPositionChanged)
        self.connect(self.ui.doubleSpinBoxCenterY, QtCore.SIGNAL("editingFinished ()"), self.centerPositionChanged)
        self.connect(self.ui.doubleSpinBoxCenterZ, QtCore.SIGNAL("editingFinished ()"), self.centerPositionChanged)
        self.connect(self.ui.doubleSpinBoxUpX, QtCore.SIGNAL("editingFinished ()"), self.upChanged)
        self.connect(self.ui.doubleSpinBoxUpY, QtCore.SIGNAL("editingFinished ()"), self.upChanged)
        self.connect(self.ui.doubleSpinBoxUpZ, QtCore.SIGNAL("editingFinished ()"), self.upChanged)                                                        
        self.connect(self.ui.doubleSpinBoxZoom, QtCore.SIGNAL("editingFinished ()"), self.zoomChanged)
        self.connect(self.ui.checkBoxLight1Enabled, QtCore.SIGNAL("toggled (bool)"), self.light1Enabled)
        self.connect(self.ui.checkBoxLight2Enabled, QtCore.SIGNAL("toggled (bool)"), self.light2Enabled)
        self.connect(self.ui.pushButtonLight1Color, QtCore.SIGNAL("colorChanged ()"), self.light1ColorChanged)
        self.connect(self.ui.pushButtonLight2Color, QtCore.SIGNAL("colorChanged ()"), self.light2ColorChanged)
        self.connect(self.ui.doubleSpinBoxLight1X, QtCore.SIGNAL("editingFinished ()"), self.light1PositionChanged)
        self.connect(self.ui.doubleSpinBoxLight1Y, QtCore.SIGNAL("editingFinished ()"), self.light1PositionChanged)
        self.connect(self.ui.doubleSpinBoxLight1Z, QtCore.SIGNAL("editingFinished ()"), self.light1PositionChanged)                                                        
        self.connect(self.ui.doubleSpinBoxLight2X, QtCore.SIGNAL("editingFinished ()"), self.light2PositionChanged)
        self.connect(self.ui.doubleSpinBoxLight2Y, QtCore.SIGNAL("editingFinished ()"), self.light2PositionChanged)
        self.connect(self.ui.doubleSpinBoxLight2Z, QtCore.SIGNAL("editingFinished ()"), self.light2PositionChanged)
        self.connect(self.ui.pushButtonBackgroundColor, QtCore.SIGNAL("colorChanged ()"), self.backgroundColorChanged)
        self.connect(self.ui.checkBoxFogEnabled, QtCore.SIGNAL("toggled (bool)"), self.fogEnabled)
        self.connect(self.ui.doubleSpinBoxFogDensity, QtCore.SIGNAL("editingFinished ()"), self.fogDensityChanged)
        self.connect(self.ui.pushButtonFogColor, QtCore.SIGNAL("colorChanged ()"), self.fogColorChanged)        
                  
        
        
    def updateFromCamera(self):
        self.ui.doubleSpinBoxEyeX.setValue(self.camera.eye[0])
        self.ui.doubleSpinBoxEyeY.setValue(self.camera.eye[1])
        self.ui.doubleSpinBoxEyeZ.setValue(self.camera.eye[2])
        self.ui.doubleSpinBoxCenterX.setValue(self.camera.center[0])
        self.ui.doubleSpinBoxCenterY.setValue(self.camera.center[1])
        self.ui.doubleSpinBoxCenterZ.setValue(self.camera.center[2])
        self.ui.doubleSpinBoxUpX.setValue(self.camera.up[0])
        self.ui.doubleSpinBoxUpY.setValue(self.camera.up[1])
        self.ui.doubleSpinBoxUpZ.setValue(self.camera.up[2])
        self.ui.doubleSpinBoxZoom.setValue(self.camera.eyeZoom)
        self.ui.checkBoxLight1Enabled.setChecked(self.camera.lightsEnabled[0])
        self.ui.doubleSpinBoxLight1X.setValue(self.camera.lightsPosition[0][0])
        self.ui.doubleSpinBoxLight1Y.setValue(self.camera.lightsPosition[0][1])
        self.ui.doubleSpinBoxLight1Z.setValue(self.camera.lightsPosition[0][2])
        self.ui.pushButtonLight1Color.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(self.camera.lightsColor[0][0]*255.0, 
                                                                                 self.camera.lightsColor[0][1]*255.0, 
                                                                                 self.camera.lightsColor[0][2]*255.0, 
                                                                                 self.camera.lightsColor[0][3]*255.0)))
        self.ui.checkBoxLight2Enabled.setChecked(self.camera.lightsEnabled[1])
        self.ui.doubleSpinBoxLight2X.setValue(self.camera.lightsPosition[1][0])
        self.ui.doubleSpinBoxLight2Y.setValue(self.camera.lightsPosition[1][1])
        self.ui.doubleSpinBoxLight2Z.setValue(self.camera.lightsPosition[1][2])
        self.ui.pushButtonLight2Color.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(self.camera.lightsColor[1][0]*255.0, 
                                                                                 self.camera.lightsColor[1][1]*255.0, 
                                                                                 self.camera.lightsColor[1][2]*255.0, 
                                                                                 self.camera.lightsColor[1][3]*255.0)))   
        self.ui.pushButtonBackgroundColor.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(self.camera.backgroundColor[0]*255.0, 
                                                                                     self.camera.backgroundColor[1]*255.0, 
                                                                                     self.camera.backgroundColor[2]*255.0, 
                                                                                     self.camera.backgroundColor[3]*255.0)))
        self.ui.checkBoxFogEnabled.setChecked(self.camera.fogEnabled)
        self.ui.doubleSpinBoxFogDensity.setValue(self.camera.fogDensity)
        self.ui.pushButtonFogColor.setColor(QtGui.QColor.fromRgba(QtGui.qRgba(self.camera.fogColor[0]*255.0, 
                                                                              self.camera.fogColor[1]*255.0, 
                                                                              self.camera.fogColor[2]*255.0, 
                                                                              self.camera.fogColor[3]*255.0)))                          
            
    def createActions(self):
        self.visualizerAct = QtGui.QAction(self.tr("Scene Options"), self)
        self.visualizerAct.setStatusTip(self.tr("Load the scene settings"))
        self.visualizerAct.setCheckable(True)
        self.visualizerAct.setChecked(False)
        self.visualizerAct.setEnabled(True)
        self.connect(self.visualizerAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("show_SceneEditor", self.visualizerAct)
  
    def createMenus(self):
        self.app.menus.addAction("window-sceneeditor", self.visualizerAct, "window")                                   
                
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
                    
    def dockVisibilityChanged(self, visible):
        self.visualizerAct.setChecked(visible)

    def eyePositionChanged(self):
        self.camera.setEye(self.ui.doubleSpinBoxEyeX.value(), self.ui.doubleSpinBoxEyeY.value(), self.ui.doubleSpinBoxEyeZ.value())
        self.camera.updateGL()

    def centerPositionChanged(self):
        self.camera.setCenter(self.ui.doubleSpinBoxCenterX.value(), self.ui.doubleSpinBoxCenterY.value(), self.ui.doubleSpinBoxCenterZ.value())
        self.camera.updateGL()
        
    def upChanged(self):
        self.camera.setUp(self.ui.doubleSpinBoxUpX.value(), self.ui.doubleSpinBoxUpY.value(), self.ui.doubleSpinBoxUpZ.value())
        self.camera.updateGL()    
    
    def zoomChanged(self):
        self.camera.setNearFarZoom(self.camera.near, self.camera.far, self.ui.doubleSpinBoxZoom.value())
        self.camera.updateGL()         
    
    def light1Enabled(self, value):
        self.camera.lightsEnabled[0] = value
        self.camera.initializeScene()
        self.camera.updateGL()        

    def light1ColorChanged(self):
        (r, g, b, a) = self.ui.pushButtonLight1Color.color().getRgbF()
        self.camera.lightsColor[0] = [r, g, b, a]
        self.camera.initializeScene()
        self.camera.updateGL()        
        
    def light1PositionChanged(self):
        self.camera.lightsPosition[0] = [self.ui.doubleSpinBoxLight1X.value(), self.ui.doubleSpinBoxLight1Y.value(), self.ui.doubleSpinBoxLight1Z.value()]
        self.camera.initializeScene()
        self.camera.updateGL()               
        
    def light2Enabled(self, value):
        self.camera.lightsEnabled[1] = value
        self.camera.initializeScene()
        self.camera.updateGL()        
        
    def light2ColorChanged(self):
        (r, g, b, a) = self.ui.pushButtonLight2Color.color().getRgbF()
        self.camera.lightsColor[1] = [r, g, b, a]
        self.camera.initializeScene()
        self.camera.updateGL()     
        
    def light2PositionChanged(self):
        self.camera.lightsPosition[1] = [self.ui.doubleSpinBoxLight2X.value(), self.ui.doubleSpinBoxLight2Y.value(), self.ui.doubleSpinBoxLight2Z.value()]
        self.camera.initializeScene()
        self.camera.updateGL()
        
    def backgroundColorChanged(self):
        (r, g, b, a) = self.ui.pushButtonBackgroundColor.color().getRgbF()
        self.camera.backgroundColor = [r, g, b, a]
        self.camera.initializeScene()
        self.camera.updateGL()    
            
    def fogEnabled(self, value):
        self.camera.fogEnabled = value
        self.camera.initializeScene()
        self.camera.updateGL()                
        
    def fogDensityChanged(self):
        self.camera.fogDensity = self.ui.doubleSpinBoxFogDensity.value()
        self.camera.initializeScene()
        self.camera.updateGL()     
                
    def fogColorChanged(self):
        (r, g, b, a) = self.ui.pushButtonFogColor.color().getRgbF()
        self.camera.fogColor = [r, g, b, a]
        self.camera.initializeScene()
        self.camera.updateGL()             