from form import Form
from ui_binary import Ui_DialogVolumeBinarySkeletonization


class VolumeBinarySkeletonizationForm(Form):

    def __init__(self, parent):
        super(VolumeBinarySkeletonizationForm, self).__init__(parent, Ui_DialogVolumeBinarySkeletonization, "Binary")
