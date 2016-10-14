#!/bin/sh

build_dir="${SRC_DIR}/../build_dir"

rm -f $build_dir/CMakeCache.txt
mkdir -p $build_dir
cd $build_dir

cmake $SRC_DIR
cmake --build ${build_dir} --config Release --target install -- -j

$PYTHON setup.py install
