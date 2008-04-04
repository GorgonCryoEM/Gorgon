import sys
from window_manager import WindowManager
from PyQt4 import QtGui
from main_window_form import MainWindowForm
from about_form import AboutForm

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    window = MainWindowForm()
    window.addModule(WindowManager(window))
    window.show()
    sys.exit(app.exec_())
    
