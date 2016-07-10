from PyQt4 import QtGui, QtCore, QtOpenGL
from libpytoolkit import SSERenderer, SSEEngine
from Explorer.base_viewer import BaseViewer
# from sse_sequence_predictor_form import SSESequencePredictorForm
from .helix_correspondence_finder_form import SSEHelixCorrespondenceFinderForm
# from libpyGORGON import SSECorrespondenceEngine, SSECorrespondenceResult
# from .volume_sse_builder_form import VolumeSSEBuilderForm
from Toolkit import SSEHelixCorrespondence

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import termcolor


class SSEViewer(BaseViewer):

    def __init__(self, main, parent=None):
        self.title = "Secondary Structure Element"
        self.app = main
        super(SSEViewer, self).__init__(SSERenderer(), main, parent)
        self.shortTitle = "SSE"
#         self.app.themes.addDefaultRGB("Secondary Structure Element:Model:0", 0, 180, 0, 255)
#         self.app.themes.addDefaultRGB("Secondary Structure Element:Model:1", 120, 185, 255, 255)
#         self.app.themes.addDefaultRGB("Secondary Structure Element:Model:2", 120, 185, 255, 255)
#         self.app.themes.addDefaultRGB("Secondary Structure Element:BoundingBox", 255, 255, 255, 255)
        self.isClosedMesh = False
        self.helixFileName = ""
        self.sheetFileName = ""
        self.currentMatch = None
        self.showBox = False;
        self.helixLoaded = False
        self.sheetLoaded = False
        self.correspondenceEngine = SSEEngine()
        self.selectEnabled = True
        self.app.viewers["sse"] = self
        self.model2Visible = True
        self.model3Visible = False
#         self.initVisualizationOptions(ModelVisualizationForm(self.app, self))
#         self.visualizationOptions.ui.checkBoxModelVisible.setText("Show helices colored:")
#         self.visualizationOptions.ui.checkBoxModel2Visible.setText("Show sheets colored:")
#         self.visualizationOptions.ui.checkBoxModel2Visible.setVisible(True)
#         self.visualizationOptions.ui.pushButtonModel2Color.setVisible(True)
#         self.visualizationOptions.ui.checkBoxModel3Visible.setText("Show skeleton sheets colored:")
#         self.visualizationOptions.ui.checkBoxModel3Visible.setVisible(False)
#         self.visualizationOptions.ui.pushButtonModel3Color.setVisible(False)
        
        # self.connect(self, QtCore.SIGNAL('elementSelected (int, int, int, int, int, int, QMouseEvent)'), self.updateCurrentMatch)
        
#         self.connect(self.app.viewers["volume"], QtCore.SIGNAL('modelLoaded()'), self.updateActionsAndMenus)
#         self.connect(self.app.viewers["volume"], QtCore.SIGNAL('modelUnloaded()'), self.updateActionsAndMenus)
#         self.connect(self, QtCore.SIGNAL('modelLoaded()'), self.updateActionsAndMenus)
#         self.connect(self, QtCore.SIGNAL('modelUnloaded()'), self.updateActionsAndMenus)
        
        self.selectedObjects = []
        self.correspondences = []
        
    def drawGL(self):
#         BaseViewer.drawGL(self)
        try:
            print termcolor.colored('renderer.draw', 'yellow')
            for kk in range(3):
                self.renderer.draw(kk, True)
        except:
            print "Problem in sseViewer::drawGL: renderer.draw"
        else:
            self.extraDrawingRoutines()

    def extraDrawingRoutines(self):
        try:
            print termcolor.colored('correspondenceEngine.draw', 'yellow')
            self.correspondenceEngine.drawAllPaths(0,True,True,True,False)
        except:
            print "Problem in sseViewer::drawGL: correspondenceEngine.draw"

    def loadHelixDataFromFile(self, fileName):
        try:
            self.renderer.loadHelixFile(str(fileName))
            self.loaded = True
            self.helixLoaded = True
            self.modelChanged()
        except:
            QtGui.QMessageBox.critical(self, "Unable to load data file", "The file might be corrupt, or the format may not be supported.", "Ok")

            self.loaded = False
            
    def loadHelixData(self):
        self.helixFileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Helix Annotations"), "", self.tr(self.renderer.getSupportedHelixLoadFileFormats()))
        self.fileName = self.helixFileName;
        if not self.helixFileName.isEmpty():
            self.loadHelixDataFromFile(self.helixFileName)
                    
    def loadSheetDataFromFile(self, fileName):
        self.setCursor(QtCore.Qt.WaitCursor)
        try:
            self.renderer.loadSheetFile(str(fileName))
            self.loaded = True
            self.sheetLoaded = True
            self.modelChanged()
        except:
            QtGui.QMessageBox.critical(self, "Unable to load data file", "The file might be corrupt, or the format may not be supported.", "Ok")
            self.loaded = False
        self.setCursor(QtCore.Qt.ArrowCursor)
        
    def loadSheetData(self):
        self.sheetFileName = QtGui.QFileDialog.getOpenFileName(self, self.tr("Open Sheet Annotations"), "", self.tr(self.renderer.getSupportedSheetLoadFileFormats()))
        self.fileName = self.sheetFileName;
        if not self.sheetFileName.isEmpty():
            self.loadSheetDataFromFile(self.sheetFileName)
            
    def saveHelixData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Helix Annotations"), "", self.tr(self.renderer.getSupportedHelixSaveFileFormats()))
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveHelixFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)

    def saveSheetData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Sheet Annotations"), "", self.tr(self.renderer.getSupportedSheetSaveFileFormats()))
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveSheetFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)

    def unloadData(self):
        self.loaded = False
        self.helixLoaded = False
        self.sheetLoaded = False
        self.helixFileName = ""
        self.sheetFileName = ""
        BaseViewer.unloadData(self)
          
    def makeSheetSurfaces(self, offsetx, offsety, offsetz, scalex, scaley, scalez):
        # rebuild the set of sheets to render
        numHelicesSheets = self.correspondenceEngine.getSkeletonSSECount()
        self.renderer.unloadGraphSSEs()
        for i in range(numHelicesSheets):
            if self.correspondenceEngine.getSkeletonSSE(i).isSheet():
                self.renderer.loadGraphSSE(i, self.correspondenceEngine.getSkeletonSSE(i), offsetx, offsety, offsetz, scalex, scaley, scalez)

    def updateCorrespondences(self, corrs):
        self.correspondences  = corrs
        
    # Overridden
    def emitElementClicked(self, hitStack, event):
#         if (self.app.viewers["calpha"].displayStyle == self.app.viewers["calpha"].DisplayStyleRibbon):
#             if(self.app.mainCamera.mouseRightPressed and hitStack[0] == 0):
#                 self.emit(QtCore.SIGNAL("SSERightClicked(PyQt_PyObject, PyQt_PyObject, QMouseEvent)"), hitStack[0], hitStack[1], event)
#         else:
        BaseViewer.emitElementClicked(self, hitStack, event)
