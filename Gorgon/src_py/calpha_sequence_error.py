# Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
# Author:        Ross A. Coleman (racolema@bcm.edu)
# Description:   An exception when the chain model does not match the sequence

# CVS Meta Information: 
#   $Source: /project/mm/cvs/graphics/ssa1/source/Gorgon/src_py/calpha_sequence_error.py,v $
#   $Revision: 1.1 $
#   $Date: 2009/03/31 21:40:13 $
#   $Author: ssa1 $
#   $State: Exp $
#
# History Log: 
#   $Log: calpha_sequence_error.py,v $
#   Revision 1.1  2009/03/31 21:40:13  ssa1
#   Refactoring: Splitting seq_model\SequenceView.py into subclasses
#

class CAlphaSequenceError(Exception):
    def __str__(self):
        return "Chain model does not match the sequence"