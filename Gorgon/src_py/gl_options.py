from PyQt4 import QtGui, QtCore

class GLOptions(QtGui.QWidget):
    def __init__(self, main, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.windows = []
        self.createUI()

    def createUI(self):
        self.createActions()
        self.createMenus()

    def createActions(self):
        shadeWireframeAct = QtGui.QAction(self.tr("&Wireframe"), self)
        shadeWireframeAct.setCheckable(True)
        shadeWireframeAct.setStatusTip(self.tr("Wireframe shading"))
        self.connect(shadeWireframeAct, QtCore.SIGNAL("triggered()"), self.shadeWireframe)
        self.app.actions.addAction("set_WireframeShading", shadeWireframeAct)

        shadeFlatAct = QtGui.QAction(self.tr("&Flat"), self)
        shadeFlatAct.setCheckable(True)
        shadeFlatAct.setStatusTip(self.tr("Flat shading"))
        self.connect(shadeFlatAct, QtCore.SIGNAL("triggered()"), self.shadeFlat)
        self.app.actions.addAction("set_FlatShading", shadeFlatAct)        

        shadeSmoothAct = QtGui.QAction(self.tr("&Smooth"), self)
        shadeSmoothAct.setCheckable(True)
        shadeSmoothAct.setStatusTip(self.tr("Smooth shading"))
        self.connect(shadeSmoothAct, QtCore.SIGNAL("triggered()"), self.shadeSmooth)
        self.app.actions.addAction("set_SmoothShading", shadeSmoothAct)    

        self.shadingGroup = QtGui.QActionGroup(self)
        self.shadingGroup.addAction(shadeWireframeAct)
        self.shadingGroup.addAction(shadeFlatAct)
        self.shadingGroup.addAction(shadeSmoothAct)
        shadeSmoothAct.setChecked(True)

        showBoxAct = QtGui.QAction(self.tr("&Show Box"), self)
        showBoxAct.setCheckable(True)
        showBoxAct.setChecked(True)
        showBoxAct.setStatusTip(self.tr("Show bounding box"))
        self.connect(showBoxAct, QtCore.SIGNAL("triggered()"), self.showBox)        
        self.app.actions.addAction("set_ShowBoundingBox", showBoxAct)  

    def createMenus(self):
        visualizationMenu = self.app.menus.addMenu("options-visualization", self.tr("&Visualization"), "options")
        self.app.menus.addAction("options-visualization-shadeWireFrame", self.app.actions.getAction("set_WireframeShading"), "options-visualization")
        self.app.menus.addAction("options-visualization-shadeFlat", self.app.actions.getAction("set_FlatShading"), "options-visualization")
        self.app.menus.addAction("options-visualization-shadeSmooth", self.app.actions.getAction("set_SmoothShading"), "options-visualization")       
        visualizationMenu.addSeparator()
        self.app.menus.addAction("options-visualization-showBoundingBox", self.app.actions.getAction("set_ShowBoundingBox"), "options-visualization")

    def addGLWindow(self, window):
        self.windows.append(window)
        
    def shadeWireframe(self):
        for w in self.windows:
            w.setDisplayType(0)
        
    def shadeFlat(self):
        for w in self.windows:
            w.setDisplayType(1)
        
    def shadeSmooth(self):
        for w in self.windows:
            w.setDisplayType(2)

    def showBox(self):
        for w in self.windows:
            w.setShowBox(self.app.actions.getAction("set_ShowBoundingBox").isChecked())

        
