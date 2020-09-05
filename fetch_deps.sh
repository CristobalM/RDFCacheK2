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

cd ..


LIB_CPPBASE64="cpp-base64"
if [[ -d ${LIB_CPPBASE64} ]]; then
    cd ${LIB_CPPBASE64}
    git fetch
    git pull
else
    if ! (git clone https://github.com/ReneNyffenegger/cpp-base64) then
        echo "Couldn't retrieve ${LIB_CPPBASE64} repository.. exiting"
        exit 1
    fi
    cd ${LIB_CPPBASE64}
fi

cd ..


LIB_NTPARSER="ntparser"
if [[ -d ${LIB_NTPARSER} ]]; then
    cd ${LIB_NTPARSER}
    git fetch
    git pull
else
    if ! (git clone https://github.com/CristobalM/${LIB_NTPARSER}) then
        echo "Couldn't retrieve ${LIB_NTPARSER} repository.. exiting"
        exit 1
    fi
    cd ${LIB_NTPARSER}
fi

cd ..