from PyQt4 import QtCore, QtGui
from ui_dialog_color_picker import Ui_DialogColorPicker
from render_area import RenderArea

class ColorPickerForm(QtGui.QDialog):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.createUI()
        
    def createUI(self):
        self.ui = Ui_DialogColorPicker()
        self.ui.setupUi(self)       
        self.connect(self.ui.horizontalSliderRed, QtCore.SIGNAL("valueChanged(int)"), self.colorChanged)
        self.connect(self.ui.horizontalSliderGreen, QtCore.SIGNAL("valueChanged(int)"), self.colorChanged)
        self.connect(self.ui.horizontalSliderBlue, QtCore.SIGNAL("valueChanged(int)"), self.colorChanged)    
        
    def colorChanged(self, dummy):
        pass


    
                                 