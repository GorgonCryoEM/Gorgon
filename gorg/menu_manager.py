class MenuManager(object):
    def __init__(self, main, parent=None):
        self.app = main
        self.menuList = {}

    def getMenu(self, tag):
        if(tag in self.menuList) :
            return self.menuList[tag]
        else :
            raise Exception("Attempting to use non-existant menu item: " + tag)
        
    def addMenu(self, tag, text, parentTag=""):        
        if (parentTag == "") :
            parentMenu = self.app.menuBar()
        else :
            parentMenu = self.getMenu(parentTag) 
            
        self.menuList[tag] = parentMenu.addMenu(text)
        return self.menuList[tag]
    
    def addAction(self, tag, action, parentTag=""):
        if (parentTag == "") :
            parentMenu = self.app.menuBar()
        else :
            parentMenu = self.getMenu(parentTag)         
        parentMenu.addAction(action)
