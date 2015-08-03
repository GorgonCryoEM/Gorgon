# Template Plugin
# This is a plugin which acts as both a template plugin as well as a update notifier which looks for
# newer versions of Gorgon on application load. 


import sys, os
import urllib
from base_plugin import BasePlugin
from PyQt4 import QtGui

class UpdateNotifierPlugin(BasePlugin):  #Plugins must inherit the BasePlugin class

    # Override this method and put in your initializers.
    # Do not call the base class method
    def __init__(self, main, parent=None):
        self.app = main
        self.checkCurrentVersion()
   
    # Override this method to return a unique id for your plugin
    # It is recommended that you use the Uppercase version of the filename.   
    # Do not call the base class method
    def getId(self):
        return "UPDATE_NOTIFIER_PLUGIN"
   
    # Override this method to return a unique name for the plugin
    # Do not call the base class method
    def getName(self):
        return "Update Notifier Plugin"
   
    # Override this method to return a description of your plugin
    # Do not call the base class method
    def getDescription(self):
        return "This notifies the user if a newer version of Gorgon is available to download"
    
    
    def checkCurrentVersion(self):
        try:
            versionFile = urllib.urlopen("http://gorgon.wustl.edu/resources/latestVersion.txt")
            version = str.strip(versionFile.read()).split(" ")           
            versionFile.close()
        except:
            version = ['GORGON', self.app.version]
        
        #Notify for update only if internet connection is present, and user has older Gorgon version
        if ((not self.app.isNightlyBuild) and (version[0] == 'GORGON') and (version[1] != self.app.version)):
            if (QtGui.QMessageBox.information(self.app, "Update Available", "A newer version of Gorgon is available for download.", "Get update...", "Remind me later") == 0):
                self.app.actions.getAction("show_GetUpdatesForm").trigger()
                
            
        

        