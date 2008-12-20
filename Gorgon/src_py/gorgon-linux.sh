#!/bin/bash
export PATH=`pwd`/bin:$PATH
export PYTHONPATH=`pwd`:`pwd`/lib:`pwd`/lib/python2.5:`pwd`/lib/python2.5/site-packages
export LD_LIBRARY_PATH=`pwd`/lib
python gorgon.pyw
