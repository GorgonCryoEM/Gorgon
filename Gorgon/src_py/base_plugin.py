# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   The base plugin.  All plugins must be derived from this class for gorgon to load them in runtime

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/base_plugin.py,v $
#   $Revision: 1.3 $
#   $Date: 2009/04/08 20:07:24 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: base_plugin.py,v $
#   Revision 1.3  2009/04/08 20:07:24  ssa1
#   *** empty log message ***
#
#   Revision 1.2  2009/04/08 20:06:31  ssa1
#   Adding in plugin functionality
#
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