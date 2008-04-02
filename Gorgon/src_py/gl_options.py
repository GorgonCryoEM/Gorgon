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
        self.shadeWireframeAct = QtGui.QAction(self.tr("&Wireframe"), self)
        self.shadeWireframeAct.setCheckable(True)
        self.shadeWireframeAct.setStatusTip(self.tr("Wireframe shading mode"))
        self.connect(self.shadeWireframeAct, QtCore.SIGNAL("triggered()"), self.shadeWireframe)

        self.shadeFlatAct = QtGui.QAction(self.tr("&Flat"), self)
        self.shadeFlatAct.setCheckable(True)
        self.shadeFlatAct.setStatusTip(self.tr("Flat shading mode"))
        self.connect(self.shadeFlatAct, QtCore.SIGNAL("triggered()"), self.shadeFlat)

        self.shadeSmoothAct = QtGui.QAction(self.tr("&Smooth"), self)
        self.shadeSmoothAct.setCheckable(True)
        self.shadeSmoothAct.setStatusTip(self.tr("Smooth shading mode"))
        self.connect(self.shadeSmoothAct, QtCore.SIGNAL("triggered()"), self.shadeSmooth)

        self.shadingGroup = QtGui.QActionGroup(self)
        self.shadingGroup.addAction(self.shadeWireframeAct)
        self.shadingGroup.addAction(self.shadeFlatAct)
        self.shadingGroup.addAction(self.shadeSmoothAct)
        self.shadeSmoothAct.setChecked(True)

        self.showBoxAct = QtGui.QAction(self.tr("&Show Box"), self)
        self.showBoxAct.setCheckable(True)
        self.showBoxAct.setStatusTip(self.tr("Show bounding box"))
        self.connect(self.showBoxAct, QtCore.SIGNAL("triggered()"), self.showBox)
        self.showBoxAct.setChecked(True)

    def createMenus(self):
        self.glMenu = self.app.menuOptions().addMenu(self.tr("&GL Options"))
        self.glMenu.addAction(self.shadeWireframeAct)
        self.glMenu.addAction(self.shadeFlatAct)
        self.glMenu.addAction(self.shadeSmoothAct)
        self.glMenu.addSeparator()
        self.glMenu.addAction(self.showBoxAct)

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
            w.setShowBox(self.showBoxAct.isChecked())

        
