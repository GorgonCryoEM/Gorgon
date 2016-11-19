from .explorer.skeleton_viewer import SkeletonViewer
from .calpha.viewer import CAlphaViewer
from .sse.helix_correspondence_form import SSEHelixCorrespondenceForm
from .sse.viewer import SSEViewer
from .explorer import Camera
from ..toolkit.sse.correspondence.StructurePrediction import StructurePrediction
from .window import Window


class SSECorrespondence(Window):

    def __init__(self, args):
        super(SSECorrespondence, self).__init__(args, SSEHelixCorrespondenceForm)
        
        self.hasSemiAtomicPlacementForm = False

        self.structPred = StructurePrediction.load(self.args.sequence, self)
        
        self.skeleton = SkeletonViewer(self)
        self.sse    = SSEViewer(self)
        self.calpha = CAlphaViewer(self)

        self.mainCamera.append_scenes([self.skeleton, self.sse, self.calpha])
        
        self.form.createUI()

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon SSE Correspondence Gui"
        parser.add_argument('skeleton')
        parser.add_argument('sequence')
        parser.add_argument('helix')

    def load(self):
        self.skeleton.load(self.args.skeleton)
        self.sse.loadHelixDataFromFile(self.args.helix)

        self.form.ui.lineEditSkeletonFile.setText(self.args.skeleton)
        self.form.ui.lineEditSequenceFile.setText(self.args.sequence)
        self.form.ui.lineEditHelixLocationFile.setText(self.args.helix)

        self.form.checkOk()
        self.form.ui.pushButtonOk.setEnabled(True)
        self.form.ui.tabWidget.setCurrentIndex(4)
