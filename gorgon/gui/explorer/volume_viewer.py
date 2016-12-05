from base_viewer import BaseViewer
from ...toolkit.libpytoolkit import VolumeRenderer
from ..histogram.volume_surface_editor_form import VolumeSurfaceEditorForm


class VolumeViewer(BaseViewer):

    def __init__(self, main, parent=None):
        self.title = "Volume"
        super(VolumeViewer, self).__init__(VolumeRenderer(), main)
        self.shortTitle = "VOL"
        
        self.loaded = False
        
        self.addHistogram()
        # self.setColor(QtGui.QColor(50, 200, 50, 150))

    def modelLoadedPreDraw(self):
        maxDensity = self.renderer.getMaxDensity()
        minDensity = self.renderer.getMinDensity()
        defaultDensity = (minDensity + maxDensity) / 2

        maxRadius = int(max(self.renderer.getMaxPos(0), self.renderer.getMaxPos(1), self.renderer.getMaxPos(2)));
        self.renderer.setSampleInterval(1)
        self.renderer.setSurfaceValue(defaultDensity)
        self.renderer.setDisplayRadius(maxRadius)
        
    def addHistogram(self):
        self.histogram_widget = VolumeSurfaceEditorForm(self)
    
        gridlayout = self.ui.ui.gridlayout6
        gridlayout.addWidget(self.histogram_widget, 2, 0, 1, 2)
        
        self.histogram_widget.setViewer(self)
