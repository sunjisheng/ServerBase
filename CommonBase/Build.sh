#!/bin/bash
echo "Build CommonBase"
cd `dirname $0`
if [ ! -d build ]; then
    mkdir build
else
    echo "directory build already exist"
fi
cd build
cmake ..
make && make install
exit $?
