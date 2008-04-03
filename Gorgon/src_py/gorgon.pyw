import sys
from topology_hunter import TopologyHunter
from PyQt4 import QtGui
from main_window import MainWindow
from about_form import AboutForm

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    window.addModule(TopologyHunter(window))
    window.addModule(AboutForm(window))
    window.show()
    sys.exit(app.exec_())
    
