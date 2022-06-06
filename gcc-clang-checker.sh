#!/bin/bash

export CC=/usr/bin/gcc
export CXX=/usr/bin/g++

HN=$(echo $(($(nproc)/2)))

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

cmake .. > /dev/null &> /dev/null
RES=$?

if [ $RES -ne 0 ]; then
	return $RES
fi

make -j$HN > /dev/null &> /dev/null
RES=$?

if [ $RES -ne 0 ]; then
	return $RES
fi

ctest -j$HN . &> /dev/null

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
    echo "works with clang compiler";
else
    echo "doesn't work with clang compiler";
fi

_clean
cd $base_dir
