#!/usr/bin/python


class ObservedSheet:
  #def __init__(self, params):

  def __init__(self, label, cornerList):
    #raise NotImplementedError, 'Not Yet Implemented: ObservedSheet contructor'
    """Note that the label is currently an integer, not a string."""
    self.cornerList = cornerList
    self.label = label #Note: currently an integer, not a string
    self.sseType = 'sheet'

  def __parseSheet(self, inputString):
    pass
