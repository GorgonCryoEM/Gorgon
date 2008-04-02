import sys
from topology_hunter import TopologyHunter
from PyQt4 import QtCore, QtGui
from main_window import MainWindow

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
   # window.addModule(TopologyHunter(window))
    window.show()
    sys.exit(app.exec_())
    
