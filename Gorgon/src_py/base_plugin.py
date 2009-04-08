# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that manages all the windows of the main form. 

# CVS Meta Information: 
#   $Source$
#   $Revision$
#   $Date$
#   $Author$
#   $State$
#
# History Log: 
#   $Log$
#   Revision 1.1  2009/04/08 19:54:59  ssa1
#   Adding in plugin functionality
#


class BasePlugin(object):
    
    def __init__(self, application, parent=None):
        raise NotImplementedError, "Plugins must implement the __init__(self, application, parent=None) method"
    
    def getId(self):
        raise NotImplementedError, "Plugins must implement the getId(self) method"
    
    def getName(self):
        return NotImplementedError, "Plugins must implement the getName(self) method"
    
    def getDescription(self):
        return NotImplementedError, "Plugins must implement the getDescription(self) method"