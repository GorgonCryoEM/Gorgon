# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
# Description:   A widget that manages all the windows of the main form. 


import inspect
import sys, os
from base_plugin import BasePlugin
from plugin_browser_form import PluginBrowserForm

class PluginManager(object):

    def __init__(self, main, parent=None):
        self.app = main
        self.pluginBrowser = PluginBrowserForm(self.app)
        dirs = self.findPluginDirectories()
        for dir in dirs:
            self.loadPlugins(dir)  

    
    def findPluginDirectories(self):
        plugindirs = []
        directories = [os.path.abspath(os.path.dirname(sys.argv[0]))]
        visited = {}
        for directory in filter(os.path.isdir, directories):            
            fullpath = os.path.abspath(directory)           
            for file in os.listdir(fullpath):
                if file.lower() == "plugins" :
                    fullpath = os.path.join(fullpath, file)
                    if (not visited.has_key(fullpath)) and (os.path.isdir(fullpath)):
                        plugindirs.append(fullpath)
                        visited[fullpath] = None
        return plugindirs
    
    
    def loadPlugins(self, directory):
        filesToLoad = {}
        for file in os.listdir(directory) :            
            f, e = os.path.splitext(file)            
            if (e.lower() == ".py") or (e.lower() == ".pyc"):
                filesToLoad[f] = None                           
        
        for f in filesToLoad :       
            try:
                importedModule = __import__("plugins." + f, globals(), locals(), ["plugins"])

                for name in dir(importedModule):
                    obj = getattr(importedModule, name)
                                        
                    if inspect.isclass(obj) and issubclass(obj, BasePlugin) and (obj.__name__ != BasePlugin.__name__):
                        try:
                            i = obj(self.app)
                            if(not self.app.plugins.has_key(i.getId())) :                                    
                                self.app.plugins[i.getId()] = i
                                self.pluginBrowser.addLoadedPlugin(i.getId(), i.getName(), i.getDescription())
                            else:                                    
                                self.pluginBrowser.addFailedPlugin(f, directory, obj.__name__, "Plugin with same id:" + i.getId() + " exists")
                                del i                                    
                        except:
                            [type, value, traceback] = sys.exc_info()
                            self.pluginBrowser.addFailedPlugin(f, directory, obj.__name__, str(value))
            except:
                [type, value, traceback] = sys.exc_info()
                self.pluginBrowser.addFailedModule(f, directory, str(value))
        

                
                
                            

                    

        
