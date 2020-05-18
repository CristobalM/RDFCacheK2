#!/bin/bash


exit_on_error() {
    exit_code=$1
    last_command=${@:2}
    if [ $exit_code -ne 0 ]; then
        >&2 echo "\"${last_command}\" command failed with exit code ${exit_code}."
        exit $exit_code
    fi
}

# enable !! command completion
set -o history -o histexpand

mkdir -p lib
cd lib

# Begin boost.endian
BOOST_ENDIAN_FOLDER="boost.endian-boost-1.69.0"
if [[ -d ${BOOST_ENDIAN_FOLDER} ]]; then
    cd ${BOOST_ENDIAN_FOLDER}
else
    if ! (wget https://github.com/glenfe/boost.endian/archive/boost-1.69.0.tar.gz) then
        echo "Couldn't retrieve boost.endian repository.. exiting"
        exit 1
    fi
    tar -zxvf boost-1.69.0.tar.gz
    rm -rf boost-1.69.0.tar.gz
    cd ${BOOST_ENDIAN_FOLDER}
fi
