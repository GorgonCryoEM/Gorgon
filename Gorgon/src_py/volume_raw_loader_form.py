# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget to load RAW files 

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/volume_raw_loader_form.py,v $
#   $Revision: 1.1 $
#   $Date: 2008/12/15 22:38:31 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: volume_raw_loader_form.py,v $
#   Revision 1.1  2008/12/15 22:38:31  ssa1
#   Adding in support to load RAW volumes
#

from PyQt4 import QtCore, QtGui
from ui_dialog_volume_raw_loader import Ui_DialogVolumeRawLoader
    
class VolumeRawLoaderForm(QtGui.QDialog, Ui_DialogVolumeRawLoader):
    
    def __init__(self, main, volumeViewer, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.app = main
        self.setupUi(self)
        self.viewer = volumeViewer                       
    
    def sizeX(self):
        return self.spinBoxSizeX.value()
    
    def sizeY(self):
        return self.spinBoxSizeY.value()
    
    def sizeZ(self):
        return self.spinBoxSizeZ.value()
    
    def bitsPerCell(self):
        if(self.comboBoxFormat.currentIndex() == 0):
            return 8
        elif(self.comboBoxFormat.currentIndex() == 1):
            return 16
