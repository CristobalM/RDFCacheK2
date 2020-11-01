#!/bin/bash

export CC=/usr/bin/gcc
export CXX=/usr/bin/g++

_TMP_DIR_0="_tmp_0"
base_dir=$(pwd)
function _clean(){
    cd $base_dir
    rm -rf $_TMP_DIR_0
}

function _build(){
_clean
cd $base_dir
mkdir -p $_TMP_DIR_0
cd $_TMP_DIR_0
cmake .. > /dev/null 2> /dev/null
make -j4 > /dev/null 2> /dev/null
return $?
}


_build
if [ $? -eq 0 ]; then
    echo "Works with GNU Compiler"
else
    echo "Doesn't work with GNU Compiler"
fi

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

_build
if [ $? -eq 0 ]; then
    echo "Works with CLANG Compiler";
else
    echo "Doesn't work with CLANG Compiler";
fi

_clean
cd $base_dir
