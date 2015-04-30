# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   An exception when the chain model does not match the sequence


class CAlphaSequenceError(Exception):
    def __str__(self):
        return "Chain model does not match the sequence"
