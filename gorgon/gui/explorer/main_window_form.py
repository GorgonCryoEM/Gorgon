from ..window import Window


class MainWindowForm(Window):

    def __init__(self, args):
        super(MainWindowForm, self).__init__(args, None)

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
