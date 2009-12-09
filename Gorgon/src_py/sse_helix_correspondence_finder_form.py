# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that can be used to find correspondance between helices 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.36.2.26  2009/12/09 04:04:05  schuhs
#   Add support for rendering a third model
#
#   Revision 1.36.2.25  2009/12/09 01:42:35  schuhs
#   When graph is rebuilt, load new graph sheets into SSE viewer class
#
#   Revision 1.36.2.24  2009/12/03 22:53:52  schuhs
#   Read constraints from c++ into python and then erase from c++. For now direction of helix constraint is ignored, so constraints are weaker in Gorgon than in the console app.
#
#   Revision 1.36.2.23  2009/12/03 15:39:25  schuhs
#   More fixes to make sheet constraints work in GUI
#
#   Revision 1.36.2.22  2009/12/03 03:10:07  schuhs
#   Reading constraints from Gorgon
#
#   Revision 1.36.2.21  2009/12/02 21:31:36  schuhs
#   Constraints set in Gorgon are correctly sent to the correspondence engine. Still need to allow constraints to be loaded from a file and preserve constraints when the graph structure is changed.
#
#   Revision 1.36.2.20  2009/12/02 20:27:39  schuhs
#   Comment out unnecessary console messages
#
#   Revision 1.36.2.19  2009/12/02 19:28:28  schuhs
#   Fixed the GUI code for setting and changing helix constraints
#
#   Revision 1.36.2.18  2009/12/01 23:44:31  schuhs
#   Some progress selecting helices and sheets for constraints.
#
#   Revision 1.36.2.17  2009/11/29 22:18:36  schuhs
#   "Include sheets" checkbox works now.
#
#   Revision 1.36.2.16  2009/11/29 20:55:09  schuhs
#   Adding code to handle "include sheets" checkbox. Still not done.
#
#   Revision 1.36.2.15  2009/11/29 19:03:13  schuhs
#   Added code to turn helix and sheet and skeleton rendering on and off
#
#   Revision 1.36.2.14  2009/11/25 21:49:44  schuhs
#   Fixing some bugs that cause crashes when setting up a correspondence
#
#   Revision 1.36.2.13  2009/11/05 17:26:47  schuhs
#   Comment out console messages used for debugging
#
#   Revision 1.36.2.12  2009/10/08 21:47:34  schuhs
#   no longer setting colors of structures when rendering paths
#
#   Revision 1.36.2.11  2009/09/25 15:26:56  schuhs
#   Comment out code that clears all constraints. This preserves the constraints loaded from the settings file.
#
#   Revision 1.36.2.10  2009/08/18 17:57:27  schuhs
#   Adding ability to ignore strands when building the sequence graph
#
#   Revision 1.36.2.9  2009/07/23 15:07:05  schuhs
#   Fixing typo that caused a crash when rendering paths
#
#   Revision 1.36.2.8  2009/07/21 14:58:57  schuhs
#   Storing helix and sheet colors in the CorrespondenceEngine and the SSERenderer classes
#
#   Revision 1.36.2.7  2009/07/17 21:06:27  schuhs
#   Starting to assign different colors to sheets and helices
#
#   Revision 1.36.2.6  2009/07/14 19:58:30  schuhs
#   Adding support for correspondences that contain sheet-strand matches
#
#   Revision 1.36.2.5  2009/06/18 20:33:53  schuhs
#   Removing linefeeds from print statements
#
#   Revision 1.36.2.4  2009/06/09 16:44:14  schuhs
#   Adding UI boxes for sheet parameters, adding UI button to load all settings from text file, and modifying skeleton parsing code to work with sheets.
#
#   Revision 1.36.2.3  2009/05/28 17:00:27  schuhs
#   Sheet skeleton parameters can be set from the UI now
#
#   Revision 1.36.2.2  2009/05/22 19:22:15  schuhs
#   Adding new tab to SSE correspondence form to set and change graph parameters and control graph visualization
#
#   Revision 1.36.2.1  2009/05/13 20:56:00  schuhs
#   Adding checkbox to draw all possible paths along skeleton, and adding input for sheet filename
#
#   Revision 1.36  2009/04/04 22:10:15  ssa1
#   Hiding selected path when correspondance finder is not in focus
#
#   Revision 1.35  2009/03/31 21:40:13  ssa1
#   Refactoring: Splitting seq_model\SequenceView.py into subclasses
#
#   Revision 1.34  2009/01/01 22:31:59  colemanr
#   added a few comments
#
#   Revision 1.33  2008/12/02 21:11:32  colemanr
#   Set Alt+h to launch this form.
#
#   Revision 1.32  2008/12/02 03:27:05  ssa1
#   putting glpushattrib and glpopattrib when setting colors
#
#   Revision 1.31  2008/12/01 23:42:55  ssa1
#   Setting theming support for backbone trace
#
#   Revision 1.30  2008/11/25 22:26:04  ssa1
#   User constraints on finding correspondences (v4 - Final)
#
#   Revision 1.29  2008/11/25 21:03:40  ssa1
#   User constraints on finding correspondences (v3)
#
#   Revision 1.28  2008/11/25 17:36:12  ssa1
#   Fixing needing 2 clicks to get RMB bug
#
#   Revision 1.27  2008/11/25 16:16:47  ssa1
#   Fixing constraining missing helices bug
#
#   Revision 1.26  2008/11/25 03:44:36  ssa1
#   User constraints on finding correspondences (v2)
#
#   Revision 1.25  2008/11/25 03:36:08  ssa1
#   User constraints on finding correspondences (v2)
#
#   Revision 1.24  2008/11/25 00:05:39  colemanr
#   updates the index of the currently selected correspondence in the CorrespondenceLibrary
#
#   Revision 1.23  2008/11/24 21:11:49  colemanr
#   changes to accept method so that the ObservedHelix objects have C-alpha coordinates
#
#   Revision 1.22  2008/11/24 20:49:52  ssa1
#   Updating information on the correspondence finder
#
#   Revision 1.21  2008/11/24 20:02:49  ssa1
#   User constraints on finding correspondences (v1)
#
#   Revision 1.20  2008/11/23 19:56:07  ssa1
#   Setting volume origin to be center of mass instead of bottom left...
#
#   Revision 1.19  2008/11/20 21:05:16  ssa1
#   Removing depricated items
#
#   Revision 1.18  2008/11/20 20:56:30  ssa1
#   Properly scaling the suggested backbone
#
#   Revision 1.17  2008/11/17 19:43:08  colemanr
#   In self.accept, moved the logic for creating a StructurePrediciton object
#   to StructurePrediction.load()
#
#   Revision 1.16  2008/11/14 22:42:45  colemanr
#   temporary hack to set the chain object in the structure prediction; set
#   CAlphaViewer.structPred to the structure prediction, so CAlphaSequenceView.py
#   will get the correct object
#
#   Revision 1.15  2008/11/13 00:30:32  colemanr
#   changes concerning *.seq files
#
#   Revision 1.14  2008/11/11 15:38:55  colemanr
#   fixed indentation problems
#
#   Revision 1.13  2008/11/10 21:12:54  colemanr
#   allows either SEQ or PDB files to define the sequence
#
#   Revision 1.12  2008/08/27 15:26:52  marshm
#   Updates to CAlphaSequenceView.  Updated coloring scheme for correspondence matches.
#
#   Revision 1.11  2008/08/06 06:21:37  ssa1
#   Tracing protein path, for SSE Correspondance matching
#
#   Revision 1.10  2008/07/29 20:40:43  ssa1
#   Re-formatting graph matching results
#
#   Revision 1.9  2008/07/29 16:57:18  ssa1
#   Visualizing the correspondence and fixing bug in the CorrespondenceLibrary
#
#   Revision 1.8  2008/07/29 15:50:31  ssa1
#   Fixing import errors
#
#   Revision 1.7  2008/07/28 16:28:16  ssa1
#   Adding in correspondance data repository
#
#   Revision 1.6  2008/07/28 16:19:22  ssa1
#   Adding in correspondance data repository
#
#   Revision 1.5  2008/06/18 18:15:41  ssa1
#   Adding in CVS meta data
#

from PyQt4 import QtCore, QtGui
from ui_dialog_sse_helix_correspondence_finder_new import Ui_DialogSSEHelixCorrespondenceFinder
#from ui_dialog_sse_helix_correspondence_finder import Ui_DialogSSEHelixCorrespondenceFinder
from libpyGORGON import SSECorrespondenceEngine, SSECorrespondenceResult
from correspondence.CorrespondenceLibrary import CorrespondenceLibrary
from correspondence.Correspondence import Correspondence
from correspondence.Match import Match
from correspondence.ObservedHelix import ObservedHelix
from correspondence.ObservedSheet import ObservedSheet
from correspondence.StructureObservation import StructureObservation
from correspondence.StructurePrediction import StructurePrediction
from seq_model.Helix import Helix
from vector_lib import *

from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

import math

class SSEHelixCorrespondenceFinderForm(QtGui.QWidget):   
    def __init__(self, main, viewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.executed = False
        self.app = main
        self.app.themes.addDefaultRGB("CorrespondenceFinder:BackboneTrace", 255, 255, 255, 255)
        self.viewer = viewer        
        self.createUI()
        self.createActions()
        self.createMenus()
        self.loadingCorrespondance = False
        self.userConstraints = {}
        self.constraintActions = {}
        self.selectedRow = 0
        self.dataLoaded = False

    def createUI(self):
        self.ui = Ui_DialogSSEHelixCorrespondenceFinder()
        self.ui.setupUi(self)       
        self.dock = QtGui.QDockWidget(self.tr("SSE - Helix Correspondence Finder"), self.app)
        self.dock.setAllowedAreas(QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea)
        self.dock.setWidget(self)
        self.dock.close()        
        self.connect(self.dock, QtCore.SIGNAL("visibilityChanged (bool)"), self.dockVisibilityChanged)
        self.connect(self.ui.pushButtonGetHelixLengthFile, QtCore.SIGNAL("pressed ()"), self.getHelixLengthFile)
        self.connect(self.ui.pushButtonGetHelixLocationFile, QtCore.SIGNAL("pressed ()"), self.getHelixLocationFile)
        self.connect(self.ui.pushButtonGetSheetLocationFile, QtCore.SIGNAL("pressed ()"), self.getSheetLocationFile)
        self.connect(self.ui.pushButtonGetSkeletonFile, QtCore.SIGNAL("pressed ()"), self.getSkeletonFile)
        self.connect(self.ui.pushButtonGetSequenceFile, QtCore.SIGNAL("pressed ()"), self.getSequenceFile)
        self.connect(self.ui.pushButtonGetSettingsFile, QtCore.SIGNAL("pressed ()"), self.getSettingsFile)
        self.connect(self.ui.pushButtonReset, QtCore.SIGNAL("pressed ()"), self.loadDefaults)
        self.connect(self.ui.pushButtonCancel, QtCore.SIGNAL("pressed ()"), self.reject)
        self.connect(self.ui.pushButtonOk, QtCore.SIGNAL("pressed ()"), self.accept)
        self.connect(self.ui.pushButtonRebuildGraph, QtCore.SIGNAL("pressed ()"), self.rebuildGraph)
        self.connect(self.ui.comboBoxCorrespondences, QtCore.SIGNAL("currentIndexChanged (int)"), self.selectCorrespondence)
        # adding new checkbox
        self.connect(self.ui.checkBoxShowAllPaths, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
        self.connect(self.ui.checkBoxShowSheetCorners, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
        self.connect(self.ui.checkBoxShowHelixCorners, QtCore.SIGNAL("toggled (bool)"), self.fullGraphVisibilityChanged)
        self.connect(self.ui.checkBoxShowSheetColors, QtCore.SIGNAL("toggled (bool)"), self.graphSheetVisibilityChanged)
        self.connect(self.ui.checkBoxShowSkeleton, QtCore.SIGNAL("toggled (bool)"), self.skeletonVisibilityChanged)
        self.connect(self.ui.checkBoxShowSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetVisibilityChanged)
        self.connect(self.ui.checkBoxShowHelices, QtCore.SIGNAL("toggled (bool)"), self.helixVisibilityChanged)
        self.connect(self.ui.checkBoxIncludeSheets, QtCore.SIGNAL("toggled (bool)"), self.sheetIncludeChanged)
        self.connect(self.ui.checkBoxMissingSheets, QtCore.SIGNAL("toggled (bool)"), self.missingSheetChanged)
        self.connect(self.ui.checkBoxMissingHelices, QtCore.SIGNAL("toggled (bool)"), self.missingHelixChanged)
        self.connect(self.app.viewers["skeleton"], QtCore.SIGNAL("modelDrawing()"), self.drawOverlay)
        self.ui.tableWidgetCorrespondenceList.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.connect(self.ui.tableWidgetCorrespondenceList, QtCore.SIGNAL("customContextMenuRequested (const QPoint&)"), self.customMenuRequested)
        self.connect(self.viewer, QtCore.SIGNAL("elementClicked (int, int, int, int, int, int, QMouseEvent)"), self.sseClicked)
          
    # populate parameter boxes with default values for correspondence search        
    def loadDefaults(self):
        self.ui.lineEditHelixLengthFile.setText("")
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.ui.lineEditSheetLocationFile.setText(self.viewer.sheetFileName)
        self.ui.lineEditSkeletonFile.setText(self.app.viewers["skeleton"].fileName)
        self.ui.lineEditSequenceFile.setText(self.app.viewers["calpha"].fileName)        
        self.ui.lineEditSettingsFile.setText("")
        self.loadDefaultParams()

    def loadDefaultParams(self):
        # Graph Settings tab
        self.ui.spinBoxBorderMarginThreshold.setValue(5)
        self.ui.doubleSpinBoxEuclideanDistance.setValue(0.0)
        self.ui.checkBoxIncludeSheets.setChecked(True)
        #min sheet size
        #max sheet distance
        #sheet self-loop length

        self.ui.checkBoxShowSkeleton.setChecked(self.app.viewers['skeleton'].visualizationOptions.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelices.setChecked(self.app.viewers['sse'].visualizationOptions.ui.checkBoxModelVisible.isChecked())
        self.ui.checkBoxShowHelixCorners.setChecked(False)
        self.ui.checkBoxShowSheets.setChecked(self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel2Visible.isChecked())
        self.ui.checkBoxShowSheetColors.setChecked(self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel3Visible.isChecked())
        self.ui.checkBoxShowSheetCorners.setChecked(False)
        self.ui.checkBoxShowAllPaths.setChecked(False)


        # Matching Settings tab
        self.ui.doubleSpinBoxEuclideanToPDBRatio.setValue(10.0)
        self.ui.radioButtonAbsoluteDifference.setChecked(True)
        self.ui.radioButtonNormalizedDifference.setChecked(False)
        self.ui.radioButtonQuadraticError.setChecked(False)

        self.ui.doubleSpinBoxLoopImportance.setValue(0.2)
        self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.setValue(5.0)
        
        self.ui.doubleSpinBoxHelixImportance.setValue(1.0)
        self.ui.checkBoxMissingHelices.setChecked(False)
        self.ui.spinBoxMissingHelixCount.setValue(0)
        self.ui.doubleSpinBoxHelixMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxHelixMissingPenaltyScaled.setValue(0.0)
        self.ui.doubleSpinBoxEndHelixMissingPenalty.setValue(5.0)

        self.ui.doubleSpinBoxSheetImportance.setValue(1.0)
        self.ui.checkBoxMissingSheets.setChecked(False)
        self.ui.spinBoxMissingSheetCount.setValue(0)
        self.ui.doubleSpinBoxSheetMissingPenalty.setValue(5.0)
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setValue(0.0)

        # Results tab
        self.ui.tableWidgetCorrespondenceList.clearContents()
        self.ui.tabWidget.setCurrentIndex(0)         
        self.ui.comboBoxCorrespondences.setCurrentIndex(-1)

        self.checkOk()
        
    def openFile(self, title, fileformats):
        fileName = QtGui.QFileDialog.getOpenFileName(self, self.tr(title), "", self.tr(fileformats))
        return fileName
    
    def getHelixLengthFile(self):
        self.ui.lineEditHelixLengthFile.setText(self.openFile("Load Helix Length File", "SSE Hunter results (*.sse)"))
        self.checkOk()

    def getHelixLocationFile(self):
        self.viewer.loadHelixData()
        self.ui.lineEditHelixLocationFile.setText(self.viewer.helixFileName)
        self.checkOk()               

    def getSheetLocationFile(self):
        self.viewer.loadSheetData()
        self.ui.lineEditSheetLocationFile.setText(self.viewer.sheetFileName)
        self.checkOk()               

    def getSkeletonFile(self):
        self.app.viewers["skeleton"].loadData()
        self.ui.lineEditSkeletonFile.setText(self.app.viewers["skeleton"].fileName)
        self.checkOk()
        
    def getSequenceFile(self):
        """
        This loads a SEQ file or, for testing purposes, a PDB file.
        """
        self.ui.lineEditSequenceFile.setText(self.openFile("Load Sequence File", "Sequence with SSE predictions (*.seq)\nPDB Helix Annotations (*.pdb)"))
        self.checkOk()
    
    def getSettingsFile(self):
        """
        This loads a settings file, which contains filenames and search parameters.
        """
        self.ui.lineEditSettingsFile.setText(self.openFile("Load Settings File", "Settings File (*.txt)"))
        self.loadSettings()
    
    def checkOk(self):
        """
        This checks if all files necessary for the correspondence search have been loaded. If so, the
        correspondence search parameter tabs are enabled and a basic correspondence is created.   
        """
        print "begin checkOk"
        print "correspondence index at beginning is "
        print self.ui.comboBoxCorrespondences.currentIndex()

        self.dataLoaded = not(self.ui.lineEditHelixLengthFile.text().isEmpty() or self.ui.lineEditHelixLocationFile.text().isEmpty()
                           or self.ui.lineEditSheetLocationFile.text().isEmpty()
                           or self.ui.lineEditSkeletonFile.text().isEmpty() or self.ui.lineEditSequenceFile.text().isEmpty())
        self.ui.pushButtonOk.setEnabled(self.dataLoaded)
        
        self.ui.tabWidget.setTabEnabled(1, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(2, self.dataLoaded)
        self.ui.tabWidget.setTabEnabled(3, self.dataLoaded)
        if(self.dataLoaded):
            self.createBasicCorrespondence()
            print "after creating basic correspondence (1), secelDict has length " + str(len(self.viewer.correspondenceLibrary.structurePrediction.secelDict))   
            self.createBasicCorrespondence()
            print "after creating basic correspondence (2), secelDict has length " + str(len(self.viewer.correspondenceLibrary.structurePrediction.secelDict))   
            self.viewer.correspondenceLibrary.correspondenceList = self.populateEmptyResults(self.viewer.correspondenceLibrary)
            print "correspondenceList has length " + str(len(self.viewer.correspondenceLibrary.correspondenceList))
            self.populateComboBox(self.viewer.correspondenceLibrary)
        else:
            print "data not loaded"                        
        print "correspondence index at end is " + str(self.ui.comboBoxCorrespondences.currentIndex())
        print "end checkOk"
    
    def loadWidget(self):
        if(self.app.actions.getAction("perform_SSEFindHelixCorrespondences").isChecked()) :
            self.app.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.dock)
            self.loadDefaults()
            self.dock.show()
        else:
            self.app.removeDockWidget(self.dock)
            
    def dockVisibilityChanged(self, visible):
        self.app.actions.getAction("perform_SSEFindHelixCorrespondences").setChecked(visible)
        self.app.viewers['skeleton'].emitModelChanged()

    def fullGraphVisibilityChanged(self, visible):
        """Called when the visibility checkbox is checked."""
        # to render again
        self.viewer.emitModelChanged()
    
    def skeletonVisibilityChanged(self, visible):
        """Called when the show skeleton checkbox is checked."""
        self.app.viewers['skeleton'].visualizationOptions.ui.checkBoxModelVisible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()
            
    def sheetVisibilityChanged(self, visible):
        """Called when the show sheet checkbox is checked."""
        #self.visualizationOptions.ui.checkBoxModel2Visible.setChecked(visible)
        self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel2Visible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()

    def graphSheetVisibilityChanged(self, visible):
        """Called when the show graph sheet checkbox is checked."""
        self.app.viewers['sse'].visualizationOptions.ui.checkBoxModel3Visible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()

    def helixVisibilityChanged(self, visible):
        """Called when the show helix checkbox is checked."""
        #self.visualizationOptions.ui.checkBoxModel2Visible.setChecked(visible)
        self.app.viewers['sse'].visualizationOptions.ui.checkBoxModelVisible.setChecked(visible)
        # to render again
        self.viewer.emitModelChanged()
            
    def sheetIncludeChanged(self, include):
        """Called when the include sheets checkbox is checked."""
        #if (include == True):
        #    self.viewer.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 1)
        #else:
        #    self.viewer.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 0)
        self.ui.spinBoxMinSheetSize.setEnabled(include)
        self.ui.doubleSpinBoxMaxSheetDistance.setEnabled(include)
        self.ui.doubleSpinBoxSheetSelfLoopLength.setEnabled(include)

    def missingHelixChanged(self, checked):
        """Called when the missing helices checkbox is checked."""
        self.ui.spinBoxMissingHelixCount.setEnabled(checked)

    def missingSheetChanged(self, checked):
        """Called when the missing helices checkbox is checked."""
        self.ui.spinBoxMissingSheetCount.setEnabled(checked)

            

    def modelChanged(self):
        if(not self.viewer.loaded) and self.app.actions.getAction("perform_SSEFindHelixCorrespondences").isChecked():
            self.app.actions.getAction("perform_SSEFindHelixCorrespondences").trigger()        
        
    def createActions(self):               
        corrAct = QtGui.QAction(self.tr("Find Alpha-&Helix Correspondences"), self)
        corrAct.setStatusTip(self.tr("Find Alpha-Helix Correspondences"))
        corrAct.setCheckable(True)
        corrAct.setChecked(False)
        self.corrAct = corrAct
        self.connect(corrAct, QtCore.SIGNAL("triggered()"), self.loadWidget)
        self.app.actions.addAction("perform_SSEFindHelixCorrespondences", corrAct)
  
    def createMenus(self):
        self.app.menus.addAction("actions-sse-findhelixcorrespondences", self.app.actions.getAction("perform_SSEFindHelixCorrespondences"), "actions-sse")        
    
    def loadSettings(self):
        
        if (not(self.ui.lineEditSettingsFile.text().isEmpty())):

            print "loading default parameters"
            self.loadDefaultParams()
        
            print "loading settings file"
            self.setCursor(QtCore.Qt.BusyCursor)
                   
            print "calling setConstantsFromFile" 
            self.viewer.correspondenceEngine.setConstantsFromFile(str(self.ui.lineEditSettingsFile.text()))
    
            print "copying constants from file to GUI" 
            self.getConstants()
    
            print "getting settings filename"
            settingsFilename = str(self.ui.lineEditSettingsFile.text())
            
            #self.viewer.correspondenceEngine.printConstants
            print "reading other filenames for parsing"
            helixFilename = self.viewer.correspondenceEngine.getConstantString("VRML_HELIX_FILE_NAME")
            print "helix filename is " + helixFilename
            sheetFilename = self.viewer.correspondenceEngine.getConstantString("VRML_SHEET_FILE_NAME")
            print "sheet filename is " + sheetFilename
            sequenceFilename = self.viewer.correspondenceEngine.getConstantString("SEQUENCE_FILE_NAME")
            print "sequence filename is " + sequenceFilename
            skeletonFilename = self.viewer.correspondenceEngine.getConstantString("MRC_FILE_NAME")
            print "skeleton filename is " + skeletonFilename
            sseFilename = self.viewer.correspondenceEngine.getConstantString("SSE_FILE_NAME")
            print "sse filename is " + sseFilename
            
            import os
            path,settingsFile = os.path.split(settingsFilename)
            ssePath,sseFile = os.path.split(sseFilename)
            helixPath,helixFile = os.path.split(helixFilename)
            sheetPath,sheetFile = os.path.split(sheetFilename)
            skeletonPath,skeletonFile = os.path.split(skeletonFilename)
            seqPath,seqFile = os.path.split(sequenceFilename)
    
            print "The path (raw) is " + path
            path = path.replace('/', os.sep)  # replace forward slash with separator for this OS
            path = path.replace('\\', os.sep) # replace backslash with separator for this OS
            print "The path (fixed) is " + path
    
            settingsFilePath = path + os.sep + settingsFile
            sseFilePath = path + os.sep + sseFile
            helixFilePath = path + os.sep + helixFile
            sheetFilePath = path + os.sep + sheetFile
            skeletonFilePath = path + os.sep + skeletonFile
            seqFilePath = path + os.sep + seqFile
    
            print "The settings file is " + settingsFilePath
            print "The sse file is " + sseFilePath
            print "The helix file is " + helixFilePath
            print "The sheet file is " + sheetFilePath
            print "The skeleton file is " + skeletonFilePath
            print "The seq file is " + seqFilePath
            
            # store sequence filename
            self.ui.lineEditSequenceFile.setText(seqFilePath)
            
            # load helix file and store the filename
            self.viewer.loadHelixDataFile(helixFilePath)
            self.ui.lineEditHelixLocationFile.setText(helixFilePath)
    
            # load sheet file and store the filename
            self.viewer.loadSheetDataFile(sheetFilePath)
            self.ui.lineEditSheetLocationFile.setText(sheetFilePath)
    
            # load skeleton file and store the filename
            self.app.viewers["skeleton"].loadDataFile(skeletonFilePath)
            self.ui.lineEditSkeletonFile.setText(skeletonFilePath)
    
            # store helix length filename
            self.ui.lineEditHelixLengthFile.setText(sseFilePath)
    
            self.setCursor(QtCore.Qt.ArrowCursor)
            
            self.checkOk()
    
            self.getConstraints()
            print "done loading settings file"
        
    def setConstants(self):
        #Data Sources tab
        self.viewer.correspondenceEngine.setConstant("SSE_FILE_NAME", str(self.ui.lineEditHelixLengthFile.text()))
        self.viewer.correspondenceEngine.setConstant("VRML_HELIX_FILE_NAME", str(self.ui.lineEditHelixLocationFile.text()))
        self.viewer.correspondenceEngine.setConstant("VRML_SHEET_FILE_NAME", str(self.ui.lineEditSheetLocationFile.text()))
        self.viewer.correspondenceEngine.setConstant("MRC_FILE_NAME", str(self.ui.lineEditSkeletonFile.text()))
        self.sequenceFileName = str(self.ui.lineEditSequenceFile.text())
        self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_NAME", self.sequenceFileName)
        if self.sequenceFileName.split('.')[-1].lower() == 'pdb':
            self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "PDB")
        elif self.sequenceFileName.split('.')[-1].lower() == 'seq':
            self.viewer.correspondenceEngine.setConstant("SEQUENCE_FILE_TYPE", "SEQ")
        
        #Graph Settings tab
        self.viewer.correspondenceEngine.setConstantInt("BORDER_MARGIN_THRESHOLD", self.ui.spinBoxBorderMarginThreshold.value())
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_DISTANCE_THRESHOLD", self.ui.doubleSpinBoxEuclideanDistance.value())
        if (self.ui.checkBoxIncludeSheets.isChecked() == False):
            self.viewer.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 0)
            self.viewer.correspondenceEngine.setConstant("MAXIMUM_DISTANCE_SHEET_SKELETON", 0.0)
        else:
            self.viewer.correspondenceEngine.setConstantInt("INCLUDE_STRANDS", 1)
            self.viewer.correspondenceEngine.setConstant("MAXIMUM_DISTANCE_SHEET_SKELETON", self.ui.doubleSpinBoxMaxSheetDistance.value())
            self.viewer.correspondenceEngine.setConstantInt("MINIMUM_SHEET_SIZE", self.ui.spinBoxMinSheetSize.value())
            self.viewer.correspondenceEngine.setConstant("SHEET_SELF_LOOP_LENGTH", self.ui.doubleSpinBoxSheetSelfLoopLength.value())


        #Matching Settings tab
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_VOXEL_TO_PDB_RATIO", self.ui.doubleSpinBoxEuclideanToPDBRatio.value())
        if(self.ui.radioButtonAbsoluteDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 1)
        elif (self.ui.radioButtonNormalizedDifference.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 2)
        else :
            self.viewer.correspondenceEngine.setConstantInt("COST_FUNCTION", 3)

        self.viewer.correspondenceEngine.setConstant("LOOP_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxLoopImportance.value())
        self.viewer.correspondenceEngine.setConstant("EUCLIDEAN_LOOP_PENALTY", self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.value())

        self.viewer.correspondenceEngine.setConstant("HELIX_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxHelixImportance.value())
        if(self.ui.checkBoxMissingHelices.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", self.ui.spinBoxMissingHelixCount.value())
        else:
            self.viewer.correspondenceEngine.setConstantInt("MISSING_HELIX_COUNT", -1)            
        self.viewer.correspondenceEngine.setConstant("MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxHelixMissingPenalty.value())
        self.viewer.correspondenceEngine.setConstant("MISSING_HELIX_PENALTY_SCALED", self.ui.doubleSpinBoxHelixMissingPenaltyScaled.value())
        self.viewer.correspondenceEngine.setConstant("START_END_MISSING_HELIX_PENALTY", self.ui.doubleSpinBoxEndHelixMissingPenalty.value())
        
        self.viewer.correspondenceEngine.setConstant("SHEET_WEIGHT_COEFFICIENT", self.ui.doubleSpinBoxSheetImportance.value())
        if(self.ui.checkBoxMissingSheets.isChecked()):
            self.viewer.correspondenceEngine.setConstantInt("MISSING_SHEET_COUNT", self.ui.spinBoxMissingSheetCount.value())
        else:
            self.viewer.correspondenceEngine.setConstantInt("MISSING_SHEET_COUNT", -1)            
        self.viewer.correspondenceEngine.setConstant("MISSING_SHEET_PENALTY", self.ui.doubleSpinBoxSheetMissingPenalty.value())
        self.viewer.correspondenceEngine.setConstant("MISSING_SHEET_PENALTY_SCALED", self.ui.doubleSpinBoxSheetMissingPenaltyScaled.value())
        
        # no longer needed?
        self.viewer.correspondenceEngine.setConstantBool("NORMALIZE_GRAPHS", True)        

        
    
    
        #Tab 4 User Constraints
        # comment out the constraint clearing so that constraints can be loaded from settings files
        #self.viewer.correspondenceEngine.clearAllConstraints()
        correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
        if(correspondenceIndex >= 0):
            corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
            predictedGraphNode = 1            
            nObservedHelices = len(self.viewer.correspondenceLibrary.structureObservation.helixDict)
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]                
                self.userConstraints[i] = match.constrained
                if(match.constrained):
                    if match.predicted.type == 'helix':
                        if(match.observed):
                            self.viewer.correspondenceEngine.setHelixConstraint(predictedGraphNode, 2*match.observed.label + 1)
                        else:      
                            self.viewer.correspondenceEngine.setHelixConstraint(predictedGraphNode, -1)
                    if match.predicted.type == 'strand':
                        if(match.observed):
                            self.viewer.correspondenceEngine.setNodeConstraint(predictedGraphNode, match.observed.label + nObservedHelices + 1)
                        else:      
                            self.viewer.correspondenceEngine.setNodeConstraint(predictedGraphNode, -1)
                if (match.predicted.type) == 'strand':
                    predictedGraphNode += 1
                if (match.predicted.type) == 'helix':
                    predictedGraphNode += 2

    def getConstants(self):
        
        #Graph Settings tab
        self.ui.spinBoxBorderMarginThreshold.setValue(self.viewer.correspondenceEngine.getConstantInt("BORDER_MARGIN_THRESHOLD"))
        self.ui.doubleSpinBoxEuclideanDistance.setValue(self.viewer.correspondenceEngine.getConstantDouble("EUCLIDEAN_DISTANCE_THRESHOLD"))
        if(self.viewer.correspondenceEngine.getConstantInt("INCLUDE_STRANDS") == 1):
            self.ui.checkBoxIncludeSheets.setChecked(True)
            self.ui.spinBoxMinSheetSize.setEnabled(True)
            self.ui.doubleSpinBoxMaxSheetDistance.setEnabled(True)
            self.ui.doubleSpinBoxSheetSelfLoopLength.setEnabled(True)
        else:
            self.ui.checkBoxIncludeSheets.setChecked(False)
            self.ui.spinBoxMinSheetSize.setEnabled(False)
            self.ui.doubleSpinBoxMaxSheetDistance.setEnabled(False)
            self.ui.doubleSpinBoxSheetSelfLoopLength.setEnabled(False)
        self.ui.spinBoxMinSheetSize.setValue(self.viewer.correspondenceEngine.getConstantInt("MINIMUM_SHEET_SIZE"))
        self.ui.doubleSpinBoxMaxSheetDistance.setValue(self.viewer.correspondenceEngine.getConstantDouble("MAXIMUM_DISTANCE_SHEET_SKELETON"))
        self.ui.doubleSpinBoxSheetSelfLoopLength.setValue(self.viewer.correspondenceEngine.getConstantDouble("SHEET_SELF_LOOP_LENGTH")) 

        #Matching settings tab
        self.ui.doubleSpinBoxEuclideanToPDBRatio.setValue(self.viewer.correspondenceEngine.getConstantDouble("EUCLIDEAN_VOXEL_TO_PDB_RATIO"))
        if(self.viewer.correspondenceEngine.getConstantInt("COST_FUNCTION") == 1):
            self.ui.radioButtonAbsoluteDifference.setChecked(True)
            self.ui.radioButtonNormalizedDifference.setChecked(False)
            self.ui.radioButtonQuadraticError.setChecked(False)
        elif (self.viewer.correspondenceEngine.getConstantInt("COST_FUNCTION") == 2):
            self.ui.radioButtonAbsoluteDifference.setChecked(False)
            self.ui.radioButtonNormalizedDifference.setChecked(True)
            self.ui.radioButtonQuadraticError.setChecked(False)
        elif (self.viewer.correspondenceEngine.getConstantInt("COST_FUNCTION") == 3):
            self.ui.radioButtonAbsoluteDifference.setChecked(False)
            self.ui.radioButtonNormalizedDifference.setChecked(False)
            self.ui.radioButtonQuadraticError.setChecked(True)

        self.ui.doubleSpinBoxLoopImportance.setValue(self.viewer.correspondenceEngine.getConstantDouble("LOOP_WEIGHT_COEFFICIENT"))
        self.ui.doubleSpinBoxEuclideanLoopUsedPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("EUCLIDEAN_LOOP_PENALTY"))

        self.ui.doubleSpinBoxHelixImportance.setValue(self.viewer.correspondenceEngine.getConstantDouble("HELIX_WEIGHT_COEFFICIENT"))
        if (self.viewer.correspondenceEngine.getConstantInt("MISSING_HELIX_COUNT") == -1):
            self.ui.checkBoxMissingHelices.setChecked(False)
        else:
            self.ui.checkBoxMissingHelices.setChecked(True)
            self.ui.spinBoxMissingHelixCount.setValue(self.viewer.correspondenceEngine.getConstantInt("MISSING_HELIX_COUNT"))
        self.ui.doubleSpinBoxHelixMissingPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_HELIX_PENALTY"))
        self.ui.doubleSpinBoxHelixMissingPenaltyScaled.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_HELIX_PENALTY_SCALED"))
        self.ui.doubleSpinBoxEndHelixMissingPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("START_END_MISSING_HELIX_PENALTY"))
        
        self.ui.doubleSpinBoxSheetImportance.setValue(self.viewer.correspondenceEngine.getConstantDouble("SHEET_WEIGHT_COEFFICIENT")) 
        if (self.viewer.correspondenceEngine.getConstantInt("MISSING_SHEET_COUNT") == -1):
            self.ui.checkBoxMissingSheets.setChecked(False)
        else:
            self.ui.checkBoxMissingSheets.setChecked(True)
            self.ui.spinBoxMissingSheetCount.setValue(self.viewer.correspondenceEngine.getConstantInt("MISSING_SHEET_COUNT"))
        self.ui.doubleSpinBoxSheetMissingPenalty.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_SHEET_PENALTY")) 
        self.ui.doubleSpinBoxSheetMissingPenaltyScaled.setValue(self.viewer.correspondenceEngine.getConstantDouble("MISSING_SHEET_PENALTY_SCALED"))

    def getConstraints(self):
        #Constraints
        print "reading constraints from c++ to gorgon"

        corr = self.viewer.correspondenceLibrary.correspondenceList[0]
        numH = len(self.viewer.correspondenceLibrary.structureObservation.helixDict)

        # count number of helices and sheets in this correspondence
        hIx = 1
        sIx = 1
        graphIx = 1
        for i in range(len(corr.matchList)):
            match = corr.matchList[i]
            #print "object has type " + str(type(match.predicted))
            if match.predicted is not None:
                if match.predicted.type == 'strand':
                    print "reading constraints for strand " + str(sIx) + " (graph node " + str(graphIx) + ")"
                    obsSheet = self.viewer.correspondenceEngine.getStrandConstraint(graphIx,0)
                    constrained = (obsSheet != 0)
                    if (obsSheet == -1):
                        sheetNum = -1
                    elif (obsSheet > 0):
                        sheetNum = obsSheet - 2 * numH
                    if constrained:
                        print "  constrained to sheet " + str(sheetNum) + " (graph node " + str(obsSheet) + ")"
                        self.constrainSSE(i, sheetNum, 0)
                                                                            
                    
                    sIx += 1
                    graphIx += 1
                elif match.predicted.type == 'helix':
                    print "reading constraints for helix " + str(hIx) + " (graph node " + str(graphIx) + ")"
                    obsHelixFwd = self.viewer.correspondenceEngine.getHelixConstraintFwd(graphIx)
                    obsHelixRev = self.viewer.correspondenceEngine.getHelixConstraintRev(graphIx)
                    obsHelixUnk = self.viewer.correspondenceEngine.getHelixConstraintUnk(graphIx)
                    print "  fwd constraint = " + str(obsHelixFwd)
                    print "  rev constraint = " + str(obsHelixRev)
                    print "  unk constraint = " + str(obsHelixUnk)
                    if (obsHelixFwd == -1 or obsHelixRev==-1 or obsHelixUnk==-1):
                        constrained = True
                        helixNum = -1
                    elif (obsHelixFwd != 0):
                        constrained = True
                        helixNum = (obsHelixFwd+1)/2
                        helixDir = 1
                    elif (obsHelixRev != 0):
                        constrained = True
                        helixNum = obsHelixRev/2
                        helixDir = -1
                    elif (obsHelixUnk != 0):
                        constrained = True
                        helixNum = (obsHelixUnk+1)/2
                        helixDir = 0
                    
                    if (constrained):
                        print "  constrained to helix " + str(helixNum) + " in direction " + str(helixDir)
                        self.constrainSSE(i, helixNum, helixDir)
                        
                    hIx += 1
                    graphIx += 2
        # now that constraints are stored, clear from c++ class
        self.viewer.correspondenceEngine.clearAllConstraints()


    def populateEmptyResults(self, library):
        """ add empty result before correspondence search is started """

        # create one empty result
        corrList = []
        matchList = []            
        
        # build an empty correspondence result by creating an empty match for each secondary structure element.
        for i in range(len(library.structurePrediction.secelDict)):                                   
            observed = None
            predicted = library.structurePrediction.secelDict[i]
            currentMatch = Match(observed, predicted, Match.FORWARD)
            currentMatch.constrained = False     
            matchList.append(currentMatch)                                              
            corr = Correspondence(library=library, matchList=matchList, score=0)            

            # add the empty correspondence to the list
            # ss 5/28 indenting next line to see if fixes bug?
            corrList.append(corr)
        return corrList
            

    
    def populateResults(self, library):

        print "Starting to populate results. Found " + str(self.resultCount) + " results."

        self.ui.tabWidget.setCurrentIndex(4)
        # clear the correspondence list
        corrList = []
        
        # iterate over all results from correspondence algorithm
        #print "Iterating over results."
        for i in range(self.resultCount):                                
            # create a Correspondence object and add it to the list

            #print "-------------- RESULT " + str(i) + " --------------"

            # start from correspondenceEngine result
            result = self.viewer.correspondenceEngine.getResult(i+1)
            
            matchList = [] # matchList holds the matches
            
            # iterate over all nodes in the matching from correspondenceEngine
            #print "Iterating over " + str(result.getNodeCount()) + " nodes of this result."
            isSecondHelixNode = False
            helicesPassed = 0
            #for j in range(result.getNodeCount()/2):
            
            # TODO: Fix code so it handles missing helices correctly. Test on 3LCK data.

            #for k in range(len(library.structurePrediction.secelType)):
                #print "secelType says element " + str(k) + " is a " + str(library.structurePrediction.secelType[k])

            
            for j in range(result.getNodeCount()):
                # print "iterating. j = " + str(j)
                if isSecondHelixNode == False:

                    # print "j = " + str(j)
                    direction = Match.FORWARD

                    # predicted helix or strand in sequence graph
                    #predicted = library.structurePrediction.secelDict[j]
                    # print "predicted is element " + str(j - helicesPassed) + " from secelDict, which has " + str(len(library.structurePrediction.secelDict)) + " entries" 
                    # print "secelType says element " + str(j - helicesPassed) + " is a " + str(library.structurePrediction.secelType[j - helicesPassed])
                    predicted = library.structurePrediction.secelDict[j - helicesPassed]
                    predictedType = library.structurePrediction.secelType[j - helicesPassed] # 'helix' or 'strand'
                    if predictedType == 'helix':
                        isSecondHelixNode = True
                        #print "predicted helix. node=" + str(j)
                    #else:
                        #print "predicted strand. node=" + str(j)
                        
                    # observed helix or sheet in skeleton graph
                    # print "secelDict has size " + str(len(library.structurePrediction.secelDict))
                    # j is a helix node in the sequence graph
                    # n1 and n2 are skeleton graph nodes for entry and exit points of the helix
                    n1 = result.getSkeletonNode(j)
                    # print "n1 is " + str(n1)
                    # print "result says the number of helices is " + str(result.getHelixCount())
                    if (n1 < 2 * result.getHelixCount() and n1 >= 0):
                        observedType = 'helix'
                    elif n1 >= 2 * result.getHelixCount():
                        observedType = 'sheet'
                    else:
                        observedType = 'none'
                        #print "error!!!"
                        #print "type none found at node " + str(j)
                    # print "the observed type is " + str(observedType)
                        
                    isHelix = (n1 < 2 * result.getHelixCount())
                    #isSheet = !(isHelix)
                    #if (isHelix):
                    if observedType == 'helix':
                        # print "node " + str(n1) + " is a helix"
                        isSecondHelixNode = True
                        n2 = result.getSkeletonNode(j+1)
                        if n1 < n2:
                            # print "forward match, nodes are " + str(n1) + " and " + str(n2)
                            direction = Match.FORWARD
                        else:
                            # print "reverse match, nodes are " + str(n1) + " and " + str(n2)
                            direction = Match.REVERSE
                    elif observedType == 'sheet':
                        # print "node " + str(n1) + " is a sheet"
                        direction = Match.FORWARD
                    #elif observedType == 'none':
                        # print "node " + str(n1) + " is not a sheet or a helix"
                                            
                    # lookup which helix from skeleton graph these nodes refer to
                    observedNo = result.nodeToHelix(n1)
                    # print "graph index " + str(n1) + " corresponds to skeleton SSE " + str(observedNo)
                    # print "observed is element " + str(observedNo) + " from helixDict, which has " + str(len(library.structureObservation.helixDict)) + " entries" 
                    # print "sheetDict has " + str(len(library.structureObservation.sheetDict)) + " entries" 
                    if observedNo >= 0:
                        if isHelix:
                            # helix in skeleton graph
                            observed = library.structureObservation.helixDict[observedNo]
                            #print "observed helix. node=" + str(j) + ", observeNo=" + str(observedNo) + ", observed=" + str(observed)
                        else: # sheet 
                            observed = library.structureObservation.sheetDict[observedNo - result.getHelixCount()]
                            #print "observed sheet. node=" + str(j) + ", observeNo=" + str(observedNo) + ", observed=" + str(observed)
                            #observed = None # for now, no code for sheets 
                    else:
                        observed = None
                        
                    
                    # create Match object holding the observed and predicted helices, and the match direction
                    currentMatch = Match(observed, predicted, direction)
                    
                    # if this helix has a constraint, store the constraint in the Match object
                    if(self.userConstraints.has_key(j - helicesPassed)):
                        currentMatch.constrained = self.userConstraints[j - helicesPassed]
                    else :                      
                        currentMatch.constrained = False     
                    # append this match to the list of matches for this result
                    matchList.append(currentMatch)
                             
                else: # isSecondHelixNode is true
                    isSecondHelixNode = False
                    helicesPassed += 1

            # now matchList holds all the helix correspondences for a single match result

            # create Correspondence object for this correspondence
            # print "Creating correspondence object for this correspondence."
            corr = Correspondence(library=library, matchList=matchList, score=result.getCost())
            
            # add to list of correspondences
            #print "Appending this correspondence object to list."
            corrList.append(corr)
            
        # corrList now holds all correspondences between the sequence and the graph,
        # as determined by the graphMatch algorithm
        print "Finished populating results."
        return corrList
            
    def populateComboBox(self, library):
        self.ui.comboBoxCorrespondences.clear()
        # add all correspondence to pulldown menu
        for i in range(len(library.correspondenceList)):
            corr = library.correspondenceList[i]                               
            self.ui.comboBoxCorrespondences.addItem("Correspondence " + str(i+1) + " - [Cost: " + str(corr.score) + "]")
                   
                   
    def createBasicCorrespondence(self):
        """Writes search parameters to correspondence object, loads predicted structure and observed structure, and creates correspondence library"""
        print "creating basic correspondence"
        self.setCursor(QtCore.Qt.BusyCursor)

        # put user-entered match parameters from UI into the correspondence object
        print "setting constants"
        self.setConstants()          
        
        
        #Loading Predicted SSEs                     
        print "loading predicted SSEs"
        # call to c++ method QueryEngine::LoadSequenceGraph()
        self.viewer.correspondenceEngine.loadSequenceGraph()
        
        print "before calling StructurePrediction.load"
        print "sequenceFileName is " + str(self.sequenceFileName)
        print "app is " + str(self.app)
        includeStrands = self.viewer.correspondenceEngine.getConstantInt("INCLUDE_STRANDS")
        structPred = StructurePrediction.load(self.sequenceFileName, self.app, includeStrands)
        print "after calling StructurePrediction.load"
        cAlphaViewer = self.app.viewers['calpha']
        sseViewer = self.app.viewers['sse']
        skeletonViewer = self.app.viewers['skeleton']
        cAlphaViewer.structPred = structPred
        def vector3DFloatToTuple(v3df):
            return (v3df.x(), v3df.y(), v3df.z())
        
        #Loading Observed SSEs
        print "loading observed SSEs"
        # call to c++ method SSECorrespondenceEngine::LoadSkeletonGraph()
        self.viewer.correspondenceEngine.loadSkeletonGraph()
        observedHelices = {}
        helixCount = 0
        observedSheets = {}
        sheetCount = 0
        # call to c++ method QueryEngine::getSkeletonSSECount()
        sseCount = self.viewer.correspondenceEngine.getSkeletonSSECount()

        print "adding helices to list of observed helices"
        for sseIx in range(sseCount):
            # call to c++ method QueryEngine::getSkeletonSSE(), which returns a c++ GeometricShape object
            cppSse = self.viewer.correspondenceEngine.getSkeletonSSE(sseIx)
            
            # create list of observed helices for this correspondence result
            if cppSse.isHelix():            
                #TODO: check whether these should be getCornerCell3(...
                p1 = cAlphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell2(1))))
                p2 = cAlphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell2(2))))
                
                q1 = cAlphaViewer.worldToObjectCoordinates(sseViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell3(1))))
                q2 = cAlphaViewer.worldToObjectCoordinates(sseViewer.objectToWorldCoordinates(vector3DFloatToTuple(cppSse.getCornerCell3(2))))
                if vectorSize(vectorAdd(p1, vectorScalarMultiply(-1, q1))) > vectorSize(
                                        vectorAdd(p1, vectorScalarMultiply(-1, q2))): #to get proper orientation
                    q1, q2 = q2, q1 #python trick for exchanging values
            
                pyHelix = ObservedHelix(sseIx, q1, q2)
                observedHelices[helixCount] = pyHelix
                helixCount = helixCount + 1

            elif cppSse.isSheet():
                cornerList = {}
                cornerNum = 1
                while True:
                    corner = vector3DFloatToTuple(cppSse.getCornerCell2(cornerNum))
                    lastSheet = ( corner == (0,0,0) )
                    p1 = cAlphaViewer.worldToObjectCoordinates(skeletonViewer.objectToWorldCoordinates(corner))
                    if lastSheet:
                        break
                    cornerList[corner] = p1
                    print "adding sheet corner " + str(cornerNum) + " with coordinates (" + str(p1[0]) + "," + str(p1[1]) + "," + str(p1[2]) + ")"
                    cornerNum = cornerNum + 1
                print "done adding sheet corners."
                pySheet = ObservedSheet(sseIx, cornerList)
                # adding sheets to list of observedHelices
                #observedHelices[helixCount + sheetCount + 1] = pySheet
                observedSheets[sheetCount] = pySheet
                print "added a sheet to observedSheets. total number of sheets is now " + str(len(observedSheets))
                sheetCount = sheetCount + 1
                #pass
                #TODO: Add Sheet support
        
        #TODO: Mike this raises an error!;
        print "found " + str(helixCount) + " helices and " + str(sheetCount) + " sheets"
        structObserv = StructureObservation(helixDict = observedHelices, sheetDict = observedSheets)
        print "writing to correspondenceLibrary"

        # create a new python CorrespondenceLibrary object 
        self.viewer.correspondenceLibrary = CorrespondenceLibrary(sp = structPred, so = structObserv)          
               
        self.setCursor(QtCore.Qt.ArrowCursor)
        
        print "finished creating basic correspondences" 
        
    def accept(self):
        print "ok button pushed"
        # read user parameters, read skeleton and sequence files, create correspondence library
        self.createBasicCorrespondence()   
        # TODO: At this point, the secelDict is not defined. Why didn't it work?    
        print "after creating basic correspondence, secelDict has length " + str(len(self.viewer.correspondenceLibrary.structurePrediction.secelDict))   
                
        # execute correspondence query and do cleanup
        print "executing query"
        self.resultCount = self.viewer.correspondenceEngine.executeQuery()
        print "found " + str(self.resultCount) + " results. cleaning up memory."
        self.viewer.correspondenceEngine.cleanupMemory()
        self.viewer.correspondenceEngine.clearAllConstraints()

        
        # populate the list of found correspondences        
        print "populating result list"
        #print "found " + size(self.viewer.correspondenceLibrary.correspondenceList) + " results. populating result list"
        self.viewer.correspondenceLibrary.correspondenceList = self.populateResults(self.viewer.correspondenceLibrary)
        #print "correspondenceList has length " + str(len(self.viewer.correspondenceLibrary.correspondenceList))

        #print "populating result pulldown"
        self.populateComboBox(self.viewer.correspondenceLibrary)       
        
        self.executed = True 
        #print "emitting model changed signal"
        self.viewer.emitModelChanged()

        print "done"
                
    def reject(self):  
        self.executed = False
        self.app.actions.getAction("perform_SSEFindHelixCorrespondences").trigger()
            
    def getIndexedSheetColor(self, index, size):
        """returns a color for sheet 'index' out of 'size' sheets. colors will be orange or red."""
        # start and end are between 0 and 1
        start = float(0.77)
        #start = float(0.0)
        end = float(1.0)
        delta = (end - start) / float(size)
        i = start + delta * float(index)
        return self.getIndexedColor(i, 1.0)
            
    def getIndexedHelixColor(self, index, size):
        """returns a color for helix 'index' out of 'size' helices. colors will be blue, green, or yellow."""
        # start and end are between 0 and 1
        start = float(0.0)
        end = float(0.75)
        delta = (end - start) / float(size)
        i = start + delta * float(index)
        return self.getIndexedColor(i, 1.0)
            
    def getIndexedColor(self, index, size):
        a = 1.0
        i = float(index)
        N = float(size)

        # blue  (0,0,1)
        if (i/N) == 0:
            r=0;g=0;b=1.0

        # blue - cyan(0,1,1)
        elif (i/N) <= 0.25:
            coeff = (i/N)/0.25
            r=0.0
            g=coeff
            b=1.0

        # cyan - green (0,1,0)
        elif (i/N) > 0.25:
            coeff=((i/N)-0.25)/0.25
            r=0.0
            g=1.0
            b=1.0-coeff

            # green - yellow (1,1,0)
            if (i/N) > 0.50:
                coeff=((i/N)-0.25)/0.50
                r=coeff
                g=1.0
                b=0.0

                # yelow - red (1,0,0)
                if (i/N) > 0.75:
                    coeff=((i/N)-0.25)/0.75
                    r=1.0
                    g=1-coeff
                    b=0.0

                    # red
                    if (i/N)==1.0:
                        r=1.0;g=0;b=0
                    
        return QtGui.QColor.fromRgba(QtGui.qRgba(r*255, g*255, b*255, a*255))
        
    def selectCorrespondence(self, correspondenceIndex):
        self.loadingCorrespondance = True
        self.ui.tableWidgetCorrespondenceList.clearContents()
        if(correspondenceIndex >= 0):
            corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
            self.ui.tableWidgetCorrespondenceList.setRowCount(len(corr.matchList))   
            notMissing = {}
            
            # count number of helices and sheets in this correspondence
            helixCount = 0
            sheetCount = 0
            for i in range(len(corr.matchList)):
                match = corr.matchList[i]
                #print "object has type " + str(type(match.predicted))
                if match.predicted is not None:
                    if match.predicted.type == 'strand':
                        # TODO: only count strands that are different. every sheet must be the same color!
                        #print "found strand"
                        sheetCount += 1
                    elif match.predicted.type == 'helix':
                        #print "found helix"
                        helixCount += 1

           
            helixIndex = 0
            sheetIndex = 0

            for i in range(len(corr.matchList)):
                match = corr.matchList[i]
                #color = self.getIndexedColor(i, len(corr.matchList))
                if match.predicted is not None:
                    if match.predicted.type == 'strand':
                        if(match.observed):
                            color = self.getIndexedSheetColor(match.observed.label - sheetCount, sheetCount)
                        else:
                            color = self.getIndexedColor(0,100)
                        #color = self.getIndexedSheetColor(sheetIndex, sheetCount)
                        sheetIndex += 1
                    elif match.predicted.type == 'helix':
                        color = self.getIndexedHelixColor(helixIndex, helixCount)
                        helixIndex += 1
                    match.predicted.setColor(color)
                if(match.predicted):
                    #print match.predicted, match.predicted.type, match.predicted.serialNo, match.predicted.label
                    cellItemPredicted =  QtGui.QTableWidgetItem(match.predicted.type + " " + str(match.predicted.serialNo) + " : " + str(match.predicted.label) +
                                                                "\n  "  + str(round(match.predicted.getLengthInAngstroms(),2)) + "A length"
                                                                "\n  "  + str(match.predicted.getResidueCount()) + " residues")
                                                                 
                    cellItemPredicted.setBackgroundColor(color)
                    self.ui.tableWidgetCorrespondenceList.setItem(i, 0, cellItemPredicted)
                if(match.observed):
                    # TODO: Clean up this code. Made quick fixes to support sheets.
                    if match.observed.sseType == 'helix':
                        cellItemObserved =  QtGui.QTableWidgetItem("helix " + str(match.observed.label+1) +
                                                                   "\n  " + str(round(match.observed.getLength(), 2)) + "A length" +
                                                                   "\n  " + str(match.direction) )
                    if match.observed.sseType == 'sheet':
                        cellItemObserved =  QtGui.QTableWidgetItem("sheet " + str(match.observed.label+1) +
                                                                   #"\n  " + str(round(match.observed.getLength(), 2)) + "A length" +
                                                                   "\n  " )
                    cellItemObserved.setBackgroundColor(color)
                    self.ui.tableWidgetCorrespondenceList.setItem(i, 1, cellItemObserved)
                    if match.observed.sseType == 'helix':
                        # color is stored in two places: the renderer and the correspondence engine. update both.
                        self.viewer.renderer.setHelixColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        #print "before setSSEColor"
                        self.viewer.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        #print "after setSSEColor"

                    # TODO: add support to renderer for colored sheets
                    if match.observed.sseType == 'sheet':
                    # TODO: Fix here. This does not color the correct sheet, and it may break if there are no sheets!
                    #    print "type is helix; setting color"
                    #    self.viewer.renderer.setSheetColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        # color is stored in two places: the renderer and the correspondence engine. update both.
                        #self.viewer.renderer.setSheetColor(2, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        #self.viewer.renderer.setSheetColor(match.observed.label - helixCount, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        self.viewer.renderer.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                        self.viewer.correspondenceEngine.setSSEColor(match.observed.label, color.redF(), color.greenF(), color.blueF(), color.alphaF())

                    notMissing[match.observed.label] = True                                                     
            
                checkBox = QtGui.QCheckBox()
                self.ui.tableWidgetCorrespondenceList.setCellWidget(i, 2, checkBox)
                self.connect(checkBox, QtCore.SIGNAL("stateChanged (int)"), self.constraintAdded)
                if(match.constrained):
                    self.ui.tableWidgetCorrespondenceList.cellWidget(i, 2).setCheckState(QtCore.Qt.Checked)
                else :
                    self.ui.tableWidgetCorrespondenceList.cellWidget(i, 2).setCheckState(QtCore.Qt.Unchecked)
                    
                self.ui.tableWidgetCorrespondenceList.resizeRowToContents(i)
                
            observedHelices = self.viewer.correspondenceLibrary.structureObservation.helixDict
            for i in range(len(observedHelices)):
                if(not notMissing.has_key(i)):
                    self.viewer.renderer.setHelixColor(i, 0.5, 0.5, 0.5, 1.0)
                
                    
        self.viewer.correspondenceEngine.setVisibleCorrespondence(correspondenceIndex)
        self.viewer.correspondenceLibrary.setCurrentCorrespondenceIndex(correspondenceIndex)
        self.viewer.emitModelChanged()
        self.loadingCorrespondance = False
        
    def drawOverlay(self):
        if self.executed and self.corrAct.isChecked():
            glPushAttrib(GL_LIGHTING_BIT)
            self.viewer.setMaterials(self.app.themes.getColor("CorrespondenceFinder:BackboneTrace"))  
            # calls Draw method of c++ SSECorrespondenceEngine object          
            self.viewer.correspondenceEngine.draw(0)
            #self.viewer.correspondenceEngine.draw(match)
            glPopAttrib()
        if self.corrAct.isChecked() and self.dataLoaded and (self.ui.checkBoxShowAllPaths.isChecked() or self.ui.checkBoxShowHelixCorners.isChecked() or self.ui.checkBoxShowSheetCorners.isChecked() or self.ui.checkBoxShowSheetColors.isChecked() ) :
            # probably not the best place for this code
            # set colors of all SSEs
            # need to learn how for loops work!
            #print "preparing to set colors"

            # TODO: Move this color changing code somewhere else
            # Probably should use the setColor calls in previous sections.
            for i in range(self.viewer.correspondenceEngine.getSkeletonSSECount()) :
                #print "setting color for helix "
                #print i
                color = self.getIndexedHelixColor(i, self.viewer.correspondenceEngine.getSkeletonSSECount())
                #color = self.getIndexedColor(i, self.viewer.correspondenceEngine.getSkeletonSSECount())
                ####self.viewer.renderer.setSSEColor(i, color.redF(), color.greenF(), color.blueF(), color.alphaF())
                ####self.viewer.correspondenceEngine.setSSEColor(i, color.redF(), color.greenF(), color.blueF(), color.alphaF())
            glPushAttrib(GL_LIGHTING_BIT)
            self.viewer.setMaterials(self.app.themes.getColor("CorrespondenceFinder:BackboneTrace"))  
            # calls DrawAllPaths method of c++ SSECorrespondenceEngine object          
            self.viewer.correspondenceEngine.drawAllPaths(0,self.ui.checkBoxShowAllPaths.isChecked(),self.ui.checkBoxShowHelixCorners.isChecked(),self.ui.checkBoxShowSheetCorners.isChecked(),False)
            glPopAttrib()
            
            
    def rebuildGraph(self):
        print "correspondence index before rebuilding is "
        print self.ui.comboBoxCorrespondences.currentIndex()
        self.ui.comboBoxCorrespondences.setCurrentIndex(-1)
        print "correspondence index after setting to -1 is "
        print self.ui.comboBoxCorrespondences.currentIndex()
        self.setConstants()
        self.checkOk()
        self.viewer.makeSheetSurfaces()
        self.viewer.emitModelChanged()
        print "correspondence index after rebuilding is "
        print self.ui.comboBoxCorrespondences.currentIndex()
        
    def constraintAdded(self, state):
        if(not self.loadingCorrespondance):
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]            
                for i in range(len(corr.matchList)):
                    match = corr.matchList[i]
                    match.constrained = (self.ui.tableWidgetCorrespondenceList.cellWidget(i, 2).checkState() == QtCore.Qt.Checked)
                    
    def createActionsForCell(self, row, col):
        self.selectedRow = row
        for act in self.ui.tableWidgetCorrespondenceList.actions()[:]:
            self.ui.tableWidgetCorrespondenceList.removeAction(act)        
        if(col == 1):
            observedHelices = self.viewer.correspondenceLibrary.structureObservation.helixDict
            observedSheets = self.viewer.correspondenceLibrary.structureObservation.sheetDict
            constrained = {}
            
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]  
                for i in range(len(corr.matchList)):
                    match = corr.matchList[i]
                    if(match.constrained and match.observed) :
                        constrained[match.observed.label] = True
                match = corr.matchList[row]
                
            else:
                match = False
            if match.predicted.type == 'helix':
                for i in range(len(observedHelices)):                
                    constrainAction = QtGui.QAction(self.tr("Observed helix " + str(i+1) + " (Length: " + str(round(observedHelices[i].getLength(), 2)) + "A)"), self)
                    constrainAction.setCheckable(True)
                    if(match and match.observed):
                        constrainAction.setChecked(match.observed.label == i)
                    else:
                        constrainAction.setChecked(False)
                    constrainAction.setEnabled(not constrained.has_key(i))
                    self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedHelix(i))       
                    self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)
            if match.predicted.type == 'strand':
                numH = len(observedHelices)
                for i in range(len(observedSheets)):                
                    constrainAction = QtGui.QAction(self.tr("Observed sheet " + str(i+numH+1)), self)
                    #constrainAction = QtGui.QAction(self.tr("Observed sheet " + str(i+numH+1) + " (Area: " + str(round(observedSheets[i].getSize(), 2)) + " voxels)"), self)
                    constrainAction.setCheckable(True)
                    if(match and match.observed):
                        constrainAction.setChecked(match.observed.label == i)
                    else:
                        constrainAction.setChecked(False)
                    constrainAction.setEnabled(True)
                    self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedSheet(i))       
                    self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)
                
            constrainAction = QtGui.QAction(self.tr("Not observed"), self)
            constrainAction.setCheckable(True)
            constrainAction.setChecked(match and not match.observed)
            constrainAction.setEnabled(True)
            self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainObservedHelix(-1))       
            self.ui.tableWidgetCorrespondenceList.addAction(constrainAction)                

    def customMenuRequested(self, point):
        self.createActionsForCell(self.ui.tableWidgetCorrespondenceList.currentRow(), self.ui.tableWidgetCorrespondenceList.currentColumn())
        if(len(self.ui.tableWidgetCorrespondenceList.actions()) > 0):
            menu = QtGui.QMenu()
            for act in self.ui.tableWidgetCorrespondenceList.actions()[:]:
                menu.addAction(act)
            menu.exec_(self.ui.tableWidgetCorrespondenceList.mapToGlobal(point), self.ui.tableWidgetCorrespondenceList.actions()[0])

    def constrainSSE(self, pred, obs, dir):
        correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
        if(correspondenceIndex >= 0):
            corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
            match = corr.matchList[pred]
            match.constrained = True
            if(obs == -1):
                match.observed = None
            else:
                if match.predicted.type == 'helix':
                    match.observed = self.viewer.correspondenceLibrary.structureObservation.helixDict[obs-1]
                    if (dir == 1): 
                        match.directionConstrained = True
                        match.direction = Match.FORWARD
                    elif (dir == -1):
                        match.directionConstrained = True
                        match.direction = Match.REVERSE
                    else:
                        match.directionConstrained = False
                            
                if match.predicted.type == 'strand':
                    match.observed = self.viewer.correspondenceLibrary.structureObservation.sheetDict[obs-1]
                
        self.selectCorrespondence(correspondenceIndex)

    def constrainObservedHelix(self, i):
        def constrainObservedHelix_i():
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]

                # clear old constraints involving helix i
                for j in range(len(corr.matchList)):
                    match = corr.matchList[j]
                    if(match and match.observed and (match.observed.label == i)) :
                        match.observed = None
                
                match = corr.matchList[self.selectedRow]
                match.constrained = True
                if(i == -1):
                    match.observed = None
                else:
                    match.observed = self.viewer.correspondenceLibrary.structureObservation.helixDict[i]
            self.selectCorrespondence(correspondenceIndex)
        return constrainObservedHelix_i

    def constrainObservedSheet(self, i):
        def constrainObservedSheet_i():
            numH = len(self.viewer.correspondenceLibrary.structureObservation.helixDict)
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
                match = corr.matchList[self.selectedRow]
                match.constrained = True
                if(i == -1):
                    match.observed = None
                else:
                    match.observed = self.viewer.correspondenceLibrary.structureObservation.sheetDict[i]
            self.selectCorrespondence(correspondenceIndex)
        return constrainObservedSheet_i

    def constrainPredictedHelix(self, predicted, observed):
        def constrainPredictedHelix_po():
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
                for j in range(len(corr.matchList)):
                    match = corr.matchList[j]
                    if(match and match.observed and (match.observed.label == observed)) :
                        match.observed = None
                        match.constrained = False
                    if(match and match.predicted and match.predicted.type == 'helix' and match.predicted.serialNo == predicted):
                        newMatch = match
                match = newMatch
                match.constrained = True
                match.observed = self.viewer.correspondenceLibrary.structureObservation.helixDict[observed]
            self.selectCorrespondence(correspondenceIndex)                
        return constrainPredictedHelix_po
    
    def constrainPredictedStrand(self, predicted, observed):
        def constrainPredictedStrand_po():
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]
                for j in range(len(corr.matchList)):
                    match = corr.matchList[j]
                    if(match and match.observed and (match.observed.label == observed)) :
                        match.observed = None
                        match.constrained = False
                    if(match and match.predicted and match.predicted.type == 'strand' and match.predicted.serialNo == predicted):
                        newMatch = match
                match = newMatch
                match.constrained = True
                match.observed = self.viewer.correspondenceLibrary.structureObservation.sheetDict[observed]
            self.selectCorrespondence(correspondenceIndex)                
        return constrainPredictedStrand_po
    
    def sseClicked(self, hit0, hit1, hit2, hit3, hit4, hit5, event):
        #print "sseClicked. visible=" + str(self.isVisible()) + ", loaded=" +str(self.dataLoaded)+", hit0=" + str(hit0) + ", hit1=" + str(hit1)
        if(self.isVisible() and self.dataLoaded and ((hit0 == 0) or (hit0 == 1) or (hit0 == 2)) and (hit1 >= 0)):
            observedType = hit0
            observedSSE = hit1
            constrained = {}
            match = None        
            matchKey = 0    
            matchKeys = []    
            correspondenceIndex = self.ui.comboBoxCorrespondences.currentIndex()
            numH = len(self.viewer.correspondenceLibrary.structureObservation.helixDict)
            if(correspondenceIndex >= 0):
                corr = self.viewer.correspondenceLibrary.correspondenceList[correspondenceIndex]  
                for i in range(len(corr.matchList)):
                    m = corr.matchList[i]
                    if(m.constrained) :
                        constrained[m.predicted.serialNo] = True

                    # find the index of the selected helix in the correspondence list
                    if observedType==0 and m.observed and m.observed.sseType == 'helix':
                        if(m.observed.label == observedSSE):
                            #print "helix match found at m (i=" +str(i)+") with label=" + str(m.observed.label)
                            match = m
                            matchKey = i
                            matchKeys.append(i)
                    
                    # find the index of the selected sheet in the correspondence list
                    #if observedType==1 and m.observed and m.observed.sseType == 'sheet':
                    #    if(m.observed.label == observedHelix):
                    #        #print "sheet match found at m (i=" +str(i)+") with label=" + str(m.observed.label)
                    #        match = m
                    #        matchKey = i
                    
                    # find the index of the selected sheet in the correspondence list
                    if observedType==2 and m.observed and m.observed.sseType == 'sheet':
                        print "graph sheet clicked (i=" +str(i)+") with label=" + str(m.observed.label) + ". observedSSE=" + str(observedSSE) 
                        if(m.observed.label-numH+1 == observedSSE):
                            print "sheet match found at m (i=" +str(i)+") with label=" + str(m.observed.label)
                            match = m
                            matchKey = i
                            matchKeys.append(i)
                    
            self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(0, 0, self.ui.tableWidgetCorrespondenceList.rowCount()-1, 2), False)                    
            if(match):
                self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(0, 0, self.ui.tableWidgetCorrespondenceList.rowCount()-1, 2), False)                    
                self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(matchKey, 0, matchKey, 2),True)
                for matchItem in matchKeys:
                    self.ui.tableWidgetCorrespondenceList.setRangeSelected(QtGui.QTableWidgetSelectionRange(matchItem, 0, matchItem, 2),True)
                    
            if(self.app.mainCamera.mouseRightPressed):                
                predictedHelices = self.viewer.correspondenceLibrary.structurePrediction.helixDict                            
                predictedStrands = self.viewer.correspondenceLibrary.structurePrediction.strandDict                            
                menu = QtGui.QMenu(self.tr("Constrain observed SSE " + str(observedSSE+1)))
                
                for i in range(len(predictedHelices)):
                    if observedType==0 and predictedHelices[i].type == 'helix': 
                        constrainAction = QtGui.QAction(self.tr("Predicted helix " + str(predictedHelices[i].serialNo)), self)
                        constrainAction.setCheckable(True)
                        if(match and match.observed):
                            constrainAction.setChecked(match.predicted.serialNo == predictedHelices[i].serialNo)
                            #constrainAction.setChecked(match.predicted.serialNo-1 == i)
                        else:
                            constrainAction.setChecked(False)
                        constrainAction.setEnabled(not constrained.has_key(predictedHelices[i].serialNo))
                        self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainPredictedHelix(predictedHelices[i].serialNo, observedSSE))       
                        menu.addAction(constrainAction)           
                
                    #if observedType==1 and predictedStrands[i].type == 'strand':
                    #    constrainAction = QtGui.QAction(self.tr("Predicted strand " + str(predictedSecels[i].serialNo)), self)
                    #    constrainAction.setCheckable(True)
                    #    if(match and match.observed):
                    #        constrainAction.setChecked(match.predicted.serialNo == i)
                    #    else:
                    #        constrainAction.setChecked(False)
                    #    constrainAction.setEnabled(not constrained.has_key(predictedSecels[i].serialNo))
                    #    self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainPredictedHelix(predictedSecels[i].serialNo, observedHelix))       
                    #    menu.addAction(constrainAction)           
                
                    if observedType==2 and predictedStrands[i].type == 'strand':
                        constrainAction = QtGui.QAction(self.tr("Predicted strand " + str(predictedStrands[i].serialNo)), self)
                        constrainAction.setCheckable(True)
                        if(match and match.observed):
                            constrainAction.setChecked(match.predicted.serialNo == i)
                        else:
                            constrainAction.setChecked(False)
                        constrainAction.setEnabled(not constrained.has_key(predictedStrands[i].serialNo))
                        self.connect(constrainAction, QtCore.SIGNAL("triggered()"), self.constrainPredictedStrand(predictedStrands[i].serialNo, observedSSE))       
                        menu.addAction(constrainAction)           
                
                menu.exec_(self.app.mainCamera.mapToGlobal(self.app.mainCamera.mouseDownPoint))
                self.app.mainCamera.updateGL()
