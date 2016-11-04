from ..window import Window
from ..sse.viewer import SSEViewer


class MainWindowForm(Window):

    def __init__(self, args):
        super(MainWindowForm, self).__init__(args, None)

        self.sse    = SSEViewer(self)

        self.scene.append(self.sse)

    @classmethod
    def set_parser(cls, parser):
        parser.description = "Gorgon Explorer"
        parser.add_argument('volume')
        parser.add_argument('skeleton')
        parser.add_argument('sequence')
        parser.add_argument('helix')
        parser.add_argument('calpha')

    def load(self):
        super(MainWindowForm, self).load()

        self.skeleton.load(self.args.skeleton)
        self.sse.loadHelixDataFromFile(self.args.helix)
