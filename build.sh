#!/bin/bash
BUILD_DIR=build
BUILD_TYPE=Debug

CURRENT_DIR=`realpath $(dirname $0)`
echo "CURRENT_DIR is ${CURRENT_DIR}"

if [ ! -f ${CURRENT_DIR}/thirdparty/googletest/CMakeLists.txt ]; then
  git submodule update --init --recursive
fi

TORCH_PATH=`python -c "import torch; print(torch.utils.cmake_prefix_path)"`
echo "TORCH PATH is ${TORCH_PATH}"

if [ ! -d ${BUILD_DIR} ]; then
  mkdir $BUILD_DIR
fi
pushd ${BUILD_DIR}
  cmake -DCMAKE_PREFIX_PATH=$TORCH_PATH \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
  ..
  make -j8
popd
