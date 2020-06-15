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
cd lib || (echo "cant enter lib" && exit)

# Begin libCSD
LIB_CSD="libCSD"
if [[ -d ${LIB_CSD} ]]; then
    cd ${LIB_CSD}
    git fetch
    git pull
else
    if ! (git clone https://github.com/CristobalM/libCSD) then
        echo "Couldn't retrieve libCSD repository.. exiting"
        exit 1
    fi
    cd ${LIB_CSD}
fi

make
cd ..

