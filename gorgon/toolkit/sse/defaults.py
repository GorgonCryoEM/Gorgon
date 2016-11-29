class ComboBox(object):

    def __init__(self):
        self.ind = -1
        self.arr = []
        
    def currentIndex(self):
        return self.ind

    def clear(self):
        self.ind = -1
        self.arr = []
        
    def addItem(self, item):
        self.arr.append(item)


class SSEDefaults(object):

    def __init__(self):
        self.correspondences = ComboBox()
