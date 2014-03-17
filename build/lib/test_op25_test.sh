#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/luizrenault/new/gr-op25/lib
export PATH=/home/luizrenault/new/gr-op25/build/lib:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-op25 
