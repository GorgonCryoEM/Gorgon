#!/usr/bin/python

# Class:  Secel
# Class Description:
#                    More info in: seq_model-doc.txt


try:
  from PyQt4 import QtGui
  qtEnabled=True
except:
  qtEnabled=False

from seq_model.Secel import Secel


class Coil(Secel):

  def __init__(self, chain, serialNo, label, startIndex, stopIndex, color=None):
    if qtEnabled and color==None:
      color=QtGui.QColor(60,60,60)
    Secel.__init__(self, chain, serialNo, label, startIndex, stopIndex, color)
    self.type="loop"

  def toPDB(self):
    return ''
