# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   The base plugin.  All plugins must be derived from this class for gorgon to load them in runtime



class BasePlugin(object):
    
    def __init__(self, application, parent=None):
        raise NotImplementedError, "Plugins must implement the __init__(self, application, parent=None) method"
    
    def getId(self):
        raise NotImplementedError, "Plugins must implement the getId(self) method"
    
    def getName(self):
        return NotImplementedError, "Plugins must implement the getName(self) method"
    
    def getDescription(self):
        return NotImplementedError, "Plugins must implement the getDescription(self) method"
