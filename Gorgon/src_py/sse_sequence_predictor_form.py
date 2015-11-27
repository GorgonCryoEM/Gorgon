# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   Using structure prediction servers to predict SSE elements from the sequence


from PyQt4 import QtCore, QtGui
from ui_dialog_sse_sequence_predictor import Ui_DialogSSESequencePredictor
from base_dock_widget import BaseDockWidget
import webbrowser

class SSESequencePredictorForm(BaseDockWidget, Ui_DialogSSESequencePredictor):
    def __init__(self, main, sseViewer, parent=None):    
        BaseDockWidget.__init__(self, 
                                main,
                                "&Predict SSE from Sequence", 
                                "Prediction of secondary structure elements from the sequence", 
                                "perform_SSESequencePrediction", 
                                "actions-sse-sequenceprediction", 
                                "actions-sse", 
                                QtCore.Qt.LeftDockWidgetArea | QtCore.Qt.RightDockWidgetArea | QtCore.Qt.BottomDockWidgetArea, 
                                QtCore.Qt.RightDockWidgetArea, 
                                parent)
        self.setupUi(self)
        self.app = main
        self.viewer = sseViewer
        self.createUI()
    
    def createUI(self):        
        self.connect(self.pushButtonClose, QtCore.SIGNAL("pressed ()"), self.closeWindow)
        self.connect(self.pushButtonSave, QtCore.SIGNAL("pressed ()"), self.savePrediction)
        self.connect(self.pushButtonPredict, QtCore.SIGNAL("pressed ()"), self.makePrediction)
        self.connect(self.textEditSequence, QtCore.SIGNAL("textChanged ()"), self.enableButtons)
        self.connect(self.textEditPrediction, QtCore.SIGNAL("textChanged ()"), self.enableButtons)
        self.connect(self.checkBoxJpred, QtCore.SIGNAL("toggled (bool)"), self.enableButtons2)
        self.connect(self.checkBoxPsipred, QtCore.SIGNAL("toggled (bool)"), self.enableButtons2)
        self.connect(self.checkBoxScratch, QtCore.SIGNAL("toggled (bool)"), self.enableButtons2)
            
           
    def enableButtons(self):
        self.pushButtonPredict.setEnabled(
                                          (len(str(self.textEditSequence.toPlainText())) > 0) and
                                          (self.checkBoxJpred.isChecked() or
                                           self.checkBoxPsipred.isChecked() or
                                           self.checkBoxScratch.isChecked())
                                          )
        self.pushButtonSave.setEnabled(
                                       (len(str(self.textEditSequence.toPlainText())) > 0) and
                                       (len(str(self.textEditPrediction.toPlainText())) > 0))
    
    def enableButtons2(self, temp):
        self.enableButtons()
        
    def openPage(self, url):
        #try:
            webbrowser.open(url)
        #except:
        #    pass;        
        
    def closeWindow(self):
        self.app.actions.getAction("perform_SSESequencePrediction").trigger()
        
    def savePrediction(self):
        fileName = QtGui.QFileDialog.getSaveFileName(self, "Save sequence prediction", "", self.tr('Sequence Predictions (*.seq)'))
        if not fileName.isEmpty():
            filetext = 'START ' + str(self.spinBoxStart.value()) + "\n" + str(self.textEditSequence.toPlainText()) + "\n" + str(self.textEditPrediction.toPlainText())
            fout = open(str(fileName), "w")
            fout.write(filetext)
            fout.close()
    
    def makePrediction(self):
        if(self.checkBoxJpred.isChecked()):
            jpredURL = 'http://www.compbio.dundee.ac.uk/~www-jpred/cgi-bin/jpred_form?seq=' + str(self.textEditSequence.toPlainText()) + '&input=seq&pdb=checked'
            self.openPage(jpredURL)
        if(self.checkBoxPsipred.isChecked()):
            psipredURL = 'http://bioinf.cs.ucl.ac.uk/psipred/psiform.html'
            self.openPage(psipredURL)        
        if(self.checkBoxScratch.isChecked()):
            scratchURL = 'http://scratch.proteomics.ics.uci.edu/'
            self.openPage(scratchURL)                    
        QtGui.QMessageBox.information(self, 'Sequence Submitted', 'Sequence submitted. \n Please copy and paste the results on the prediction text box')
        
        
