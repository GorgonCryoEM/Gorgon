from base_viewer import BaseViewer
from ...toolkit.libpytoolkit import VolumeRenderer
from ..histogram.volume_surface_editor_form import VolumeSurfaceEditorForm


class VolumeViewer(BaseViewer):

    def __init__(self, main, parent=None):
        self.title = "Volume"
        super(VolumeViewer, self).__init__(VolumeRenderer(), main)
        self.shortTitle = "VOL"
        
        self.loaded = False
        
        self.form = VolumeSurfaceEditorForm(main)
        # self.setColor(QtGui.QColor(50, 200, 50, 150))
        
        self.ui.pushButtonSave.clicked.connect(self.saveData)

    def modelLoadedPreDraw(self):
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2

        maxRadius = int(max(self.renderer.getMaxPos(0), self.renderer.getMaxPos(1), self.renderer.getMaxPos(2)));
        self.renderer.setSampleInterval(1)
        self.renderer.setSurfaceValue(defaultDensity)
        self.renderer.setDisplayRadius(maxRadius)
        
        self.form.setViewer(self)
        self.form.modelLoadedPreDraw()
        
    def saveData(self):
        self.fileName = QtGui.QFileDialog.getSaveFileName(self, self.tr("Save Data"), "")
        if not self.fileName.isEmpty():
            self.setCursor(QtCore.Qt.WaitCursor)
            self.renderer.saveFile(str(self.fileName))
            self.dirty = False
            self.setCursor(QtCore.Qt.ArrowCursor)
