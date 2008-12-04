#!/bin/bash
export PATH=`pwd`:$PATH
export PYTHONPATH=`pwd`:`pwd`/site-packages:$PYTHONPATH
export LD_LIBRARY_PATH=`pwd`:$LD_LIBRARY_PATH
python gorgon.pyw &
