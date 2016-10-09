#!/bin/sh

unset MACOSX_DEPLOYMENT_TARGET

build_dir="${SRC_DIR}/../build_dir"

rm -rf $build_dir
mkdir -p $build_dir
cd $build_dir

cmake $SRC_DIR
cmake --build ${build_dir} --config Release --target install -- -j

$PYTHON setup.py install
