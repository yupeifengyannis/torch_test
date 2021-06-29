#!/bin/bash
BUILD_DIR=build
BUILD_TYPE=Debug

TORCH_PATH=`python -c "import torch; print(torch.utils.cmake_prefix_path)"`
echo $TORCH_PATH

if [ ! -d ${BUILD_DIR} ]; then
  mkdir $BUILD_DIR
fi
pushd ${BUILD_DIR}
  cmake -DCMAKE_PREFIX_PATH=$TORCH_PATH \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
  ..
  make -j8
popd
